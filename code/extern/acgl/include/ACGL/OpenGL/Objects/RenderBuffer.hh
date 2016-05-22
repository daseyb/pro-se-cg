/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_RENDERBUFFER_HH
#define ACGL_OPENGL_OBJECTS_RENDERBUFFER_HH

/**
 * An OpenGL RenderBuffer that can be used with FBOs.
 *
 * A RenderBuffer is an alternative to a texture as a render target if the later
 * usage as a texture is not needed (e.g. as a depth attachment if only the
 * color attachments are needed from the offscreen renderpass).
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/Math/Math.hh>

namespace ACGL{
namespace OpenGL{

class RenderBuffer
{
    ACGL_NOT_COPYABLE(RenderBuffer)

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    RenderBuffer(
        GLenum _internalFormat)
    :   mObjectName(0),
        mInternalFormat(_internalFormat),
        mWidth(0),
        mHeight(0)
    {
        mObjectName = 0;
        glGenRenderbuffers(1, &mObjectName);
    }

    virtual ~RenderBuffer(void)
    {
        // buffer 0 will get ignored by OpenGL
        glDeleteRenderbuffers(1, &mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#ifdef ACGL_OPENGL_DEBUGGER_SUPPORT
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_RENDERBUFFER>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_RENDERBUFFER>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint  getObjectName     (void) const { return mObjectName;     }
    inline GLenum  getInternalFormat (void) const { return mInternalFormat; }
    inline GLsizei getWidth          (void) const { return mWidth;          }
    inline GLsizei getHeight         (void) const { return mHeight;         }
    inline glm::uvec2 getSize        (void) const { return glm::uvec2( mWidth, mHeight ); }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! Get the actual number of samples
#ifdef ACGL_OPENGL_ES
    inline int_t getSamples(void) const { return 1; }
#else
    inline int_t getSamples(void) const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, mObjectName);
        GLint samples;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &samples);
        return (int_t)samples;
    }
#endif

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:    
    //! Bind the renderbuffer
    inline void bind(void) const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, mObjectName);
    }

    //! Set texture size and NULL data
    inline void setSize(
        GLsizei _width,
        GLsizei _height,
        GLsizei _samples = 0)
    {
        mWidth = _width;
        mHeight = _height;
        // the sample count will not get saved as the real samples returned by GL might differ this number anyway!
        glBindRenderbuffer(GL_RENDERBUFFER, mObjectName);

#if (ACGL_OPENGL_VERSION >= 30)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, mInternalFormat, mWidth, mHeight);
#else // OpenGL ES, as Desktop GL didn't have renderbuffers pre 3.0 anyway
        glRenderbufferStorage(GL_RENDERBUFFER, mInternalFormat, mWidth, mHeight);
#endif // OpenGL >= 3.0

    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLuint  mObjectName;
    GLenum  mInternalFormat;
    GLsizei mWidth;
    GLsizei mHeight;
};

ACGL_SMARTPOINTER_TYPEDEFS(RenderBuffer)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_RENDERBUFFER_HH
