/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_DATA_COLORSPACE_HH
#define ACGL_OPENGL_DATA_COLORSPACE_HH

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

enum ColorSpace
{
    AUTO_DETECT,
    LINEAR,
    SRGB
};

//! Recommends an OpenGL internal format for a given pair of format and color spaces
GLenum recommendedInternalFormat(GLenum _format, ColorSpace _colorSpace);

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_DATA_COLORSPACE_HH
