/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_TEXTURE_HH
#define ACGL_OPENGL_OBJECTS_TEXTURE_HH

/**
 * A Texture wrapps the OpenGL texture. To fill these with data from image files a
 * matching TextureControllerFile* is needed.
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Data/TextureData.hh>
#include <ACGL/Math/Math.hh>

#include <vector>

namespace ACGL{
namespace OpenGL{

// the combination of Image and SharedTextureBase will identify a Rendertarget etc.:
struct Image {
    Image() : mipmapLevel(0), layer(0), cubeMapFace(GL_INVALID_ENUM) {}
    unsigned int      mipmapLevel; // always 0 if texture has no mipmaps
    unsigned int      layer;       // array layer or slice in a 3D texture, always 0 if texture is 1D or 2D
    GLenum            cubeMapFace; // GL_INVALID_ENUM if texture is not a cube map
};

/**
 * A Texture consists of:
 *   1. 1..N Images (e.g. mipmap layers)
 *   2. an internal data format (datatype and number of channels on the GPU)
 *   3. a sampling mode (alternatively a sampler object can be used)
 *
 * The TextureBase class holds the methods to define 2 & 3, the Images (incl. getter and setter)
 * vary between different texture types, so subclasses will take care if that.
 *
 * Every subclass of TextureBase has:
   * A constructor which defines the internal format (data layout in GPU memory)
     (this can't be changed later)
   * A constructor that reserves memory for a certain texture size.
   * A setImageData with a SharedTextureData and optinally additional parameters
     about which layer/mipmap-level/slice etc. should be set

 * Some might also have:
   * A resize function which differs in the dimensionality from subclass to subclass.
 */
class TextureBase
{
    ACGL_NOT_COPYABLE(TextureBase)

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    //!
    /*!
        Default texture parameters taken from: http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml
     */
    TextureBase(GLenum _target)
    :   mObjectName(0),
        mTarget(_target),
        mWidth(0),
        mHeight(1),
        mDepth(1),
        mInternalFormat(GL_RGBA)
    {
        glGenTextures(1, &mObjectName);
    }

    TextureBase(GLenum _target, GLenum _internalFormat, GLuint _objectName = 0) 
      : mObjectName(_objectName),
      mTarget(_target),
      mWidth(0),
      mHeight(1),
      mDepth(1),
      mInternalFormat(_internalFormat) {
      if (mObjectName == 0) {
        glGenTextures(1, &mObjectName);
      }
    }

    virtual ~TextureBase(void)
    {
        // object name 0 will get ignored by OpenGL
        glDeleteTextures(1, &mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#ifdef ACGL_OPENGL_DEBUGGER_SUPPORT
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_TEXTURE>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_TEXTURE>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint     getObjectName     (void) const { return mObjectName;     }
    inline GLenum     getTarget         (void) const { return mTarget;         }
    inline GLsizei    getWidth          (void) const { return mWidth;          }
    inline GLsizei    getHeight         (void) const { return mHeight;         }
    inline GLsizei    getDepth          (void) const { return mDepth;          }
    inline GLenum     getInternalFormat (void) const { return mInternalFormat; }
    inline GLint      getMinFilter      (void) const { return getParameterI(GL_TEXTURE_MIN_FILTER); }
    inline GLint      getMagFilter      (void) const { return getParameterI(GL_TEXTURE_MAG_FILTER); }
#if (( ACGL_OPENGLES_VERSION > 20) || (ACGL_OPENGL_VERSION > 20))
    inline GLint      getBaseLevel      (void) const { return getParameterI(GL_TEXTURE_BASE_LEVEL); }
    inline GLint      getMaxLevel       (void) const { return getParameterI(GL_TEXTURE_MAX_LEVEL); }
    inline GLint      getMinLOD         (void) const { return getParameterI(GL_TEXTURE_MIN_LOD); }
    inline GLint      getMaxLOD         (void) const { return getParameterI(GL_TEXTURE_MAX_LOD); }
    inline GLenum     getCompareMode    (void) const { return (GLenum) getParameterI(GL_TEXTURE_COMPARE_MODE); }
    inline GLenum     getCompareFunc    (void) const { return (GLenum) getParameterI(GL_TEXTURE_COMPARE_FUNC); }
    inline GLenum     getWrapS          (void) const { return (GLenum) getParameterI(GL_TEXTURE_WRAP_S); }
    inline GLenum     getWrapT          (void) const { return (GLenum) getParameterI(GL_TEXTURE_WRAP_T); }
#endif
#if (ACGL_OPENGL_VERSION > 20)
    inline GLfloat    getLODBias        (void) const { return getParameterF(GL_TEXTURE_LOD_BIAS); }
    inline glm::vec4  getBorderColor    (void) const { return getParameter4F(GL_TEXTURE_BORDER_COLOR); }
#endif
    inline glm::uvec3 getSize           (void) const { return glm::uvec3( mWidth, mHeight, mDepth ); }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:
    //! Activate texture unit and bind this texture to it. _textureUnit starts at 0!
    inline void bind(GLuint _textureUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + _textureUnit);
        glBindTexture(mTarget, mObjectName);
    }

    //! Bind this texture to the currently active texture unit.
    inline void bind(void) const
    {
        glBindTexture(mTarget, mObjectName);
    }

    //! sets the minification filter
    void setMinFilter(GLint _value = GL_NEAREST_MIPMAP_LINEAR );

    //! sets the magnification filter
    void setMagFilter(GLint _value = GL_LINEAR);

#if (( ACGL_OPENGLES_VERSION > 20) || (ACGL_OPENGL_VERSION > 20))
    void setWrapS( GLenum _wrapS = GL_REPEAT );
    void setWrapT( GLenum _wrapT = GL_REPEAT );
    void setWrapR( GLenum _wrapR = GL_REPEAT );

    //! Note: The function will bind this texture!
    void setWrap(GLenum _wrapS, GLenum _wrapT = 0, GLenum _wrapR = 0);

    //! lowest defined mipmap level
    void setBaseLevel(  GLint _level = -1000 );

    //! highest defined mipmap level
    void setMaxLevel(  GLint _level =  1000 );

    //! lowest mipmap level to use
    void setMinLOD(  GLint _lod = -1000 );

    //! highest mipmap level to use
    void setMaxLOD(  GLint _lod =  1000 );

    //! for usage of a texture with depth data
    void setCompareMode( GLenum _mode = GL_NONE );

    //! for usage of a texture with depth data
    void setCompareFunc( GLenum _func = GL_LEQUAL );
#endif
    
#if (ACGL_OPENGL_VERSION > 20)
    //! offset to add to the mipmap level calculation
    void setLODBias( GLfloat _bias = 0.0f );
    
    //! color that is sampled outside range if wrap is set to GL_CLAMP_TO_BORDER
    void setBorderColor(const glm::vec4& _color = glm::vec4(0.0));
#endif
    
    //! _sampleCount = 1.0 to deactivate anisotrop filtering, maximum is often 16. If a value is too high it will get clamped to the maximum
    void setAnisotropicFilter( GLfloat _sampleCount );

    //! Generate mipmaps from the current base texture (i.e. the texture from level 0)
    void generateMipmaps(void);

    void resize(const glm::uvec3& _size) { resizeI(glm::uvec3(_size.x, _size.y, _size.z)); }
    void resize(const glm::uvec2& _size) { resizeI(glm::uvec3(_size.x, _size.y, 1      )); }
    void resize(const glm::uvec1& _size) { resizeI(glm::uvec3(_size.x, 1,       1      )); }
    void resize(      GLuint      _size) { resizeI(glm::uvec3(_size,   1,       1      )); }

#ifndef ACGL_OPENGLES_VERSION_20
    //! get one texture image:
    TextureData *getTextureImageRAW( const Image &_image = Image(), GLenum _format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE ) const;
    SharedTextureData getTextureImage( const Image &_image = Image(), GLenum _format = GL_RGBA, GLenum _type = GL_UNSIGNED_BYTE ) const {
        return SharedTextureData( getTextureImageRAW(_image, _format, _type) );
    }
#endif // ES 2

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLuint  mObjectName;

    // kind of texture data:
    GLenum  mTarget;
    GLsizei mWidth;  // width
    GLsizei mHeight; // height or 1 in case of a 1D texture
    GLsizei mDepth;  // depth or #of layer in a 2D array, 1 otherwise
    GLenum  mInternalFormat; // often used, so store it here

    // Checks what texture is currently bound at the texture target used by this texture
    // to be later used to restore that texture (to be side effect free). Then binds this texture.
    GLuint bindAndGetOldTexture() const;

    // generic get parameter functions:
    GLint     getParameterI ( GLenum _name ) const;
    GLfloat   getParameterF ( GLenum _name ) const;
    glm::vec4 getParameter4F( GLenum _name ) const;

    //! returns a format compatible with the internal, only used to
    //! reserve memory, not to upload actual data!
    GLenum getCompatibleFormat( GLenum _internalFormat );
    GLenum getCompatibleType(   GLenum _internalFormat );


    //! to be used by subclasses, will not check if usage is meaningfull for the
    //! texture target, this has to be checked by the subclass (this is why this
    //! function is private here)
#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D)
    void texImage1D( const SharedTextureData &_data, GLint _mipmapLevel );
    void texSubImage1D( const SharedTextureData &_data, GLint _mipmapLevel, uint32_t _offset = 0 );
#endif
    
    void texImage2D( const SharedTextureData &_data, GLint _mipmapLevel );
    void texSubImage2D( const SharedTextureData &_data, GLint _mipmapLevel, glm::uvec2 _offset = glm::uvec2(0) );

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_3D)
    void texImage3D( const SharedTextureData &_data, GLint _mipmapLevel );
    void texSubImage3D( const SharedTextureData &_data, GLint _mipmapLevel, glm::uvec3 _offset = glm::uvec3(0) );
#endif
    
    //! returns true if space for the texture was allocated
    bool textureStorageIsAllocated()
    {
        return (mWidth != 0);
    }

    //! Resizes the texture. Subclasses implementing this method may use only use the first entries of _size if they are of lower dimension
    virtual void resizeI(const glm::uvec3& _size) = 0;
};
ACGL_SMARTPOINTER_TYPEDEFS(TextureBase)

// missing classes:
// GL_TEXTURE_1D                        x
// GL_TEXTURE_2D                        x
// GL_TEXTURE_3D                        x
// GL_TEXTURE_1D_ARRAY                  x
// GL_TEXTURE_2D_ARRAY                  x
// GL_TEXTURE_RECTANGLE                 x
// GL_TEXTURE_2D_MULTISAMPLE
// GL_TEXTURE_2D_MULTISAMPLE_ARRAY
// GL_TEXTURE_BINDING_CUBE_MAP          x
// GL_TEXTURE_BINDING_CUBE_MAP_ARRAY

#if defined(ACGL_OPENGL_SUPPORTS_TEXTURE_RECTANGLE)
/**
 * In contrast to 'normal' textures these can't have mipmaps and are accessed in the shader by
 * pixel-coordinates instead of texture coordinates from 0 to 1.
 * Ths makes them a good choice for multiple renderpasses (no mipmaps needed and texture access
 * can be done via gl_FragCoord).
 * For all other cases use Texture2D.
 */
class TextureRectangle : public TextureBase
{
public:
    TextureRectangle( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_RECTANGLE, _internalFormat )
    {
        setMinFilter( GL_LINEAR ); // default would be MipMapped but that's not supported for Rect Textures!
    }

    TextureRectangle( const glm::uvec2 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_RECTANGLE, _internalFormat )
    {
        setMinFilter( GL_LINEAR ); // default would be MipMapped but that's not supported for Rect Textures!
        resize( _size );
    }

    //! sets the content to the given TextureData, might resize the texture
    void setImageData( const SharedTextureData &_data );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );

private:
    void generateMipmaps(void) { ACGL::Utils::error() << "Rectangle Textures don't support MipMaps!" << std::endl; }
};
ACGL_SMARTPOINTER_TYPEDEFS(TextureRectangle)
#endif

#if defined(ACGL_OPENGL_SUPPORTS_TEXTURE_1D)
/**
 * 1D textures.
 * can be used as general purpose data for a shader if texture filtering is wanted, otherwise look
 * at TextureBuffers!
 * 1D textures can have mipmaps, TextureBuffers can't.
 */
class Texture1D : public TextureBase
{
public:
    Texture1D( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_1D, _internalFormat ) {}
    Texture1D( const uint32_t _size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_1D, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content to the given TextureData, might resize the texture
    void setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );
};
ACGL_SMARTPOINTER_TYPEDEFS(Texture1D)
#endif

/**
 * "Normal" 2D texture.
 */
class Texture2D : public TextureBase
{
public:
    Texture2D(GLenum _internalFormat, GLuint _objectName) : TextureBase(GL_TEXTURE_2D, _internalFormat, _objectName) {}

    Texture2D( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_2D, _internalFormat ) {}

    Texture2D( const glm::uvec2 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_2D, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content to the given TextureData, might resize the texture
    void setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );
};
ACGL_SMARTPOINTER_TYPEDEFS(Texture2D)


#if defined(ACGL_OPENGL_SUPPORTS_TEXTURE_3D)
/**
 * 3D volume texture.
 */
class Texture3D : public TextureBase
{
public:
    Texture3D(GLenum _internalFormat, GLuint _objectName) : TextureBase(GL_TEXTURE_3D, _internalFormat, _objectName) {}

    Texture3D( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_3D, _internalFormat ) {}

    Texture3D( const glm::uvec3 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_3D, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content of one slice to the given TextureData
    void setImageData( const SharedTextureData &_data, uint32_t _slice, uint32_t _mipmapLayer );

    //! sets the content of all slices to the given TextureData
    void setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );
};
ACGL_SMARTPOINTER_TYPEDEFS(Texture3D)
#endif


#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D_ARRAY)
/**
 * Array of 1D textures, technically a 2D texture but without filtering between array layers.
 */
class Texture1DArray : public TextureBase
{
public:
    Texture1DArray( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_1D_ARRAY, _internalFormat ) {}
    Texture1DArray( const glm::uvec2 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_1D_ARRAY, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content to the given TextureData to one layer of the texture
    void setImageData( const SharedTextureData &_data, uint32_t _layer, uint32_t _mipmapLayer );

    //! sets the content to the given TextureData to fill all layers
    void setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );
};
ACGL_SMARTPOINTER_TYPEDEFS(Texture1DArray)
#endif
    

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_2D_ARRAY)
/**
 * A "stack" of 2D textures. Each must have the same size.
 * Can be used to access many texture in a shader without having to deal with the
 * texture unit limit.
 * Technically a 3D texture but there is no filtering in between the levels (Z-axis of
 * the 3D texture).
 */
class Texture2DArray : public TextureBase
{
public:
    Texture2DArray( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_2D_ARRAY, _internalFormat ) {}
    Texture2DArray( const glm::uvec3 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_2D_ARRAY, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content to the given TextureData, might resize the texture
    void setImageData( const SharedTextureData &_data, uint32_t _arrayLayer = 0, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );
};
ACGL_SMARTPOINTER_TYPEDEFS(Texture2DArray)
#endif

/**
 * For environmant mapping.
 */
class TextureCubeMap : public TextureBase
{
public:
    TextureCubeMap( GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_CUBE_MAP, _internalFormat ) {}
    TextureCubeMap( const glm::uvec2 &_size, GLenum _internalFormat = GL_RGBA ) : TextureBase( GL_TEXTURE_CUBE_MAP, _internalFormat )
    {
        resize( _size );
    }

    //! sets the content to the given TextureData, might resize the texture
    void setImageData( const SharedTextureData &_data, GLenum _cubeSide, uint32_t _mipmapLayer = 0 );

protected:
    //! content of the texture is undefined after this, this texture will be bound to the active binding point
    //! nothing should be bound to the pixel unpack buffer when calling this
    void resizeI( const glm::uvec3 &_newSize );

private:
    bool cubeSideIsValid( const GLenum _cubeSide ) const;
    void texImage2DCube( const SharedTextureData &_data, GLenum _cubeSide, GLint _mipmapLevel );
    void texSubImage2DCube( const SharedTextureData &_data, GLenum _cubeSide, GLint _mipmapLevel, glm::ivec2 _offset = glm::ivec2(0) );
};
ACGL_SMARTPOINTER_TYPEDEFS(TextureCubeMap)



} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_TEXTURE_HH
