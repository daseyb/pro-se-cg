/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Helper function for writing the contents of a Texture object into a file
 * and loading them from a file.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Data/ColorSpace.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>

#include <string>

namespace ACGL{
namespace OpenGL{

//! loads the texture and creates mip maps
SharedTexture2D loadTexture2D(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

#ifdef ACGL_OPENGL_SUPPORTS_S3TC
//! loads the texture including mipmaps from a DDS file
//! supports DXT1, DXT3 and DXT5 compression
SharedTexture2D      loadTexture2DFromDDS     (const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);
SharedTexture3D      loadTexture3DFromDDS     (const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);
SharedTextureCubeMap loadTextureCubeMapFromDDS(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);
#endif

}
}
