/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_UNIFORM_BUFFER_HH
#define ACGL_OPENGL_OBJECTS_UNIFORM_BUFFER_HH

/**
 * A uniform buffer is an OpenGL buffer object bound to a uniform buffer location.
 * It can be used:
   * provide the same set of uniforms to different ShaderPrograms without setting
     the values multiple times
   * set multiple uniform variables at once by mapping the buffer into the CPU
     memory, memset all values and unmap it (quickest way to change a lot of uniforms).
   * quick switching between multiple sets of uniforms (e.g. material properties).
 *
 * To be used, uniforms must be organized in a uniform block, this block has to be bound
 * to the same location the uniform buffer gets bound to (quite similar to textures).
 *
 * If only advantage one is requested, the individual offsets along with the uniform names
 * can be saved in a uniform buffer to set the uniforms by setUniform() as it would be done
 * for normal uniforms in a ShaderProgram.
 * Otherwise the exact memory layout must be known, which can be queried by OpenGL, but which
 * is also well defined in the case of std140-blocks (see OpenGL spec).
 *
 * In contrast to ShaderPrograms, nothing has to be activated before setUniform can be called
 * here.
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Objects/Buffer.hh>

#include <ACGL/OpenGL/Data/LocationMappings.hh>
#include <ACGL/Math/Math.hh>

#if (ACGL_OPENGL_VERSION >= 31)
namespace ACGL{
namespace OpenGL{

class ShaderProgram;

class UniformBuffer : public Buffer
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    UniformBuffer()
        : Buffer(GL_UNIFORM_BUFFER)
    {}

    UniformBuffer( SharedBufferObject _pBuffer )
         : Buffer(_pBuffer, GL_UNIFORM_BUFFER)
    {}

    //! inits the uniformbuffer to be used with the given shaderprogram, _uboName is the name of this buffer in the shader
    UniformBuffer( const ptr::shared_ptr<const ShaderProgram> &_shaderProgram, const std::string &_uboName );

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    /** returns the byte offset of a uniform within the buffer
     * Needed to upload the new value of the uniform.
     * Initially the UniformBuffer does not know any locations as it is just unstructured memory with no
     * intrinsic meaning, so the locations have to be queried from the matching ShaderProgram and provided
     * to the UniformBuffer. This is optional, the application can also know the layout and upload the data
     * without storing/querying the mappings to/from the buffer.
     */
    GLint getUniformOffset (const std::string& _nameInShader) const {
        if (!uniformNameToOffsetMap) return -1;
        return uniformNameToOffsetMap->getLocation(_nameInShader);
    }
    void setUniformOffsets (SharedLocationMappings _uniformNameToOffsetMap) { uniformNameToOffsetMap = _uniformNameToOffsetMap; }

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/
public:
    //! reserve a number of bytes on the GPU for uniforms
    inline void reserveMemory(GLsizeiptr _size){ setData( _size, NULL, GL_STREAM_DRAW ); }

    //! uniform setters for scalar types: can only work if the offset-uniformname mapping was set before via setUniformOffsets()
    inline void setUniform (const std::string &_nameInShader, GLfloat   _v) { setUniformScalar<GLfloat>  (_nameInShader, _v); }
    inline void setUniform (const std::string &_nameInShader, GLint     _v) { setUniformScalar<GLint>    (_nameInShader, _v); }
    inline void setUniform (const std::string &_nameInShader, GLuint    _v) { setUniformScalar<GLuint>   (_nameInShader, _v); }
    inline void setUniform (const std::string &_nameInShader, GLboolean _v) { setUniformScalar<GLboolean>(_nameInShader, _v); }
    inline void setUniform (const std::string &_nameInShader, GLdouble  _v) { setUniformScalar<GLdouble> (_nameInShader, _v); }

    //! asuming std140 layout, add padding:
    void setUniform(const std::string &_nameInShader, glm::mat2x2 _v) { setUniform(_nameInShader, glm::mat2x4(_v)); }
    void setUniform(const std::string &_nameInShader, glm::mat3x2 _v) { setUniform(_nameInShader, glm::mat3x4(_v)); }
    void setUniform(const std::string &_nameInShader, glm::mat4x2 _v) { setUniform(_nameInShader, glm::mat4x4(_v)); }
    void setUniform(const std::string &_nameInShader, glm::mat2x3 _v) { setUniform(_nameInShader, glm::mat2x4(_v)); }
    void setUniform(const std::string &_nameInShader, glm::mat3x3 _v) { setUniform(_nameInShader, glm::mat3x4(_v)); }
    void setUniform(const std::string &_nameInShader, glm::mat4x3 _v) { setUniform(_nameInShader, glm::mat4x4(_v)); }

    //! uniform setters for glm types: can only work if the offset-uniformname mapping was set before via setUniformOffsets()
    template <typename T>
    void setUniform (const std::string &_nameInShader, T _v) {
        GLint offset = getUniformOffset( _nameInShader );
        if (offset == -1) {
            // hack for MacOS bug:
            offset = getUniformOffset( mBlockName+"."+_nameInShader  );
            //ACGL::Utils::debug() << "testing " + mBlockName+"."+_nameInShader << std::endl;
        }
        if (offset == -1) {
            ACGL::Utils::error() << "UniformBuffer does not know uniform " << _nameInShader << std::endl;
            return;
        }
        setSubData( offset, sizeof(T), glm::value_ptr(_v) );
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
private:
    // template for scalar types: private as the setUniform() functions above map to this
    template <typename T>
    void setUniformScalar (const std::string &_nameInShader, T _v) {
        GLint offset = getUniformOffset( _nameInShader );
        if (offset == -1) {
            // hack for MacOS bug:
            offset = getUniformOffset( mBlockName+"."+_nameInShader  );
            //ACGL::Utils::debug() << "testing " + mBlockName+"."+_nameInShader << std::endl;
        }
        if (offset == -1) {
            ACGL::Utils::error() << "UniformBuffer does not know uniform " << _nameInShader << std::endl;
            return;
        }
        setSubData( offset, sizeof(T), &_v );
    }

    SharedLocationMappings uniformNameToOffsetMap;

public: // for mac bug
    std::string mBlockName;
};

ACGL_SMARTPOINTER_TYPEDEFS(UniformBuffer)

} // OpenGL
} // ACGL

#endif // OpenGL >= 3.1

#endif // ACGL_OPENGL_OBJECTS_UNIFORM_BUFFER_HH
