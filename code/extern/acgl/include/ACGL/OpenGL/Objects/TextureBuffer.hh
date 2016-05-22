/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_TEXTUREBUFFER_HH
#define ACGL_OPENGL_OBJECTS_TEXTUREBUFFER_HH

/**
 * A Texture wraps the OpenGL texture buffer.
 *
 * TextureBuffers are 1D textures which store there data in a Buffer.
 * They are useful to access large chunks of data from within a shader.
 * Technically they are accessed as textures (uniform samplerBuffer) via texelFetch
 * with a 1D coordinate (int coordinate, so no filtering). This means they benefit from
 * texture caches.
 * Use these if the data doesn't fit into a UniformBufferObject.
 *
 * Data gets stored in the Buffer, no glTexImage calls are allowed!
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Objects/Buffer.hh>

#include <cassert>

#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_BUFFER )

namespace ACGL{
namespace OpenGL{

class TextureBuffer : public Buffer
{
public:
    // create a new BufferObject with _reservedMemory space (in bytes!)
    TextureBuffer( GLenum _dataType, size_t _reservedMemory = 1 ) : Buffer(GL_TEXTURE_BUFFER) {
        mTextureObjectName = 0;
        glGenTextures(1, &mTextureObjectName);
        mDataType = _dataType;
        Buffer::setData( _reservedMemory );
        attachBufferToTexture();
    }

    // use an existing BufferObject
    TextureBuffer( GLenum _dataType, SharedBufferObject _pBuffer ) : Buffer(_pBuffer, GL_TEXTURE_BUFFER) {
        mTextureObjectName = 0;
        glGenTextures(1, &mTextureObjectName);
        mDataType = _dataType;
        attachBufferToTexture();
    }

    ~TextureBuffer() {
        setBufferObject( SharedBufferObject() ); // detach the Buffer
        glDeleteTextures(1, &mTextureObjectName);
    }

    //! the GL buffer can get changed at any time
    void setBufferObject( SharedBufferObject _pBuffer ) {
        Buffer::setBufferObject( _pBuffer );
        if (!_pBuffer) {
            // detach all buffers:
            glTexBuffer( GL_TEXTURE_BUFFER, mDataType, 0 );
        } else {
            attachBufferToTexture();
        }
    }

    //! Bind the texture part to access it from a shader
    void bindTexture(GLuint _textureUnit = 0) const {
        glActiveTexture(GL_TEXTURE0 + _textureUnit);
        glBindTexture(GL_TEXTURE_BUFFER, mTextureObjectName);
    }

    //! Bind the buffer part to change the data
    void bindBuffer() const {
        Buffer::bind();
    }

    inline GLuint  getTextureObjectName() const { return mTextureObjectName; }
    inline GLuint  getBufferObjectName()  const { return Buffer::getObjectName(); }

private:
    //! private to prevent it from being called -> it's not clear whether the texture or the buffer should get bound, call
    //! bindBuffer() or bindTexture() directly!
    void bind() {}

    void attachBufferToTexture() {
        if (!mBuffer) {
            // the buffer was in fact detached
            bindTexture();
            glTexBuffer( GL_TEXTURE_BUFFER, mDataType, 0 );
        } else {
            assert( Buffer::getSize() > 0 && "glTexBuffer will fail if the buffer is empty" );
            bindTexture();
            glTexBuffer( GL_TEXTURE_BUFFER, mDataType, Buffer::getObjectName() );
        }
    }

    GLenum mDataType;
    GLuint mTextureObjectName;
};
ACGL_SMARTPOINTER_TYPEDEFS(TextureBuffer)

} // namespace
} // namespace

#endif // OpenGL 3.0+


#endif // ACGL_OPENGL_OBJECTS_TEXTUREBUFFER_HH
