/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_FRAMEBUFFEROBJECT_HH
#define ACGL_OPENGL_OBJECTS_FRAMEBUFFEROBJECT_HH

/**
 * This FrameBufferObject class encapsulates an OpenGL frame buffer object (FBO).
 * A FrameBufferObject is a target for rendering and thus consists of different "layers":
 *
 * one or no depthbuffer
 * one or no stencilbuffer
 * one (OpenGL ES) to many (hardware dependent limit) colorbuffers
 *
 * These buffers get attached to the FrameBufferObject.
 *
 * There exists one system-provided frame buffer object for rendering to the screen
 * and optionaly multiple user defined frame buffer objects for offscreen rendering.
 *
 * This class does not encapsulate the system-provided FBO.
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Objects/RenderBuffer.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>
#include <ACGL/OpenGL/Data/LocationMappings.hh>

#include <vector>
#include <map>

namespace ACGL{
namespace OpenGL{

class FrameBufferObject
{
    ACGL_NOT_COPYABLE(FrameBufferObject)

    // ==================================================================================================== \/
    // ============================================================================================ STRUCTS \/
    // ==================================================================================================== \/
public:
    //! defines a clear color per color buffer
    struct ClearColor {
        ClearColor() {
            mType = Float;
            mColor.floatColor[0] = mColor.floatColor[1] = mColor.floatColor[2] = mColor.floatColor[3] = 0.0f;
        }

        enum Type { Integer, UnsignedInteger, Float };
        union Data {
            GLint   intColor[4];
            GLuint  uintColor[4];
            GLfloat floatColor[4];
        } mColor;
        Type mType; // defines which of the data types of the union to use
    };

    //! An attachment can be a texture or a render buffer
    struct Attachment
    {
        std::string             name;         // user defined name that matches the fragment shader out
        ConstSharedTextureBase  texture;      // attached color texture, or:
        ConstSharedRenderBuffer renderBuffer; // attached renderbuffer - only this or the texture should be set!
        GLuint                  location;     // the frag data location that maps to this attachment
        Image                   image;        // in case of a texture the image will define which part of the texture to use
        ClearColor              clearColor;   // clear color of the attachement
    };

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< Attachment > AttachmentVec;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    FrameBufferObject(void)
    :   mObjectName(0),
        mColorAttachments(),
        mDepthAttachment()
    {
        mObjectName = 0;
        glGenFramebuffers(1, &mObjectName);
        mDepthAttachment.texture      = ConstSharedTextureBase();
        mDepthAttachment.renderBuffer = ConstSharedRenderBuffer();
        mDepthAttachment.name     = ""; // not useful here
        mDepthAttachment.location =  0; // not useful here
    }

    virtual ~FrameBufferObject(void)
    {
        // buffer 0 will get ignored by OpenGL
        glDeleteFramebuffers(1, &mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#ifdef ACGL_OPENGL_DEBUGGER_SUPPORT
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_FRAMEBUFFER>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_FRAMEBUFFER>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline       GLuint         getObjectName       (void) const { return mObjectName; }
    inline const AttachmentVec& getColorAttachments (void) const { return mColorAttachments; }
    inline const Attachment&    getDepthAttachment  (void) const { return mDepthAttachment;  }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:
     int_t getColorAttachmentIndexByName(const std::string& _name) const;

     void validate (void) const;

    /**
     * Per default a FrameBufferObject gets used for read/write operations, but we can
     * bind two different FrameBufferObjects for these operations!
     */
    inline void bind(GLenum _type = GL_FRAMEBUFFER) const
    {
        glBindFramebuffer(_type, mObjectName);
    }

    //! let OpenGL validate the completeness
    bool isFrameBufferObjectComplete() const;

    /*
     * Attach another RenderBuffer as a render target. If the name already exists the old target will get replaced.
     */
    inline bool attachColorRenderBuffer(const std::string &_name, const ConstSharedRenderBuffer& _renderBuffer)
    {
        Attachment a = {_name, SharedTextureBase(), _renderBuffer, 0xFFFFFFFF, Image(), ClearColor()};
        return attachColorAttachment( a );
    }

    inline bool attachColorTexture(const std::string &_name, const ConstSharedTextureBase& _texture, const Image _image = Image() )
    {
        Attachment a = {_name, _texture, SharedRenderBuffer(), 0xFFFFFFFF, _image, ClearColor()};
        return attachColorAttachment( a );
    }

    inline bool attachColorRenderBuffer(const std::string &_name, const ConstSharedRenderBuffer& _renderBuffer, GLuint _location )
    {
        Attachment a = {_name, SharedTextureBase(), _renderBuffer, _location, Image(), ClearColor()};
        return attachColorAttachment( a );
    }

    inline bool attachColorTexture(const std::string &_name, const ConstSharedTextureBase& _texture, GLuint _location, const Image _image = Image() )
    {
        Attachment a = {_name, _texture, SharedRenderBuffer(), _location, _image, ClearColor()};
        return attachColorAttachment( a );
    }

    // if the location within the attachment is larger than the possible number of attachments,
    // attachColorAttachment will try to find a non-colliding attachment point.
    // this can be used to say "i don't care about the actual attachment number used"
    bool attachColorAttachment( const Attachment &_attachment );

    void remapAttachments();

    inline bool setDepthRenderBuffer(const ConstSharedRenderBuffer& _renderBuffer)
    {
        bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer->getObjectName() );
        #if (ACGL_OPENGLES_VERSION == 20)
            if( _renderBuffer->getInternalFormat() == GL_DEPTH24_STENCIL8_OES ||
                _renderBuffer->getInternalFormat() == GL_DEPTH_STENCIL_OES)
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer->getObjectName() );
        #else
            if( _renderBuffer->getInternalFormat() == GL_DEPTH24_STENCIL8 ||
                _renderBuffer->getInternalFormat() == GL_DEPTH_STENCIL)
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer->getObjectName() );
        #endif
        mDepthAttachment.renderBuffer = _renderBuffer;
        return true;
    }

    //! todo: support mipmap levels
    inline bool setDepthTexture(const ConstSharedTextureBase& _texture)
    {
        bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _texture->getTarget(), _texture->getObjectName(), 0);
        #if (ACGL_OPENGLES_VERSION == 20)
            if( _texture->getInternalFormat() == GL_DEPTH24_STENCIL8_OES ||
                _texture->getInternalFormat() == GL_DEPTH_STENCIL_OES)
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, _texture->getTarget(), _texture->getObjectName(), 0);
        #else
            if( _texture->getInternalFormat() == GL_DEPTH24_STENCIL8 ||
                _texture->getInternalFormat() == GL_DEPTH_STENCIL)
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, _texture->getTarget(), _texture->getObjectName(), 0);
        #endif
        mDepthAttachment.texture = _texture;
        return true;
    }

    //! x,y coordinate is the FBO size in pixels, z coordinate the number of color attachments. x,y are both 0 if the FBO has no useful size
    //! todo: support mipmap levels
    glm::uvec3 getSize() const
    {
        glm::uvec3 size;

        if (mDepthAttachment.texture) {
            // use the size of the depth texture
            size = mDepthAttachment.texture->getSize();
        } else if (mDepthAttachment.renderBuffer) {
            // use the size of the depth render buffer
            size = glm::uvec3( mDepthAttachment.renderBuffer->getSize(), 0 );
        } else if ( mColorAttachments.size() > 0 ) {
            if (mColorAttachments[0].texture) {
                // use the size of the first texture:
                size = mColorAttachments[0].texture->getSize();
            } else if (mColorAttachments[0].renderBuffer) {
                // use the size of the first renderBuffer:
                size = glm::uvec3( mColorAttachments[0].renderBuffer->getSize(), 0 );
            }
        } else {
            size = glm::uvec3(0);
        }

        size.z = (glm::uint) mColorAttachments.size();
        return size;
    }

    // =================================================================================================== \/
    // =========================================================================================== METHODS \/
    // =================================================================================================== \/
public:
    //! sets the attachment locations of this FBO where they match the names specified in _locationMappings
    void setAttachmentLocations(ConstSharedLocationMappings _locationMappings);

    //! get a list of attachment locations and names that can be used to set up a ShaderProgram
    SharedLocationMappings getAttachmentLocations() const;

    //! returns the current contents of the default FrameBuffer
    //! the format of the returned TextureData will be GL_RGB, the type will be GL_UNSIGNED_INT
    //! _readBuffer = GL_INVALID_ENUM will read out the default read buffer
    static SharedTextureData getImageData(GLsizei _width, GLsizei _height, GLint _x = 0, GLint _y = 0, GLenum _readBuffer = GL_INVALID_ENUM);

    //! get the part of the framebuffer thats part of the current viewport
    static SharedTextureData getImageData();

    //
    // clear buffer functions:
    //

    //! clear only the depth buffer:
    void clearDepthBuffer();

#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 20))
    //! clear one specific color buffer:
    void clearBuffer( const std::string &_name );

    //! clear all buffers, color and depth:
    void clearBuffers();
#endif

    //! sets the clear color for one buffer:
    void setClearColor( const std::string &_name, const glm::vec4 &_color );
    //! sets the clear color for one buffer:
    void setClearColor( const std::string &_name, const glm::ivec4 &_color );
    //! sets the clear color for one buffer:
    void setClearColor( const std::string &_name, const glm::uvec4 &_color );

    //! sets the clear color for all color buffers:
    void setClearColor( const glm::vec4 &_color );
    //! sets the clear color for all color buffers:
    void setClearColor( const glm::ivec4 &_color );
    //! sets the clear color for all color buffers:
    void setClearColor( const glm::uvec4 &_color );

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLuint        mObjectName;
    AttachmentVec mColorAttachments;
    Attachment    mDepthAttachment;   // depth and stencil are combined
};

ACGL_SMARTPOINTER_TYPEDEFS(FrameBufferObject)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_FRAMEBUFFEROBJECT_HH
