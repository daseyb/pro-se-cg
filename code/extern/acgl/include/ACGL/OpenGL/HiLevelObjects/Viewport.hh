/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_VIEWPORT_HH
#define ACGL_OPENGL_OBJECTS_VIEWPORT_HH

#include <vector>
#include <string>
#include <iostream>

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

class Viewport
{
    ACGL_NOT_COPYABLE(Viewport)

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    Viewport(GLint   _offsetX = 0,
             GLint   _offsetY = 0,
             GLsizei _width = 0,
             GLsizei _height = 0)
    :   mOffsetX(_offsetX),
        mOffsetY(_offsetY),
        mWidth(_width),
        mHeight(_height)
    { }

    virtual ~Viewport(void) { }

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLint   getOffsetX (void) const { return mOffsetX; }
    inline GLint   getOffsetY (void) const { return mOffsetY; }
    inline GLsizei getWidth   (void) const { return mWidth;   }
    inline GLsizei getHeight  (void) const { return mHeight;  }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    inline void use (void) const
    {
        glViewport(mOffsetX, mOffsetY, mWidth, mHeight);
    }

    inline void setOffset (GLint _offsetX, GLint _offsetY)
    {
        mOffsetX = _offsetX;
        mOffsetY = _offsetY;
    }

    inline void setSize (GLsizei _width, GLsizei _height)
    {
        mWidth = _width;
        mHeight = _height;
    }

    //! gets the current viewport form OpenGL
    void setFromGLContext()
    {
        GLint viewport[4];
        glGetIntegerv( GL_VIEWPORT, &(viewport[0]) );
        setOffset(viewport[0], viewport[1]);
        setSize(  viewport[2], viewport[3]);
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLint   mOffsetX;
    GLint   mOffsetY;
    GLsizei mWidth;
    GLsizei mHeight;
};

ACGL_SMARTPOINTER_TYPEDEFS(Viewport)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_STATE_HH

