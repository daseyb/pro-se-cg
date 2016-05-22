/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/TextureData.hh>
#include <ACGL/Utils/Memory.hh>

namespace ACGL {
namespace OpenGL {

GLsizei  TextureData::getPackAlignment() const
{
    size_t dataAlignment = Utils::pointerAlignment( mData.get());
    size_t rowAlignment  = Utils::pointerAlignment( mData.get() + getBytesPerScanline() );

	return (GLsizei) std::min(dataAlignment, rowAlignment); //minimum of the data and the begining of the second row
}


GLsizei  TextureData::getNumberOfChannels() const
{
    return ACGL::OpenGL::getNumberOfChannels( mFormat );
}


GLenum TextureData::getRecommendedInternalFormat() const
{
    return recommendedInternalFormat(mFormat, mColorSpace);
}


void TextureData::flipVertically()
{
    size_t scanlineInBytes = getBytesPerScanline();
    GLubyte *tmpScanLine = new GLubyte[ scanlineInBytes ];

    for (GLsizei line = 0; line < mHeight/2; ++line) {
        size_t topLine    = line;
        size_t bottomLine = mHeight - line - 1;
        void *topLinePtr    = mData.get() + topLine*scanlineInBytes;
        void *bottomLinePtr = mData.get() + bottomLine*scanlineInBytes;
        memcpy( tmpScanLine,   topLinePtr,    scanlineInBytes ); // top    -> tmp
        memcpy( topLinePtr,    bottomLinePtr, scanlineInBytes ); // bottom -> top
        memcpy( bottomLinePtr, tmpScanLine,   scanlineInBytes ); // tmp    -> bottom
    }

    delete[] tmpScanLine;
}


size_t TextureData::getBytesPerScanline() const
{
    // if uncompressed -> texel size is a multiple of 8
    // if compressed   -> mWidth is a multiple of 4
    // so this function will work for bitsizes of 2,4,8,16 etc. -> 1 bit per pixel might fail
    return (mWidth*getTexelSizeInBits())/8 + mPaddingBytesPerRow;
}

size_t TextureData::getSizeInBytes() const
{
    size_t s = getBytesPerScanline(); // correct even for compressed data
    if (mHeight > 0) s *= mHeight;
    if (mDepth  > 0) s *= mDepth;
    return s;
}

bool TextureData::dataIsCompressed() const
{
#if defined (ACGL_OPENGL_SUPPORTS_S3TC)
    if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || mFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) {
        // BC 1 aka DXT 1
        return true;
    } else if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) {
        // BC 2 aka DXT 3
        return true;
    } else if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        // BC 3 aka DXT 5
        return true;
    } else if (mFormat == GL_COMPRESSED_RED_RGTC1 || mFormat == GL_COMPRESSED_SIGNED_RED_RGTC1) {
        // BC 4
        return true;
    }
#endif
    // else if (mFormat == GL_COMPRESSED_RED_GREEN_RGTC2 || mFormat == GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2) {
    //    // BC 5
    //    return true;
    //} else if (mFormat == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT || mFormat == GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT) {
    //    // BC 6H
    //    return true;
    //} else if (mFormat == GL_COMPRESSED_RGBA_BPTC_UNORM || mFormat == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM) {
    //    // BC 7
    //    return true;
    //}
    
    return false;
}

size_t TextureData::getTexelSizeInBits() const
{
    #if defined (ACGL_OPENGL_SUPPORTS_S3TC)
    if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || mFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) {
        // BC 1 aka DXT 1
        return 4;
    } else if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) {
        // BC 2 aka DXT 3
        return 8;
    } else if (mFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
        // BC 3 aka DXT 5
        return 8;
    } else if (mFormat == GL_COMPRESSED_RED_RGTC1 || mFormat == GL_COMPRESSED_SIGNED_RED_RGTC1) {
        // BC 4
        return 8;
    }
#endif
    //} else if (mFormat == GL_COMPRESSED_RED_GREEN_RGTC2 || mFormat == GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2) {
    //    // BC 5
    //    return 8;
    //} else if (mFormat == GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT || mFormat == GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT) {
    //    // BC 6H
    //    return 8;
    //} else if (mFormat == GL_COMPRESSED_RGBA_BPTC_UNORM || mFormat == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM) {
    //    // BC 7
    //    return 8;
    // }
    return ( getNumberOfChannels()*getGLTypeSize(mType) )*8;
}

glm::vec4 TextureData::getTexel( glm::uvec2 _texCoord )
{
    // clamp negative to 0:
    _texCoord.x = std::max( 0u, _texCoord.x );
    _texCoord.y = std::max( 0u, _texCoord.y );

    // clamp values larger than the texture size to the maximum:
    _texCoord.x = std::min( _texCoord.x, (unsigned int) getWidth()  );
    _texCoord.y = std::min( _texCoord.y, (unsigned int) getHeight() );

    // the byte offset into pData of the desired texel:
    size_t texelOffset  = _texCoord.y * getBytesPerScanline();
    texelOffset        += _texCoord.x * getNumberOfChannels()*getGLTypeSize(mType);

    // default values:
    glm::vec4 result = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if ( mType == GL_BYTE ) {
        GLbyte *data = (GLbyte *) (mData.get()+texelOffset);
        result.r = data[0]/128.0f; // to -1..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/128.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/128.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/128.0f;

    } else if ( mType == GL_UNSIGNED_BYTE ) {
        GLubyte *data = (GLubyte *) (mData.get() +texelOffset);
        result.r = data[0]/255.0f; // to 0..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/255.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/255.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/255.0f;

    } else if ( mType == GL_SHORT ) {
        GLshort *data = (GLshort *) (mData.get() +texelOffset);
        result.r = data[0]/32768.0f; // to -1..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/32768.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/32768.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/32768.0f;

    } else if ( mType == GL_UNSIGNED_SHORT ) {
        GLushort *data = (GLushort *) (mData.get() +texelOffset);
        result.r = data[0]/65535.0f; // to 0..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/65535.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/65535.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/65535.0f;

    } else if ( mType == GL_INT ) {
        GLint *data = (GLint *) (mData.get() +texelOffset);
        result.r = data[0]/2147483648.0f; // to -1..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/2147483648.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/2147483648.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/2147483648.0f;

    } else if ( mType == GL_UNSIGNED_INT ) {
        GLuint *data = (GLuint *) (mData.get() +texelOffset);
        result.r = data[0]/4294967295.0f; // to 0..1
        if ( getNumberOfChannels() > 1 ) result.g = data[1]/4294967295.0f;
        if ( getNumberOfChannels() > 2 ) result.b = data[2]/4294967295.0f;
        if ( getNumberOfChannels() > 3 ) result.a = data[3]/4294967295.0f;

    } else if ( mType == GL_FLOAT ) {
        GLfloat *data = (GLfloat *) (mData.get() +texelOffset);
        result.r = data[0];
        if ( getNumberOfChannels() > 1 ) result.g = data[1];
        if ( getNumberOfChannels() > 2 ) result.b = data[2];
        if ( getNumberOfChannels() > 3 ) result.a = data[3];

    }
#if defined(GL_DOUBLE)
    else if ( mType == GL_DOUBLE ) {
        GLdouble *data = (GLdouble *) (mData.get() +texelOffset);
        result.r = (float) data[0];
        if ( getNumberOfChannels() > 1 ) result.g = (float) data[1];
        if ( getNumberOfChannels() > 2 ) result.b = (float) data[2];
        if ( getNumberOfChannels() > 3 ) result.a = (float) data[3];
    }
#endif
    else {
        ACGL::Utils::error() << "datatype " << mType << " not supported for getTexel! Return (0,0,0,1)." << std::endl;
    }

    return result;
}

void TextureData::setTexel( glm::uvec2 _texCoord, glm::vec4 _color )
{
    assert( mData && "can't set texels if there is no data store, define data using setData()" );
    // clamp negative to 0:
    _texCoord.x = std::max( 0u, _texCoord.x );
    _texCoord.y = std::max( 0u, _texCoord.y );

    // clamp values larger than the texture size to the maximum:
    _texCoord.x = std::min( _texCoord.x, (unsigned int) getWidth()  );
    _texCoord.y = std::min( _texCoord.y, (unsigned int) getHeight() );

    // the byte offset into pData of the desired texel:
    size_t texelOffset  = _texCoord.y * getBytesPerScanline();
    texelOffset        += _texCoord.x * getNumberOfChannels()*getGLTypeSize(mType);

    if ( mType == GL_BYTE ) {
        GLbyte *data = (GLbyte *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(128.0f) );
        color = glm::clamp( color, glm::ivec4(-128), glm::ivec4(127) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_UNSIGNED_BYTE ) {
        GLubyte *data = (GLubyte *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(255.0f) );
        color = glm::clamp( color, glm::ivec4(0), glm::ivec4(255) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_SHORT ) {
        GLshort *data = (GLshort *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(32768.0f) );
        color = glm::clamp( color, glm::ivec4(-32768), glm::ivec4(32767) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_UNSIGNED_SHORT ) {
        GLushort *data = (GLushort *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(65535.0f) );
        color = glm::clamp( color, glm::ivec4(0), glm::ivec4(65535) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_INT ) {
        GLint *data = (GLint *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(2147483648.0f) );
        color = glm::clamp( color, glm::ivec4(std::numeric_limits<int>::min()), glm::ivec4(2147483647) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_UNSIGNED_INT ) {
        GLuint *data = (GLuint *) (mData.get() +texelOffset);

        glm::ivec4 color = glm::ivec4( _color * glm::vec4(4294967295.0f) );
        color = glm::clamp( color, glm::ivec4(0), glm::ivec4(4294967295) );

        data[0] = color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = color.a;

    } else if ( mType == GL_FLOAT ) {
        GLfloat *data = (GLfloat *) (mData.get() +texelOffset);

        data[0] = _color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = _color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = _color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = _color.a;

    }
#if defined(GL_DOUBLE)
    else if ( mType == GL_DOUBLE ) {
        GLdouble *data = (GLdouble *) (mData.get() +texelOffset);

        data[0] = (float) _color.r;
        if ( getNumberOfChannels() > 1 ) data[1] = (float) _color.g;
        if ( getNumberOfChannels() > 2 ) data[2] = (float) _color.b;
        if ( getNumberOfChannels() > 3 ) data[3] = (float) _color.a;
    }
#endif
    else {
        ACGL::Utils::error() << "datatype " << mType << " not supported for setTexel!" << std::endl;
    }
}

float grayscaleMixdown(float _r, float _g, float _b)
{
    return 0.299f * _r + 0.587f * _g + 0.114f * _b;
}

glm::vec4 convertTexelNumChannels(glm::vec4 _texel, GLsizei _from, GLsizei _to)
{
    if (_from == _to) {
        return _texel;
    }
    else if (_from == 1) {
        switch (_to) {
            case 2: return glm::vec4( _texel.r, 1.0, 0.0, 0.0 );
            case 3: return glm::vec4( _texel.r, _texel.r, _texel.r, 0.0 );
            case 4: return glm::vec4( _texel.r, _texel.r, _texel.r, 1.0 );
        }
    }
    else if (_from == 2) {
        switch (_to) {
            case 1: return glm::vec4( _texel.r, 0.0, 0.0, 0.0 );
            case 3: return glm::vec4( _texel.r, _texel.r, _texel.r, 0.0 );
            case 4: return glm::vec4( _texel.r, _texel.r, _texel.r, _texel.g );
        }
    }
    else if (_from == 3) {
        switch (_to) {
            case 1: return glm::vec4( grayscaleMixdown(_texel.r, _texel.g, _texel.b), 0.0, 0.0, 0.0 );
            case 2: return glm::vec4( grayscaleMixdown(_texel.r, _texel.g, _texel.b), 1.0, 0.0, 0.0 );
            case 4: return glm::vec4( _texel.r, _texel.g, _texel.b, 1.0 );
        }
    }
    else if (_from == 4) {
        switch (_to) {
            case 1: return glm::vec4( grayscaleMixdown(_texel.r, _texel.g, _texel.b), 0.0, 0.0, 0.0 );
            case 2: return glm::vec4( grayscaleMixdown(_texel.r, _texel.g, _texel.b), 1.0, 0.0, 0.0 );
            case 3: return glm::vec4( _texel.r, _texel.g, _texel.b, 0.0 );
        }
    }
    return _texel;
}

void convertTextureData(const SharedTextureData& _from, const SharedTextureData& _to)
{
    assert(_from);
    assert(_to);
    if (!_from->getData()) {
        ACGL::Utils::error() << "Cannot convert TextureData: source TextureData contains no data" << std::endl;
        return;
    }

    // Setup target texture dimensions
    _to->setWidth(_from->getWidth());
    _to->setHeight(_from->getHeight());
    _to->setDepth(_from->getDepth());

    // Allocate new memory
    _to->deleteData();
    GLubyte* data = new GLubyte[_to->getSizeInBytes()];
    _to->setData(data);

    // Transfer pixels
    for (GLsizei y = 0; y < _to->getHeight(); ++y) {
        for (GLsizei x = 0; x < _to->getWidth(); ++x) {
            auto texel = convertTexelNumChannels(_from->getTexel(glm::uvec2(x, y)), _from->getNumberOfChannels(), _to->getNumberOfChannels());
            _to->setTexel(glm::uvec2(x, y), texel);
        }
    }
}

} // namespace OpenGL
} // namespace ACGL
