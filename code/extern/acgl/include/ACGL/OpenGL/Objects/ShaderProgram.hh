/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_SHADERPROGRAM_HH
#define ACGL_OPENGL_OBJECTS_SHADERPROGRAM_HH

/**
 * A ShaderProgram is a wrapper around an OpenGL Program: A combination of Shaders
 * that are linked together to controll the programmable pipeline stages.
 *
 * A ShaderProgram is still quite low-level and just wraps the OpenGL object itself.
 *
 * One note on uniforms:
 * There are basically four ways to set uniform values here:
 *
 * setUniform(        GLint       _location, VALUE );
 * setUniform(        std::string _location, VALUE );
 * setProgramUniform( GLint       _location, VALUE );
 * setProgramUniform( std::string _location, VALUE );
 *
 * The versions with a std::string as a location are easy to use, just provide the name
 * the uniform is called in the shaderfile. But it will have to query the uniform location
 * each call and thus is inefficient! It would be faster to query the location once using
 * getUniformLocation( std::string ); and use the returned value in combination with the
 * set*Uniform( GLint, ...) versions.
 *
 * Both are provided as setUniform and setProgramUniform:
 * In order for setUniform(...) to work as intendet the ShaderProgram has to be active ( use() ),
 * setProgramUniform(...) does not have this limitation and is based on direct state access
 * (via an extension or a basic simulation of the extension).
 * Use setProgramUniform if you can't know which program is in use right now, setUniform should
 * be prefered for performance critical parts of your app.
 *
 * In short: setProgramUniform( std::string _location, VALUE ); is the most error proof option
 *           and good for testing out new stuff
 *           setUniform( GLint _location, VALUE ); is best for performance critical code thats
 *           well tested
 *
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Objects/Shader.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>
#include <ACGL/OpenGL/Objects/TextureBuffer.hh>
#include <ACGL/OpenGL/Data/LocationMappings.hh>
#include <ACGL/Math/Math.hh>

#include <vector>

namespace ACGL{
namespace OpenGL{

class ShaderProgram
{
    ACGL_NOT_COPYABLE(ShaderProgram)

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< ConstSharedShader > ConstSharedShaderVec;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    ShaderProgram(void)
    :   mObjectName(0),
        mShaders()
    {
        mObjectName = glCreateProgram();
    }

    virtual ~ShaderProgram(void)
    {
        // "DeleteProgram will silently ignore the value zero." - GL Spec
        glDeleteProgram(mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_PROGRAM>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_PROGRAM>(getObjectName()); }
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGLES_VERSION >= 10))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_PROGRAM_OBJECT_EXT>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_PROGRAM_OBJECT_EXT>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline       GLuint                getObjectName(void) const { return mObjectName; }
    inline const ConstSharedShaderVec& getShaders   (void) const { return mShaders;    }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:
    //! use, or activate it for rendering, also needed to set uniforms:
    inline void use(void) const { glUseProgram(mObjectName); }

    //! attach a single shader, don't forget to relink!
    inline void attachShader(const ConstSharedShader& _shader)
    {
        mShaders.push_back(_shader);
        glAttachShader( mObjectName, _shader->getObjectName() );
    }

    //! link the program, has to be redone after changing input or output locations:
    bool link (void) const;

#if (ACGL_OPENGL_VERSION >= 41)
    //! set the program separable to be used in a program pipeline object
    inline bool setSeparable( GLboolean _b = GL_TRUE ) {
        glProgramParameteri( mObjectName, GL_PROGRAM_SEPARABLE, _b );
        return link();
    }
#endif

    // ===================================================================================================== \/
    // =========================================================================================== LOCATIONS \/
    // ===================================================================================================== \/
public:
    //////////// uniform (block) locations:
	inline GLint getUniformLocation(const std::string& _nameInShader) const 
	{
		// Cache uniform location
		if ( !mUniformLocationCache.count(_nameInShader) ) 
			mUniformLocationCache[_nameInShader] = glGetUniformLocation(mObjectName, _nameInShader.c_str()); 
		return mUniformLocationCache[_nameInShader];
	}

#if (ACGL_OPENGL_VERSION >= 31)
    //! if the block name does not exist, GL_INVALID_INDEX will get returned
    inline GLuint getUniformBlockIndex (const std::string& _nameInShader) const { return glGetUniformBlockIndex(mObjectName, _nameInShader.c_str()); }

    //! binds a uniform block, the string version will ignore a non-existent block
    inline void   setUniformBlockBinding( GLuint            _blockIndex, GLuint _bindingPoint ) const { glUniformBlockBinding( mObjectName, _blockIndex, _bindingPoint ); }
    inline void   setUniformBlockBinding( const std::string& _blockName, GLuint _bindingPoint ) const {
        GLuint blockIndex = getUniformBlockIndex(_blockName);
        if (blockIndex != GL_INVALID_INDEX) glUniformBlockBinding( mObjectName, blockIndex, _bindingPoint );
    }

    GLint getUniformBlockBinding( const std::string& _blockName ) const { return getUniformBlockBinding( getUniformBlockIndex(_blockName)); }
    GLint getUniformBlockBinding( GLuint            _blockIndex ) const {
        GLint bindingPoint;
        glGetActiveUniformBlockiv( mObjectName, _blockIndex, GL_UNIFORM_BLOCK_BINDING, &bindingPoint );
        return bindingPoint;
    }

    //! returns a mapping from the uniforms in a given block to the offset within the block
    SharedLocationMappings getUniformOffsetsOfBlock( const std::string &_blockName  ) const { return getUniformOffsetsOfBlock(getUniformBlockIndex(_blockName)); }
    SharedLocationMappings getUniformOffsetsOfBlock( GLuint             _blockIndex ) const;

    //! returns the size in bytes of the uniform block, can be used to allocate the right amount of memory
    GLsizeiptr getUniformBlockSize( const std::string &_blockName  ) const { return getUniformBlockSize(getUniformBlockIndex(_blockName)); }
    GLsizeiptr getUniformBlockSize( GLuint             _blockIndex ) const ;

#endif // OpenGL >= 3.1

    //////////// attribute locations:
    inline GLint getAttributeLocation  (const std::string& _nameInShader) const { return glGetAttribLocation  (mObjectName, _nameInShader.c_str()); }
    inline void bindAttributeLocation  (const std::string& _nameInShader, GLuint _location) const { glBindAttribLocation   (mObjectName, _location, _nameInShader.c_str()); }

    //! Sets the attribute locations of this ShaderProgram according to the mappings specified in _locationMappings
    void setAttributeLocations( ConstSharedLocationMappings _locationMappings );
    //! Get all attribute names with there locations:
    SharedLocationMappings getAttributeLocations() const;

    //////////// fragdata locations:
#if (ACGL_OPENGL_VERSION >= 30)
    //! if the location does not exist, -1 will get returned
    inline GLint getFragmentDataLocation (const std::string& _nameInShader) const { return glGetFragDataLocation(mObjectName, _nameInShader.c_str()); }
    inline void bindFragmentDataLocation (const std::string& _nameInShader, GLuint _location) const { glBindFragDataLocation (mObjectName, _location, _nameInShader.c_str()); }

    //! Sets the fragment data locations of this ShaderProgram according to the mappings specified in
    void setFragmentDataLocations( ConstSharedLocationMappings _locationMappings );
    //! Get all fragdata names with there locations:
    SharedLocationMappings getFragmentDataLocations();
#endif // OpenGL >= 3.0

    // ===================================================================================================== \/
    // ============================================================================================ UNIFORMS \/
    // ===================================================================================================== \/
public:
    // int by location
    inline void setUniform (GLint _location, GLint _v)             const { glUniform1i (_location, _v); }
    inline void setUniform (GLint _location, GLsizei _n, GLint *_v)const { glUniform1iv(_location, _n, _v); }
    inline void setUniform (GLint _location, const glm::ivec2& _v) const { glUniform2iv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::ivec3& _v) const { glUniform3iv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::ivec4& _v) const { glUniform4iv(_location, 1, glm::value_ptr(_v)); }

#if (ACGL_OPENGL_VERSION >= 30)
    // unsigned int by location
    inline void setUniform (GLint _location, GLuint _v)            const { glUniform1ui (_location, _v); }
    inline void setUniform (GLint _location, GLsizei _n, GLuint*_v)const { glUniform1uiv(_location, _n, _v); }
    inline void setUniform (GLint _location, const glm::uvec2& _v) const { glUniform2uiv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::uvec3& _v) const { glUniform3uiv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::uvec4& _v) const { glUniform4uiv(_location, 1, glm::value_ptr(_v)); }

#endif // OpenGL >= 3.0

    // float by location
    inline void setUniform (GLint _location, GLfloat _v)          const { glUniform1f (_location, _v); }
    inline void setUniform (GLint _location, GLsizei _n, GLfloat* _v)const { glUniform1fv(_location, _n, _v); }

    inline void setUniform (GLint _location, GLsizei _n, glm::vec2* _v)const{ glUniform2fv(_location, _n, (GLfloat*)_v); }
    inline void setUniform (GLint _location, const glm::vec2& _v) const { glUniform2fv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::vec3& _v) const { glUniform3fv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::vec4& _v) const { glUniform4fv(_location, 1, glm::value_ptr(_v)); }
#if (ACGL_OPENGL_VERSION >= 40)

    // double by location
    inline void setUniform (GLint _location, GLdouble _v)          const { glUniform1d (_location, _v); }
    inline void setUniform (GLint _location, GLsizei _n, GLdouble*_v)const{ glUniform1dv(_location, _n, _v); }
    inline void setUniform (GLint _location, const glm::dvec2& _v) const { glUniform2dv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dvec3& _v) const { glUniform3dv(_location, 1, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dvec4& _v) const { glUniform4dv(_location, 1, glm::value_ptr(_v)); }
#endif // OpenGL >= 4.0

    // float matrix by location
#ifndef ACGL_OPENGLES_VERSION_20
    // ES 2 only has square matrices, so omit these:
    inline void setUniform (GLint _location, const glm::mat2x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x3fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat2x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x4fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x2fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x4fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x2fv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x3fv(_location, 1, _transpose, glm::value_ptr(_v)); }
#endif
    inline void setUniform (GLint _location, const glm::mat2&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2fv  (_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat3&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3fv  (_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::mat4&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4fv  (_location, 1, _transpose, glm::value_ptr(_v)); }

    // float matrix array by location
#ifndef ACGL_OPENGLES_VERSION_20
    // ES 2 only has square matrices, so omit these:
    inline void setUniform (GLint _location, const glm::mat2x3* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x3fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat2x4* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x4fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat3x2* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x2fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat3x4* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x4fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat4x2* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x2fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat4x3* _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x3fv(_location, _n, _transpose, glm::value_ptr(*_v)); }
#endif
    inline void setUniform (GLint _location, const glm::mat2*   _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2fv  (_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat3*   _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3fv  (_location, _n, _transpose, glm::value_ptr(*_v)); }
    inline void setUniform (GLint _location, const glm::mat4*   _v, GLsizei _n, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4fv  (_location, _n, _transpose, glm::value_ptr(*_v)); }
    
#if (ACGL_OPENGL_VERSION >= 40)
    // double matrix by location
    inline void setUniform (GLint _location, const glm::dmat2&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat2x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x3dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat2x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix2x4dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat3x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x2dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat3&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3dv  (_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat3x4& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix3x4dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat4x2& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x2dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat4x3& _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4x3dv(_location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setUniform (GLint _location, const glm::dmat4&   _v, GLboolean _transpose = GL_FALSE) const { glUniformMatrix4dv  (_location, 1, _transpose, glm::value_ptr(_v)); }
#endif // OpenGL >= 4.0

    //! sets a texture uniform to a given texture unit and also binds the texture to the same unit
    inline void setTexture (GLint _location,                  const ConstSharedTextureBase& _texture,   GLint _unit) const { glUniform1i(_location, _unit);                                      _texture->bind(_unit); }
    inline void setTexture (const std::string& _nameInShader, const ConstSharedTextureBase& _texture,   GLint _unit) const { setUniform( getUniformLocation(_nameInShader), (GLint) _unit);      _texture->bind(_unit); }
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_BUFFER )
    inline void setTexture (const std::string& _nameInShader, const ConstSharedTextureBuffer& _texture, GLint _unit) const { setUniform( getUniformLocation(_nameInShader), (GLint) _unit);      _texture->bindTexture(_unit); }
#endif
    
    #if (ACGL_OPENGL_VERSION >= 41)
    // DSA versions:
    
    // int DSA by location
    inline void setProgramUniform (GLint _location, GLint _v)             const { glProgramUniform1i (mObjectName, _location, _v); }
    inline void setProgramUniform (GLint _location, GLsizei _n, GLint *_v)const { glProgramUniform1iv(mObjectName, _location, _n, _v); }
    inline void setProgramUniform (GLint _location, const glm::ivec2& _v) const { glProgramUniform2iv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::ivec3& _v) const { glProgramUniform3iv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::ivec4& _v) const { glProgramUniform4iv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    
    // unsigned int DSA by location
    inline void setProgramUniform (GLint _location, GLuint _v)            const { glProgramUniform1ui (mObjectName, _location, _v); }
    inline void setProgramUniform (GLint _location, GLsizei _n, GLuint*_v)const { glProgramUniform1uiv(mObjectName, _location, _n, _v); }
    inline void setProgramUniform (GLint _location, const glm::uvec2& _v) const { glProgramUniform2uiv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::uvec3& _v) const { glProgramUniform3uiv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::uvec4& _v) const { glProgramUniform4uiv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    
    // float DSA by location
    inline void setProgramUniform (GLint _location, GLfloat _v)          const { glProgramUniform1f (mObjectName, _location, _v); }
    inline void setProgramUniform (GLint _location, GLsizei _n, GLfloat*_v)const{ glProgramUniform1fv(mObjectName, _location, _n, _v); }
    inline void setProgramUniform (GLint _location, const glm::vec2& _v) const { glProgramUniform2fv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::vec3& _v) const { glProgramUniform3fv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::vec4& _v) const { glProgramUniform4fv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    
    // double DSA by location
    inline void setProgramUniform (GLint _location, GLdouble _v)          const { glProgramUniform1d (mObjectName, _location, _v); }
    inline void setProgramUniform (GLint _location, GLsizei _n, GLdouble*_v)const{glProgramUniform1dv(mObjectName, _location, _n, _v); }
    inline void setProgramUniform (GLint _location, const glm::dvec2& _v) const { glProgramUniform2dv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dvec3& _v) const { glProgramUniform3dv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dvec4& _v) const { glProgramUniform4dv(mObjectName, _location, 1, glm::value_ptr(_v)); }
    
    // float matrix DSA by location
    inline void setProgramUniform (GLint _location, const glm::mat2&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2fv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat2x3& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2x3fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat2x4& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2x4fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat3x2& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3x2fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat3&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3fv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat3x4& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3x4fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat4x2& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4x2fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat4x3& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4x3fv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::mat4&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4fv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }

    
    // double matrix DSA by location
    inline void setProgramUniform (GLint _location, const glm::dmat2&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2dv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat2x3& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2x3dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat2x4& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix2x4dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat3x2& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3x2dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat3&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3dv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat3x4& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix3x4dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat4x2& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4x2dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat4x3& _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4x3dv(mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }
    inline void setProgramUniform (GLint _location, const glm::dmat4&   _v, GLboolean _transpose = GL_FALSE) const { glProgramUniformMatrix4dv  (mObjectName, _location, 1, _transpose, glm::value_ptr(_v)); }

    inline void setProgramTexture (GLint _location,                  const ConstSharedTextureBase& _texture, GLint _unit) const { glProgramUniform1i(mObjectName, _location, _unit);                    _texture->bind(_unit); }
    inline void setProgramTexture (const std::string& _nameInShader, const ConstSharedTextureBase& _texture, GLint _unit) const { setProgramUniform( getUniformLocation(_nameInShader), (GLint) _unit); _texture->bind(_unit); }

#endif

    // ======================================================================================================= \/
    // ============================================================================================ HIGH LEVEL \/
    // ======================================================================================================= \/
public:
    // normal:
    template <typename T>
    inline void setUniform (const std::string& _nameInShader, T _v) const
    {
        setUniform( getUniformLocation(_nameInShader), _v);
    }

    // DSA:
    template <typename T>
    inline void setProgramUniform (const std::string& _nameInShader, T _v) const
    {
        setProgramUniform( getUniformLocation(_nameInShader), _v);
    }

    // normal for arrays:
    template <typename T>
    inline void setUniform (const std::string& _nameInShader, GLsizei _n, T _v) const
    {
        setUniform( getUniformLocation(_nameInShader), _n, _v);
    }

    // DSA for arrays:
    template <typename T>
    inline void setProgramUniform (const std::string& _nameInShader, GLsizei _n, T _v) const
    {
        setProgramUniform( getUniformLocation(_nameInShader), _n, _v);
    }

    // normal for matrices with additional transpose parameter
    template <typename T>
    inline void setUniform (const std::string& _nameInShader, T _v, GLboolean _transpose) const
    {
        setUniform( getUniformLocation(_nameInShader), _v, _transpose);
    }

    // DSA for matrices with additional transpose parameter
    template <typename T>
    inline void setProgramUniform (const std::string& _nameInShader, T _v, GLboolean _transpose) const
    {
        setProgramUniform( getUniformLocation(_nameInShader), _v, _transpose);
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLuint               mObjectName;
    ConstSharedShaderVec mShaders;

	/// Cache for uniform locations
	mutable std::map<std::string, int> mUniformLocationCache;
};

ACGL_SMARTPOINTER_TYPEDEFS(ShaderProgram)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_SHADERPROGRAM_HH
