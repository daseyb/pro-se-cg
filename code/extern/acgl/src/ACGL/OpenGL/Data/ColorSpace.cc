/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/ColorSpace.hh>

// Format conversion helpers
namespace
{

GLenum formatToSRGB(GLenum _format)
{
    switch(_format)
    {
        // Core
        case GL_RGB:                                      return GL_SRGB;
        case GL_RGB8:                                     return GL_SRGB8;
        case GL_RGBA:                                     return GL_SRGB_ALPHA;
        case GL_RGBA8:                                    return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_RGB:                           return GL_COMPRESSED_SRGB;
        case GL_COMPRESSED_RGBA:                          return GL_COMPRESSED_SRGB_ALPHA;
            
#if defined(ACGL_OPENGL_SUPPORTS_ETC)
        case GL_COMPRESSED_RGB8_ETC2:                     return GL_COMPRESSED_SRGB8_ETC2;
        case GL_COMPRESSED_RGBA8_ETC2_EAC:                return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2: return GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
#endif

#if defined(ACGL_OPENGL_SUPPORTS_S3TC)
        // EXT_texture_sRGB, EXT_texture_compression_s3tc
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:             return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
#endif
        // OpenGL 4.2?
        //case GL_COMPRESSED_RGBA_BPTC_UNORM:               return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
    }
    return _format;
}

}

namespace ACGL {
namespace OpenGL {

GLenum recommendedInternalFormat(GLenum _format, ColorSpace _colorSpace)
{
    switch(_colorSpace)
    {
        case ColorSpace::SRGB: return formatToSRGB(_format);
        default:               return _format;
    }
}

}
}
