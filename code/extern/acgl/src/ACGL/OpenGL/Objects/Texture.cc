/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/Texture.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh> // for anisotrophic filtering
#include <ACGL/Math/Math.hh>

using namespace ACGL::OpenGL;

namespace
{

// Helper class to set the unpack alignment and reset it to the previous value
// at the end of the scope.
class ScopedUnpackAlignment
{
public:
    ScopedUnpackAlignment(size_t _unpackAlignment)
    {
        // Save previous, set new
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &mPreviousUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, std::min<GLuint>( (GLuint)_unpackAlignment, 8));
    }

    ~ScopedUnpackAlignment()
    {
        // Restore previous
        glPixelStorei(GL_UNPACK_ALIGNMENT, mPreviousUnpackAlignment);
    }

private:
    GLint mPreviousUnpackAlignment;
};

class ScopedPackAlignment
{
public:
    ScopedPackAlignment(size_t _packAlignment)
    {
        // Save previous, set new
        glGetIntegerv(GL_PACK_ALIGNMENT, &mPreviousUnpackAlignment);
        glPixelStorei(GL_PACK_ALIGNMENT, std::min<GLuint>( (GLuint)_packAlignment, 1));
    }

    ~ScopedPackAlignment()
    {
        // Restore previous
        glPixelStorei(GL_PACK_ALIGNMENT, mPreviousUnpackAlignment);
    }

private:
    GLint mPreviousUnpackAlignment;
};

}

void TextureBase::setMinFilter(GLint _value)
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, _value);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setMagFilter(GLint _value)
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, _value);
    glBindTexture(mTarget, prevTexture);
}

#if (( ACGL_OPENGLES_VERSION > 20) || (ACGL_OPENGL_VERSION > 20))
void TextureBase::setWrapS( GLenum _wrapS )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, _wrapS);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setWrapT( GLenum _wrapT )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, _wrapT);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setWrapR( GLenum _wrapR )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri(mTarget, GL_TEXTURE_WRAP_R, _wrapR);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setWrap( GLenum _wrapS, GLenum _wrapT, GLenum _wrapR )
{
    setWrapS(_wrapS);

    if(_wrapT != 0)
    {
        setWrapT(_wrapT);
    }

    if(_wrapR != 0)
    {
        setWrapR(_wrapR);
    }
}

void TextureBase::setBaseLevel(  GLint _level )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, _level);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setMaxLevel(  GLint _level )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, _level);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setMinLOD(  GLint _lod )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_MIN_LOD, _lod);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setMaxLOD(  GLint _lod )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_MAX_LOD, _lod);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setCompareMode( GLenum _mode )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, _mode);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setCompareFunc( GLenum _func )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, _func);
    glBindTexture(mTarget, prevTexture);
}

#endif
#if (ACGL_OPENGL_VERSION > 20)
void TextureBase::setLODBias( GLfloat _bias )
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameterf( mTarget, GL_TEXTURE_LOD_BIAS, _bias);
    glBindTexture(mTarget, prevTexture);
}

void TextureBase::setBorderColor(const glm::vec4& _color)
{
    GLuint prevTexture = bindAndGetOldTexture();
    glTexParameterfv( mTarget, GL_TEXTURE_BORDER_COLOR, (GLfloat*)&_color);
    glBindTexture(mTarget, prevTexture);
}
#endif

void TextureBase::setAnisotropicFilter( GLfloat _sampleCount )
{
    if ( ACGL_EXT_texture_filter_anisotrophic() ) {
        // anisotrophic filtering is supported:
        GLuint prevTexture = bindAndGetOldTexture();

        _sampleCount = std::max( _sampleCount, 1.0f );
        _sampleCount = std::min( _sampleCount, ACGL_MAX_TEXTURE_MAX_ANISOTROPY );
        glTexParameterf( mTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, _sampleCount );

        glBindTexture(mTarget, prevTexture);
    } else
    {
        if (_sampleCount != 1.0f ) {
            // anisotropic filtering will just increase the image quality, so this is just
            // a warning and no error, 1 sample is equal to no anisotrophic filtering, so ignore
            // that case entirely!
            ACGL::Utils::warning() << "Anisotropic filtering is not supported, ignored" << std::endl;
        }
    }
}

void TextureBase::generateMipmaps(void)
{
    GLuint prevTexture = bindAndGetOldTexture();

#if (!defined ACGL_OPENGL_PROFILE_CORE)
    // on some ATI systems texturing has to be enabled to generate MipMaps
    // this is not needed by the spec and deprecated on core profiles (generates
    // an error on MacOS X Lion)
    //glEnable(mTarget);
	//glGetError(); // ignore the error
#endif
#ifdef ACGL_OPENGL_VERSION_21
    // OpenGL 2 way to generate MipMaps
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
#else
    glGenerateMipmap(mTarget);
#endif

    glBindTexture(mTarget, prevTexture);
}

GLint   TextureBase::getParameterI( GLenum _name ) const
{
    GLuint prevTexture = bindAndGetOldTexture();
    GLint param;
    glGetTexParameteriv( mTarget, _name, &param );
    glBindTexture(mTarget, prevTexture);
    return param;
}

GLfloat TextureBase::getParameterF( GLenum _name ) const
{
    GLuint prevTexture = bindAndGetOldTexture();
    GLfloat param;
    glGetTexParameterfv( mTarget, _name, &param );
    glBindTexture(mTarget, prevTexture);
    return param;
}

glm::vec4 TextureBase::getParameter4F( GLenum _name ) const
{
    GLuint prevTexture = bindAndGetOldTexture();
    glm::vec4 param;
    glGetTexParameterfv( mTarget, _name, (GLfloat*)&param );
    glBindTexture(mTarget, prevTexture);
    return param;
}

GLuint TextureBase::bindAndGetOldTexture() const
{
    GLint prevTexture = 0;

#if defined(ACGL_OPENGL_SUPPORTS_TEXTURE_1D)
    if     (mTarget == GL_TEXTURE_1D)                   glGetIntegerv(GL_TEXTURE_BINDING_1D,                   &prevTexture);
    else
#endif
        if(mTarget == GL_TEXTURE_2D)                   glGetIntegerv(GL_TEXTURE_BINDING_2D,                   &prevTexture);
#if defined(ACGL_OPENGL_SUPPORTS_TEXTURE_3D)
    else if(mTarget == GL_TEXTURE_3D)                   glGetIntegerv(GL_TEXTURE_BINDING_3D,                   &prevTexture);
#endif
#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D_ARRAY)
    else if(mTarget == GL_TEXTURE_1D_ARRAY)             glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY,             &prevTexture);
#endif
#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_2D_ARRAY)
    else if(mTarget == GL_TEXTURE_2D_ARRAY)             glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY,             &prevTexture);
#endif
    else if(mTarget == GL_TEXTURE_CUBE_MAP)             glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,             &prevTexture);
#if (ACGL_OPENGL_VERSION >= 31)
    else if(mTarget == GL_TEXTURE_RECTANGLE)            glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE,            &prevTexture);
#if (ACGL_OPENGL_VERSION >= 32)
    else if(mTarget == GL_TEXTURE_2D_MULTISAMPLE)       glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE,       &prevTexture);
    else if(mTarget == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &prevTexture);
#if (ACGL_OPENGL_VERSION >= 40)
    else if(mTarget == GL_TEXTURE_CUBE_MAP_ARRAY)       glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,       &prevTexture);
#endif // 4.0
    else if(mTarget == GL_TEXTURE_BUFFER)               glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,               &prevTexture);
#endif // 3.2
#endif // 3.1
    else {
        ACGL::Utils::error() << "Unknown texture target, will create sideeffecs as old bound texture can not get restored!" << std::endl;
    }

    glBindTexture(mTarget, mObjectName);

    return (GLuint) prevTexture;
}

GLenum TextureBase::getCompatibleFormat( GLenum _internalFormat )
{
#if ((ACGL_OPENGLES_VERSION >= 20))
    if (   (_internalFormat == GL_ALPHA) || (_internalFormat == GL_RGB) || (_internalFormat == GL_RGBA)
        || (_internalFormat == GL_LUMINANCE) || (_internalFormat == GL_LUMINANCE_ALPHA)) {
        return _internalFormat;
    } else {
        ACGL::Utils::error() << "Unsupported format for ES" << std::endl;
    }
#else
    // Desktop OpenGL
    if (_internalFormat == GL_DEPTH24_STENCIL8) {
        return GL_DEPTH_STENCIL;
    }
    if (   _internalFormat == GL_DEPTH_COMPONENT
        || _internalFormat == GL_DEPTH_COMPONENT16
        || _internalFormat == GL_DEPTH_COMPONENT24
        || _internalFormat == GL_DEPTH_COMPONENT32F
        ) {
        return GL_DEPTH_COMPONENT;
    }

    if (   _internalFormat == GL_R8I
        || _internalFormat == GL_R8UI
        || _internalFormat == GL_R16I
        || _internalFormat == GL_R16UI
        || _internalFormat == GL_R32I
        || _internalFormat == GL_R32UI
        || _internalFormat == GL_RG8I
        || _internalFormat == GL_RG8UI
        || _internalFormat == GL_RG16I
        || _internalFormat == GL_RG16UI
        || _internalFormat == GL_RG32I
        || _internalFormat == GL_RG32UI
        || _internalFormat == GL_RGB8I
        || _internalFormat == GL_RGB8UI
        || _internalFormat == GL_RGB16I
        || _internalFormat == GL_RGB16UI
        || _internalFormat == GL_RGB32I
        || _internalFormat == GL_RGB32UI
        || _internalFormat == GL_RGBA8I
        || _internalFormat == GL_RGBA8UI
        || _internalFormat == GL_RGBA16I
        || _internalFormat == GL_RGBA16UI
        || _internalFormat == GL_RGBA32I
        || _internalFormat == GL_RGBA32UI ) {
        return GL_RGBA_INTEGER;
    }
#endif

    // there are probably some cases missing that don't work with RGBA
    return GL_RGBA;
}

GLenum TextureBase::getCompatibleType(   GLenum _internalFormat )
{
#if (ACGL_OPENGL_VERSION >= 20)
    if (_internalFormat == GL_DEPTH24_STENCIL8) {
        return GL_UNSIGNED_INT_24_8;
    }
    if (   _internalFormat == GL_R8I
        || _internalFormat == GL_R8UI
        || _internalFormat == GL_R16I
        || _internalFormat == GL_R16UI
        || _internalFormat == GL_R32I
        || _internalFormat == GL_R32UI
        || _internalFormat == GL_RG8I
        || _internalFormat == GL_RG8UI
        || _internalFormat == GL_RG16I
        || _internalFormat == GL_RG16UI
        || _internalFormat == GL_RG32I
        || _internalFormat == GL_RG32UI
        || _internalFormat == GL_RGB8I
        || _internalFormat == GL_RGB8UI
        || _internalFormat == GL_RGB16I
        || _internalFormat == GL_RGB16UI
        || _internalFormat == GL_RGB32I
        || _internalFormat == GL_RGB32UI
        || _internalFormat == GL_RGBA8I
        || _internalFormat == GL_RGBA8UI
        || _internalFormat == GL_RGBA16I
        || _internalFormat == GL_RGBA16UI
        || _internalFormat == GL_RGBA32I
        || _internalFormat == GL_RGBA32UI ) {
        return GL_INT;
    }
#endif
    // not sure if this works for all formats:
    return GL_UNSIGNED_BYTE;
}


#if (ACGL_OPENGL_VERSION >= 20)
// not supported so far on ES
TextureData *TextureBase::getTextureImageRAW( const Image &_image, GLenum _format, GLenum _type ) const
{
    ScopedPackAlignment alignment(1);

    GLenum target = mTarget;
    if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY) {
        target = _image.cubeMapFace;
    }

    // remember the previously bound texture and bind this one
    GLuint prevTexture = bindAndGetOldTexture();

    // determine the required buffer size to hold the requested LOD level
    GLint width, height, depth;
    glGetTexLevelParameteriv(mTarget, _image.mipmapLevel, GL_TEXTURE_WIDTH,  &width);
    glGetTexLevelParameteriv(mTarget, _image.mipmapLevel, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(mTarget, _image.mipmapLevel, GL_TEXTURE_DEPTH,  &depth);

    // fetch the image data
    int channels = 4;
    if      (_format == GL_DEPTH_COMPONENT) { channels = 1; } // TODO: test
    else if (_format == GL_DEPTH_STENCIL)   { channels = 2; } // TODO: test
    else if (_format == GL_RED)   { channels = 1; }
    else if (_format == GL_GREEN) { channels = 1; }
    else if (_format == GL_BLUE)  { channels = 1; }
    else if (_format == GL_RG)    { channels = 2; }
    else if (_format == GL_RGB)   { channels = 3; }
    else if (_format == GL_RGBA)  { channels = 4; }
    else if (_format == GL_BGR)   { channels = 3; }
    else if (_format == GL_BGRA)  { channels = 4; }
    else if (_format == GL_RED_INTEGER)   { channels = 1; }
    else if (_format == GL_GREEN_INTEGER) { channels = 1; }
    else if (_format == GL_BLUE_INTEGER)  { channels = 1; }
    else if (_format == GL_RG_INTEGER)    { channels = 2; }
    else if (_format == GL_RGB_INTEGER)   { channels = 3; }
    else if (_format == GL_RGBA_INTEGER)  { channels = 4; }
    else if (_format == GL_BGR_INTEGER)   { channels = 3; }
    else if (_format == GL_BGRA_INTEGER)  { channels = 4; }
    else {
        std::cerr << "unknown format: will guess the number of channels" << std::endl;
        channels = 4;
    }

    GLubyte *imageData = new GLubyte[ width * height * getGLTypeSize(_type) * channels ];
    glGetTexImage(target, _image.mipmapLevel, _format, _type, (GLvoid*)imageData);

    // revert to the previously bound texture
    glBindTexture(mTarget, prevTexture);

    // store the image data and meta information in a TextureData object
    TextureData *dataObject = new TextureData();
    dataObject->setWidth(  width  );
    dataObject->setHeight( height );
    dataObject->setDepth(  depth  );
    dataObject->setType(  _type   );
    dataObject->setFormat(_format );
    dataObject->setData( imageData); // dataObject will take care of freeing imageData

    return dataObject;
}
#endif // GL ES

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_RECTANGLE)
void TextureRectangle::resizeI(const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));
        setImageData( sTexData );
    }
}

void TextureRectangle::setImageData( const SharedTextureData &_data )
{
    mWidth  = _data->getWidth();
    mHeight = _data->getHeight();

    bind();
    glTexImage2D(
        mTarget,
        0, // there are no mipmaps for Rect Textures!
        mInternalFormat,
        mWidth, mHeight,
        0, // no border
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}
#endif

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D)
void Texture1D::setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        texImage1D( _data, _mipmapLayer );
    } else {
        texSubImage1D( _data, _mipmapLayer );
    }
}

void Texture1D::resizeI(const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage1D( sTexData, 0 );
    }
}
#endif

void Texture2D::setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        texImage2D( _data, _mipmapLayer );
    } else {
        texSubImage2D( _data, _mipmapLayer );
    }
}

void Texture2D::resizeI( const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage2D( sTexData, 0 );
    }
}


#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_3D)
void Texture3D::setImageData( const SharedTextureData &_data, uint32_t _slice, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        glm::uvec3 newSize = _data->getSize();
        newSize.z = _slice; // _data holds just one slice, so z = 1, resize the texture enough to hold at least _slice number of slices
        newSize = newSize * ((unsigned int)1<<_mipmapLayer);
        resize(newSize);
    }

    glm::uvec3 offset = glm::uvec3(0,0,_slice);
    texSubImage3D( _data, _mipmapLayer, offset );
}

void Texture3D::setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        glm::uvec3 newSize = _data->getSize();
        newSize = newSize * ((unsigned int)1<<_mipmapLayer);
        resize(newSize);
    }

    glm::uvec3 offset = glm::uvec3(0,0,0);
    texSubImage3D( _data, _mipmapLayer, offset );
}

void Texture3D::resizeI( const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight || _newSize.z != (unsigned int)mDepth) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setDepth (_newSize.z);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage3D( sTexData, 0 );
    }
}
#endif


#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D_ARRAY)
void Texture1DArray::setImageData( const SharedTextureData &_data, uint32_t _layer, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        glm::uvec2 newSize;
        newSize.x = _data->getWidth();
        newSize.y = _layer;
        newSize = newSize * ((unsigned int)1<<_mipmapLayer);
        resize(newSize);
    }

    glm::uvec2 offset = glm::uvec2(0, _layer );
    texSubImage2D( _data, _mipmapLayer, offset );
}

void Texture1DArray::setImageData( const SharedTextureData &_data, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        glm::uvec2 newSize;
        newSize.x = _data->getWidth();
        newSize.y = _data->getHeight();
        newSize = newSize * ((unsigned int)1<<_mipmapLayer);
        resize(newSize);
    }

    texSubImage2D( _data, _mipmapLayer );
}

void Texture1DArray::resizeI(const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage2D( sTexData, 0 );
    }
}
#endif


#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_2D_ARRAY)
void Texture2DArray::setImageData( const SharedTextureData &_data, uint32_t _arrayLayer, uint32_t _mipmapLayer )
{
    if (!textureStorageIsAllocated()) {
        texImage3D( _data, _mipmapLayer );
    } else {
        glm::uvec3 offset = glm::uvec3(0,0,_arrayLayer); // the array layer is the offset in Z in a 3D texture
        texSubImage3D( _data, _mipmapLayer, offset );
    }
}

void Texture2DArray::resizeI( const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight || _newSize.z != (unsigned int)mDepth) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setDepth (_newSize.z);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage3D( sTexData, 0 );
    }
}
#endif


void TextureCubeMap::setImageData( const SharedTextureData &_data, GLenum _cubeSide, uint32_t _mipmapLayer )
{
    if (!cubeSideIsValid(_cubeSide)) {
        std::cerr << "can't set cubemap side as provided enum does not name a side!" << std::endl;
        return;
    }
    if (!_data) {
        std::cerr << "can't set cubemap side as data is invalid" << std::endl;
        return;
    }
    //if (!textureStorageIsAllocated()) {
        texImage2DCube( _data, _cubeSide, _mipmapLayer );
    ///} else {
    //    texSubImage2DCube( _data, _cubeSide, _mipmapLayer );
    //}
}

void TextureCubeMap::resizeI(const glm::uvec3 &_newSize )
{
    if (_newSize.x != (unsigned int)mWidth || _newSize.y != (unsigned int)mHeight) {
        SharedTextureData sTexData( new TextureData() );
        sTexData->setData  (NULL);
        sTexData->setWidth (_newSize.x);
        sTexData->setHeight(_newSize.y);
        sTexData->setFormat(getCompatibleFormat( mInternalFormat ));
        sTexData->setType  (getCompatibleType(   mInternalFormat ));

        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0 );
        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0 );
        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0 );
        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0 );
        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0 );
        texImage2DCube( sTexData, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0 );
    }
}

bool TextureCubeMap::cubeSideIsValid( const GLenum _cubeSide ) const
{
    if (   _cubeSide == GL_TEXTURE_CUBE_MAP_POSITIVE_X || _cubeSide == GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        || _cubeSide == GL_TEXTURE_CUBE_MAP_POSITIVE_Y || _cubeSide == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        || _cubeSide == GL_TEXTURE_CUBE_MAP_POSITIVE_Z || _cubeSide == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z ) return true;
    return false;
}

void TextureCubeMap::texImage2DCube( const SharedTextureData &_data, GLenum _cubeSide, GLint _mipmapLevel )
{
    bind();
    mWidth  = _data->getWidth();
    mHeight = _data->getHeight();

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexImage2D(
        _cubeSide,
        _mipmapLevel,
        mInternalFormat,
        mWidth, mHeight,
        0, // no border
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}


void TextureCubeMap::texSubImage2DCube( const SharedTextureData &_data, GLenum _cubeSide, GLint _mipmapLevel, glm::ivec2 _offset )
{
    bind();

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexSubImage2D(
        _cubeSide,
        _mipmapLevel,
        _offset.x, _offset.y,
        mWidth, mHeight,
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_1D)
void TextureBase::texImage1D( const SharedTextureData &_data, GLint _mipmapLevel )
{
    bind();
    mWidth  = _data->getWidth();

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexImage1D(
        mTarget,
        _mipmapLevel,
        mInternalFormat,
        mWidth,
        0, // no border
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}

void TextureBase::texSubImage1D( const SharedTextureData &_data, GLint _mipmapLevel, uint32_t _offset )
{
    bind();
    GLsizei w = std::min( mWidth,  _data->getWidth()  );

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexSubImage1D(
        mTarget,
        _mipmapLevel,
        _offset,
        w,
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}
#endif

void TextureBase::texImage2D( const SharedTextureData &_data, GLint _mipmapLevel )
{
#if (ACGL_OPENGLES_VERSION >= 10)
    if (mInternalFormat != _data->getFormat()) {
        ACGL::Utils::error() << "On ES the internal and external formats must match, chaning internal format" << std::endl;
        mInternalFormat = _data->getFormat();
    }
    if ((_data->getType() != GL_UNSIGNED_BYTE) && (_data->getType() != GL_UNSIGNED_SHORT_5_6_5)
        && (_data->getType() != GL_UNSIGNED_SHORT_4_4_4_4) && (_data->getType() != GL_UNSIGNED_SHORT_5_5_5_1)) {
            ACGL::Utils::error() << "External type is unsupported on ES, this might fail!" << std::endl;
        }
#endif
    bind();
    mWidth  = _data->getWidth();
    mHeight = _data->getHeight();

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexImage2D(
        mTarget,
        _mipmapLevel,
        mInternalFormat,
        mWidth, mHeight,
        0, // no border
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}

void TextureBase::texSubImage2D( const SharedTextureData &_data, GLint _mipmapLevel, glm::uvec2 _offset )
{
#if (ACGL_OPENGLES_VERSION >= 10)
    if (mInternalFormat != _data->getFormat()) {
        ACGL::Utils::error() << "On ES the internal and external formats must match, chaning internal format" << std::endl;
        mInternalFormat = _data->getFormat();
    }
    if ((_data->getType() != GL_UNSIGNED_BYTE) && (_data->getType() != GL_UNSIGNED_SHORT_5_6_5)
        && (_data->getType() != GL_UNSIGNED_SHORT_4_4_4_4) && (_data->getType() != GL_UNSIGNED_SHORT_5_5_5_1)) {
        ACGL::Utils::error() << "External type is unsupported on ES, this might fail!" << std::endl;
    }
#endif
    
    bind();
    GLsizei w =           std::min( mWidth,  _data->getWidth()  );
    GLsizei h = std::max( std::min( mHeight, _data->getHeight() ), 1);

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexSubImage2D(
        mTarget,
        _mipmapLevel,
        _offset.x, _offset.y,
        w,h,
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}

#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_3D)
void TextureBase::texImage3D( const SharedTextureData &_data, GLint _mipmapLevel )
{
#if (ACGL_OPENGLES_VERSION >= 10)
    if (mInternalFormat != _data->getFormat()) {
        ACGL::Utils::error() << "On ES the internal and external formats must match, chaning internal format" << std::endl;
        mInternalFormat = _data->getFormat();
    }
    if ((_data->getType() != GL_UNSIGNED_BYTE) && (_data->getType() != GL_UNSIGNED_SHORT_5_6_5)
        && (_data->getType() != GL_UNSIGNED_SHORT_4_4_4_4) && (_data->getType() != GL_UNSIGNED_SHORT_5_5_5_1)) {
        ACGL::Utils::error() << "External type is unsupported on ES, this might fail!" << std::endl;
    }
#endif
    
    bind();
    mWidth  = _data->getWidth();
    mHeight = _data->getHeight();
    mDepth  = _data->getDepth();

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexImage3D(
        mTarget,
        _mipmapLevel,
        mInternalFormat,
        mWidth, mHeight, mDepth,
        0, // no border
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}

void TextureBase::texSubImage3D( const SharedTextureData &_data, GLint _mipmapLevel, glm::uvec3 _offset )
{
#if (ACGL_OPENGLES_VERSION >= 10)
    if (mInternalFormat != _data->getFormat()) {
        ACGL::Utils::error() << "On ES the internal and external formats must match, chaning internal format" << std::endl;
        mInternalFormat = _data->getFormat();
    }
    if ((_data->getType() != GL_UNSIGNED_BYTE) && (_data->getType() != GL_UNSIGNED_SHORT_5_6_5)
        && (_data->getType() != GL_UNSIGNED_SHORT_4_4_4_4) && (_data->getType() != GL_UNSIGNED_SHORT_5_5_5_1)) {
        ACGL::Utils::error() << "External type is unsupported on ES, this might fail!" << std::endl;
    }
#endif
    
    bind();
    GLsizei w =           std::min( mWidth,  _data->getWidth()  );
    GLsizei h = std::max( std::min( mHeight, _data->getHeight() ), 1);
    GLsizei d = std::max( std::min( mDepth,  _data->getDepth()  ), 1);

    ScopedUnpackAlignment alignment(_data->getPackAlignment());
    glTexSubImage3D(
        mTarget,
        _mipmapLevel,
        _offset.x, _offset.y, _offset.z,
        w,h,d,
        _data->getFormat(),
        _data->getType(),
        _data->getData() );
}
#endif

