/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>
#include <ACGL/OpenGL/HiLevelObjects/Viewport.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;

int_t FrameBufferObject::getColorAttachmentIndexByName(const std::string& _name) const
{
     for(AttachmentVec::size_type i = 0; i < mColorAttachments.size(); i++)
     {
         if(mColorAttachments[i].name == _name)
             return (int_t) i;
     }

     return -1;
 }

bool FrameBufferObject::isFrameBufferObjectComplete() const
{
    bind();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Utils::error() << "Failed to make complete FrameBufferObject object: ";
        if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            Utils::error() << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        } else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            Utils::error() << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        } else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
            Utils::error() << "GL_FRAMEBUFFER_UNSUPPORTED";
#ifndef ACGL_OPENGLES_VERSION_20
        } else if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
            Utils::error() << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
#endif
        } else {
            Utils::error() << status;
        }
        Utils::error() << std::endl;
        return false;
    }
    return true;
}

void FrameBufferObject::validate(void) const
{
    // if OpenGL says were ok, return
    if (isFrameBufferObjectComplete()) return;

    // the above call will create some output, but let's try to get more infos:
    if(mColorAttachments.size() > 0)
    {
        int width  = -1;
        int height = -1;

        if(mColorAttachments[0].texture)
        {
            width  = mColorAttachments[0].texture->getWidth();
            height = mColorAttachments[0].texture->getHeight();
        }
        else
        {
            width  = mColorAttachments[0].renderBuffer->getWidth();
            height = mColorAttachments[0].renderBuffer->getHeight();
        }

        for(AttachmentVec::size_type k = 0; k < mColorAttachments.size(); k++)
        {
            bool fail = false;

            if(mColorAttachments[k].texture)
                fail = (mColorAttachments[k].texture->getWidth() != width || mColorAttachments[k].texture->getHeight() != height);
            else //otherwise its a RenderBuffer
                fail = (mColorAttachments[k].renderBuffer->getWidth() != width || mColorAttachments[k].renderBuffer->getHeight() != height);

            if(fail)
                Utils::error() << "FrameBufferObject validation failed: Color attachment "<< k << " has different size." << std::endl;
        }
    }
    else
        Utils::error() << "FrameBufferObject validation failed: No color attachments."<< std::endl;
}

bool FrameBufferObject::attachColorAttachment( const Attachment &_attachment )
{
    int realLocation = -1;
    GLint maxColorBuffers;
#ifdef ACGL_OPENGLES_VERSION_20
    maxColorBuffers = 1;
#else
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxColorBuffers);
#endif
    
    for (unsigned int i = 0; i < mColorAttachments.size(); ++i) {
        if (mColorAttachments[i].name == _attachment.name) {
            // replace this attachment
            GLuint newLocation = _attachment.location;
            if (newLocation > (GLuint) maxColorBuffers) {
                // we have to find a place, but luckily we can use the old location of the same-named attachment:
                newLocation = mColorAttachments[i].location;
            }
            mColorAttachments[i] = _attachment;
            mColorAttachments[i].location = newLocation;
            realLocation = i;
        }
    }
    if (realLocation == -1) {
        // it's a new attachment
        GLuint newLocation = _attachment.location;
        if (newLocation > (GLuint) maxColorBuffers) {
            // we have to find a place:
            newLocation = (GLuint) mColorAttachments.size();
        }

        realLocation = (int) mColorAttachments.size();
        mColorAttachments.push_back(_attachment);
        mColorAttachments[mColorAttachments.size()-1].location = newLocation;
    }

    // attach it to the OpenGL object:
    bind();
    if (_attachment.renderBuffer) {
        // it's a renderBuffer
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + realLocation, GL_RENDERBUFFER, _attachment.renderBuffer->getObjectName() );
    } else {
        // it's a texture
        GLenum textureTarget = _attachment.texture->getTarget();
        int dimensionality = 0;

#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_1D )
        if (textureTarget == GL_TEXTURE_1D) {
            dimensionality = 1;
        }
#endif
        if (textureTarget == GL_TEXTURE_2D) {
            dimensionality = 2;
        }
#if (ACGL_OPENGL_VERSION > 30)
        if (textureTarget == GL_TEXTURE_2D_MULTISAMPLE) {
            dimensionality = 2;
        }
#endif
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_3D )
        if (textureTarget == GL_TEXTURE_3D) {
            dimensionality = 3;
        }
#endif
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_1D_ARRAY )
        if (textureTarget == GL_TEXTURE_1D_ARRAY) {
            dimensionality = 2;
        }
#endif
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_2D_ARRAY )
        if (textureTarget == GL_TEXTURE_2D_ARRAY) {
            dimensionality = 3;
        }
#endif
#if (ACGL_OPENGL_VERSION >= 30)
        if (textureTarget == GL_TEXTURE_2D_MULTISAMPLE_ARRAY) {
            dimensionality = 3;
        }
#endif
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_RECTANGLE )
        if (textureTarget == GL_TEXTURE_RECTANGLE) {
            dimensionality = 2;
        }
#endif
        if (textureTarget == GL_TEXTURE_CUBE_MAP) {
            dimensionality = 2;
            textureTarget = _attachment.image.cubeMapFace;
#if (ACGL_OPENGL_VERSION >= 40)
        } else if (textureTarget == GL_TEXTURE_CUBE_MAP_ARRAY) {
            dimensionality = 3;
            textureTarget = _attachment.image.cubeMapFace;
#endif // OpenGL 4.0+
        }
        if (dimensionality == 0) {
            Utils::error() << "attachColorAttachment failed, texture target not supported" << std::endl;
        }

        bool attachmentOK = false;
#if defined( ACGL_OPENGL_SUPPORTS_TEXTURE_1D )
        if (dimensionality == 1) {
            glFramebufferTexture1D( GL_FRAMEBUFFER,
                                    GL_COLOR_ATTACHMENT0 + realLocation,
                                    textureTarget,
                                    _attachment.texture->getObjectName(),
                                    _attachment.image.mipmapLevel );
            attachmentOK = true;
        }
#endif
        if (dimensionality == 2) {
            glFramebufferTexture2D( GL_FRAMEBUFFER,
                                    GL_COLOR_ATTACHMENT0 + realLocation,
                                    textureTarget,
                                    _attachment.texture->getObjectName(),
                                    _attachment.image.mipmapLevel );
            attachmentOK = true;
        }
#if defined (ACGL_OPENGL_SUPPORTS_TEXTURE_3D )
        if (dimensionality == 3) {
            glFramebufferTextureLayer( GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0 + realLocation,
                                       _attachment.texture->getObjectName(),
                                       _attachment.image.mipmapLevel,
                                       _attachment.image.layer
                                       );
            attachmentOK = true;
        }
#endif
        if (!attachmentOK) {
            Utils::error() << "attachColorAttachment failed, texture target not supported" << std::endl;
        }

        //Utils::debug() << "glFramebufferTexture2D( " << _attachment.name << " to " << realLocation <<" )" << std::endl;
    }

    remapAttachments();
    return true;
}

void FrameBufferObject::remapAttachments()
{
    GLint maxColorBuffers;
#ifdef ACGL_OPENGLES_VERSION_20
    maxColorBuffers = 1;
#else
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxColorBuffers);
#endif
    
    GLenum* bufferMappings = new GLenum[maxColorBuffers];

    int attachments = std::min( maxColorBuffers, (int) mColorAttachments.size() );

    for (int i = 0; i < maxColorBuffers; ++i) {
        bufferMappings[i] = GL_NONE;
    }
    for (int i = 0; i < attachments; ++i) {
        if (bufferMappings[ mColorAttachments[i].location ] != GL_NONE) {
            Utils::warning() << "FBO: Attachment mapping collision: Location " << mColorAttachments[i].location;
            Utils::warning() << " maps to both attachments " << mColorAttachments[i].name;
            Utils::warning() << " and " << mColorAttachments[bufferMappings[mColorAttachments[i].location] - GL_COLOR_ATTACHMENT0].name << std::endl;
        }
        bufferMappings[ mColorAttachments[i].location ] = GL_COLOR_ATTACHMENT0 + i;
    }

    // debug:
/*
    if (bufferMappings[0] == GL_NONE) {
    Utils::error() << "remapAttachments: " << std::endl;
    for (unsigned int i = 0; i < maxColorBuffers; ++i) {
        if (bufferMappings[i] == GL_NONE) {
            Utils::error() << "bufferMappings["<<i<<"] GL_NONE" << std::endl;
        } else {
            Utils::error() << "bufferMappings["<<i<<"] "<< bufferMappings[i]-GL_COLOR_ATTACHMENT0 << std::endl;
        }
    }
    }*/

    // end debug

    bind(); // glDrawBuffers will get part of the FBO state!

#if ( defined ( __APPLE__) && !defined(ACGL_PLATFORM_IOS) )
    // Somehow, Apple's current (OSX 10.7) OpenGL implementation skips over all
    // GL_NONE entries in the bufferMappings array if the only color attachment
    // specified is GL_COLOR_ATTACHMENT0.
    // This however causes that no change from the initial FBO state is
    // detected when the only color attachment should be assigned to a FragData
    // location other than 0.
    // Hotfix: Temporarily set all draw buffers to GL_NONE before setting the
    // actual configuration:
    glDrawBuffers(0, (GLenum*)NULL);
#endif // __APPLE__

#ifndef ACGL_OPENGLES_VERSION_20
    glDrawBuffers( maxColorBuffers, bufferMappings );
#endif

    delete[] bufferMappings;
}


void FrameBufferObject::setAttachmentLocations(ConstSharedLocationMappings _locationMappings)
{
    bool needsUpdate = false;

    for(AttachmentVec::size_type i = 0; i < mColorAttachments.size(); i++)
    {
        int_t location = _locationMappings->getLocation(mColorAttachments[i].name);

        if (location != -1) // is a mapping by that name specified?
        {
            mColorAttachments[i].location = location;
            needsUpdate = true;
        }
    }

    if(needsUpdate) remapAttachments();
}


SharedLocationMappings FrameBufferObject::getAttachmentLocations() const
{
    SharedLocationMappings locationMap = SharedLocationMappings( new LocationMappings() );

    for (AttachmentVec::size_type i = 0; i < mColorAttachments.size(); i++)
    {
        locationMap->setLocation( mColorAttachments[i].name, mColorAttachments[i].location );
        //ACGL::Utils::debug() << "locationMap->setLocation( "<<mColorAttachments[i].name<<", "<<mColorAttachments[i].location<<" );"<<std::endl;
    }

    return locationMap;
}

SharedTextureData FrameBufferObject::getImageData(GLsizei _width, GLsizei _height, GLint _x, GLint _y, GLenum _readBuffer)
{
    GLenum format = GL_RGB;

    //
    // glReadPixels aligns each pixel row for 4 bytes which can create gaps in each row
    // in case of 1-3 channel data (and one byte per channel)
    //
    size_t packingAlignment = 4; // can be 1,2,4,8
    size_t bytesOfDataPerRow = _width*getNumberOfChannels(format);
    size_t paddingPerRow = ( packingAlignment - (bytesOfDataPerRow % packingAlignment) ) % packingAlignment;
    GLubyte* frameBufferData = new GLubyte[_height * (bytesOfDataPerRow + paddingPerRow) ];

    ACGL::OpenGL::bindDefaultFramebuffer();
    
#ifndef ACGL_OPENGLES_VERSION_20
    if(_readBuffer != GL_INVALID_ENUM)
        glReadBuffer(_readBuffer);
#endif

    glPixelStorei( GL_PACK_ALIGNMENT, (GLint)packingAlignment ); // 4 is the default
    glReadPixels(_x, _y, _width, _height, format, GL_UNSIGNED_BYTE, frameBufferData);

    SharedTextureData texData = SharedTextureData(new TextureData());
    texData->setWidth(_width);
    texData->setHeight(_height);
    texData->setFormat(format);
    texData->setType(GL_UNSIGNED_BYTE);
    texData->setData(frameBufferData); // frameBufferData memory will be managed by texData
    texData->setPadding( (GLsizei) paddingPerRow);

    return texData;
}

SharedTextureData FrameBufferObject::getImageData()
{
    Viewport currentViewport;
    currentViewport.setFromGLContext();

    return FrameBufferObject::getImageData( currentViewport.getWidth(), currentViewport.getHeight(), currentViewport.getOffsetX(), currentViewport.getOffsetY() );
}


//! clear only the depth buffer:
void FrameBufferObject::clearDepthBuffer()
{
    if (!mDepthAttachment.texture && !mDepthAttachment.renderBuffer) return;

    bind(); // NOTE: if the old binding gets restored (removing this side effect), update clearBuffers() as it relies on it!
    glClear( GL_DEPTH_BUFFER_BIT );
}

#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 20))
//! clear one specific color buffer:
void FrameBufferObject::clearBuffer( const std::string &_name )
{
    int index = getColorAttachmentIndexByName( _name );
    if (index == -1) {
        ACGL::Utils::error() << "can't clear color buffer " << _name << " - attachment does not exist" << std::endl;
    }

    bind();
    if (mColorAttachments[index].clearColor.mType == ClearColor::Float) {
        glClearBufferfv(  GL_COLOR, mColorAttachments[index].location, mColorAttachments[index].clearColor.mColor.floatColor);
    } else if (mColorAttachments[index].clearColor.mType == ClearColor::Integer) {
        glClearBufferiv(  GL_COLOR, mColorAttachments[index].location, mColorAttachments[index].clearColor.mColor.intColor);
    } else {
        glClearBufferuiv( GL_COLOR, mColorAttachments[index].location, mColorAttachments[index].clearColor.mColor.uintColor);
    }
}

//! clear all buffers, color and depth:
void FrameBufferObject::clearBuffers()
{
    clearDepthBuffer(); // will also bind this FBO
    for (unsigned int i = 0; i < mColorAttachments.size(); ++i) {
        if (mColorAttachments[i].clearColor.mType == ClearColor::Float) {
            glClearBufferfv(  GL_COLOR, mColorAttachments[i].location, mColorAttachments[i].clearColor.mColor.floatColor);
        } else if (mColorAttachments[i].clearColor.mType == ClearColor::Integer) {
            glClearBufferiv(  GL_COLOR, mColorAttachments[i].location, mColorAttachments[i].clearColor.mColor.intColor);
        } else {
            glClearBufferuiv( GL_COLOR, mColorAttachments[i].location, mColorAttachments[i].clearColor.mColor.uintColor);
        }
    }
}
#endif

//! sets the clear color for one buffer:
void FrameBufferObject::setClearColor( const std::string &_name, const glm::vec4 &_color )
{
    int index = getColorAttachmentIndexByName( _name );
    if (index == -1) {
        ACGL::Utils::error() << "can't set clear color of " << _name << " - attachment does not exist" << std::endl;
    }

    mColorAttachments[index].clearColor.mType = ClearColor::Float;
    mColorAttachments[index].clearColor.mColor.floatColor[0] = _color.r;
    mColorAttachments[index].clearColor.mColor.floatColor[1] = _color.g;
    mColorAttachments[index].clearColor.mColor.floatColor[2] = _color.b;
    mColorAttachments[index].clearColor.mColor.floatColor[3] = _color.a;
}
//! sets the clear color for one buffer:
void FrameBufferObject::setClearColor( const std::string &_name, const glm::ivec4 &_color )
{
    int index = getColorAttachmentIndexByName( _name );
    if (index == -1) {
        ACGL::Utils::error() << "can't set clear color of " << _name << " - attachment does not exist" << std::endl;
    }

    mColorAttachments[index].clearColor.mType = ClearColor::Integer;
    mColorAttachments[index].clearColor.mColor.intColor[0] = _color.r;
    mColorAttachments[index].clearColor.mColor.intColor[1] = _color.g;
    mColorAttachments[index].clearColor.mColor.intColor[2] = _color.b;
    mColorAttachments[index].clearColor.mColor.intColor[3] = _color.a;
}
//! sets the clear color for one buffer:
void FrameBufferObject::setClearColor( const std::string &_name, const glm::uvec4 &_color )
{
    int index = getColorAttachmentIndexByName( _name );
    if (index == -1) {
        ACGL::Utils::error() << "can't set clear color of " << _name << " - attachment does not exist" << std::endl;
    }

    mColorAttachments[index].clearColor.mType = ClearColor::UnsignedInteger;
    mColorAttachments[index].clearColor.mColor.uintColor[0] = _color.r;
    mColorAttachments[index].clearColor.mColor.uintColor[1] = _color.g;
    mColorAttachments[index].clearColor.mColor.uintColor[2] = _color.b;
    mColorAttachments[index].clearColor.mColor.uintColor[3] = _color.a;
}

//! sets the clear color for all color buffers:
void FrameBufferObject::setClearColor( const glm::vec4 &_color )
{
    for (unsigned int i = 0; i < mColorAttachments.size(); ++i) {
        mColorAttachments[i].clearColor.mType = ClearColor::Float;
        mColorAttachments[i].clearColor.mColor.floatColor[0] = _color.r;
        mColorAttachments[i].clearColor.mColor.floatColor[1] = _color.g;
        mColorAttachments[i].clearColor.mColor.floatColor[2] = _color.b;
        mColorAttachments[i].clearColor.mColor.floatColor[3] = _color.a;
    }
}
//! sets the clear color for all color buffers:
void FrameBufferObject::setClearColor( const glm::ivec4 &_color )
{
    for (unsigned int i = 0; i < mColorAttachments.size(); ++i) {
        mColorAttachments[i].clearColor.mType = ClearColor::Integer;
        mColorAttachments[i].clearColor.mColor.intColor[0] = _color.r;
        mColorAttachments[i].clearColor.mColor.intColor[1] = _color.g;
        mColorAttachments[i].clearColor.mColor.intColor[2] = _color.b;
        mColorAttachments[i].clearColor.mColor.intColor[3] = _color.a;
    }
}
//! sets the clear color for all color buffers:
void FrameBufferObject::setClearColor( const glm::uvec4 &_color )
{
    for (unsigned int i = 0; i < mColorAttachments.size(); ++i) {
        mColorAttachments[i].clearColor.mType = ClearColor::UnsignedInteger;
        mColorAttachments[i].clearColor.mColor.uintColor[0] = _color.r;
        mColorAttachments[i].clearColor.mColor.uintColor[1] = _color.g;
        mColorAttachments[i].clearColor.mColor.uintColor[2] = _color.b;
        mColorAttachments[i].clearColor.mColor.uintColor[3] = _color.a;
    }
}

