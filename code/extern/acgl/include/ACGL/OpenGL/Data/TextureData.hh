/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_DATA_TEXTUREDATA_HH
#define ACGL_OPENGL_DATA_TEXTUREDATA_HH

/**
 * TextureData holds the data of a 1,2 or 3 dimensional image to be used as a texture.
 * This structure is used for texture loading.
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Data/ColorSpace.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Math/Math.hh>

namespace ACGL{
namespace OpenGL{

class TextureData
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    TextureData(void)
      : mData(nullptr, [](GLubyte* ptr) { delete[] ptr; }),
        mWidth(0),
        mHeight(0),
        mDepth(0),
        mFormat(GL_RGBA),
        mType(GL_UNSIGNED_BYTE),
        mPaddingBytesPerRow(0),
        mColorSpace(ColorSpace::AUTO_DETECT)
    {}

    // ========================================================================================================= \/
    // ================================================================================================= GETTERS \/
    // ========================================================================================================= \/
public:
    //! pointer to the raw pixel data
    GLubyte* getData() const { return mData.get(); }

    //! width in pixels
    GLsizei getWidth() const { return mWidth; }

    //! height in pixels
    GLsizei getHeight() const { return mHeight; }

    //! depth in pixels
    GLsizei getDepth() const { return mDepth; }

    //! channels etc. (e.g. GL_RGB)
    GLenum  getFormat() const { return mFormat; }

    //! data type (e.g. GL_BYTE)
    GLenum getType() const { return mType; }

    //! each line can have a few bytes of padding, number of bytes is returned
    GLsizei getPadding() const { return mPaddingBytesPerRow; }

    //! the color space in which the data is represented
    ColorSpace getColorSpace() const { return mColorSpace; }

    //! in pixel
    glm::uvec3 getSize() const { return glm::uvec3( mWidth, mHeight, mDepth ); }

    //! in bytes
    size_t getSizeInBytes() const;

    //! the byte alignment of each pixel row, (e.g. 1, 2, 4, 8 bytes)
    GLsizei  getPackAlignment() const;

    //! 1, 2, 3 or 4 based on format
    GLsizei getNumberOfChannels() const;

    //! A recommended value for the internalFormat enum of a Texture object. Based on the format, type and color space
    GLenum getRecommendedInternalFormat() const;

    //! flips the image vertically as some image formats have a different coordinate system as OpenGL has. (flip it upside down)
    void flipVertically();

    //! returns the texel converted to float (0..1 in case the data was int, -1..1 in case it was unsigned int).
    //! If the Image had less than 4 components it get's filled with (0,0,0,1)
    //! _texCoord is NOT normalized to 0..1! and gets clamped to the actual image size
    //! NOTE: this might be slow, for performance get the raw pointer and work on that!
    glm::vec4 getTexel( glm::uvec2 _texCoord );

    //! sets one texel, if the texture has less color components than 4, the superfluous components get ignored.
    //! in case the texture is int, the values from 0..1 will get scaled and clamped if needed.
    void setTexel( glm::uvec2 _texCoord, glm::vec4 _color );

    //! returns true if the data is stored as one of the compressed formats in OpenGL, the compression type is stored in mFormat, mType might be invalid
    bool dataIsCompressed() const;

    // ========================================================================================================= \/
    // ================================================================================================= SETTERS \/
    // ========================================================================================================= \/
public:
    typedef void(*data_deleter)(GLubyte*);
    typedef std::unique_ptr<GLubyte, data_deleter> DataPtr;
    
    void setData      (DataPtr&& _data)         { mData   = std::move(_data); }

    //! mData has to be created by new GLubyte[...] and will get deleted by this TextureData object!
    void setData(GLubyte* _data) {
      mData = DataPtr(_data, [](GLubyte* ptr) { delete[] ptr; });
    }

    void setData(GLubyte* _data, data_deleter _deleter) { 
      mData = DataPtr(_data, _deleter);
    }

    void setWidth     (GLsizei  _width)         { mWidth  = _width; }
    void setHeight    (GLsizei  _height)        { mHeight = _height; }
    void setDepth     (GLsizei  _depth)         { mDepth  = _depth; }
    void setFormat    (GLenum   _format)        { mFormat = _format; }
    void setType      (GLenum   _type)          { mType   = _type; }
    void setPadding   (GLsizei _padding)        { mPaddingBytesPerRow = _padding; }
    void setColorSpace(ColorSpace _colorSpace)  { mColorSpace = _colorSpace; }
    void setSize      (const glm::uvec3& _size) { mWidth = _size.x; mHeight = _size.y; mDepth = _size.z; }

    //! deletes the data attached to this object
    void deleteData() { mData.reset(nullptr); }

private:
    size_t getBytesPerScanline() const;

    //! returns the size per texel in bits(!) - bits to support compressed types!
    size_t getTexelSizeInBits() const;

    // ========================================================================================================= \/
    // ================================================================================================== FIELDS \/
    // ========================================================================================================= \/
    DataPtr    mData;               
    GLsizei    mWidth;
    GLsizei    mHeight;
    GLsizei    mDepth;
    GLenum     mFormat;             // channel types and count
    GLenum     mType;               // data type, invalid if the format is a compressed format
    GLsizei    mPaddingBytesPerRow; // number of padding bytes added per row: glReadPixel can read with padding and
                                    // some image writers support/need this as well (e.g. QT)
    ColorSpace mColorSpace;
};

ACGL_SMARTPOINTER_TYPEDEFS(TextureData)

//! Converts the texture data in _from to the target format and type given in
//! _to. Overwrites width, height, and depth in _to. Old texture data is removed
//! and new memory is allocated.
void convertTextureData(const SharedTextureData& _from, const SharedTextureData& _to);

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_DATA_TEXTUREDATA_HH
