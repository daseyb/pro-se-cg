/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <nv_dds/nv_dds.h>

#include <fstream>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

#ifdef ACGL_OPENGL_SUPPORTS_S3TC

using namespace nv_dds;

namespace {

GLenum getDDSInternalFormat(bool compressed, GLenum format, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT)
{
    _CRT_UNUSED(compressed);
    GLenum internal_format = format;

    // Internal format for uncompressed color formats
    // For compressed textures, internal_format == image.get_format(), i.e. no conversion
    if(format == GL_BGRA) internal_format = GL_RGBA;
    if(format == GL_BGR)  internal_format = GL_RGB;
    if(format == GL_RED)  internal_format = GL_RED;

    // Transform internal format according to desired color space
    internal_format = recommendedInternalFormat(internal_format, _colorSpace);

    return internal_format;
}

// To prevent code duplication, wrap glTexImage calls to take a compressed parameter

void texImage2D(bool compressed, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
    if(compressed)
        glCompressedTexImage2D(target, level, internalformat, width, height, 0, imageSize, data);
    else
        glTexImage2D(target, level, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void texImage3D(bool compressed, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data)
{
    if(compressed)
        glCompressedTexImage3D(target, level, internalformat, width, height, depth, 0, imageSize, data);
    else
        glTexImage3D(target, level, internalformat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
}

}

namespace ACGL {
namespace OpenGL {

SharedTexture2D loadTexture2DFromDDS(const std::string& _filename, ColorSpace _colorSpace)
{
    CDDSImage image;
    SharedTexture2D texture;

    if(image.load(_filename))
    {
        GLenum internal_format = getDDSInternalFormat(image.is_compressed(), image.get_format(), _colorSpace);

        if(image.is_volume() || image.is_cubemap())
        {
            ACGL::Utils::error() << _filename << " is not a 2D texture" << std::endl;
        }
        else
        {
            // It is a 2D texture
            texture = SharedTexture2D(new Texture2D(glm::uvec2(image.get_width(), image.get_height()), internal_format));
            texture->bind();
            texImage2D(image.is_compressed(), texture->getTarget(), 0, internal_format, image.get_width(), image.get_height(), image.get_format(), image.get_size(), image);

            for(unsigned int i = 0; i < image.get_num_mipmaps(); i++)
            {
                const CSurface& mipmap = image.get_mipmap(i);
                texImage2D(image.is_compressed(), texture->getTarget(), i+1, internal_format, mipmap.get_width(), mipmap.get_height(), image.get_format(), mipmap.get_size(), mipmap);
            }

            texture->setMaxLevel(image.get_num_mipmaps());
        }
    }
    else
    {
        ACGL::Utils::error() << "could not open " << _filename << std::endl;
    }

    texture->setObjectLabel( _filename );
    return texture;
}

SharedTexture3D loadTexture3DFromDDS(const std::string& _filename, ColorSpace _colorSpace)
{
    CDDSImage image;
    SharedTexture3D texture;

    if(image.load(_filename))
    {
        GLenum internal_format = getDDSInternalFormat(image.is_compressed(), image.get_format());

        if(image.is_volume())
        {
            // It is a 3D texture
            texture = SharedTexture3D(new Texture3D(glm::uvec3(image.get_width(), image.get_height(), image.get_depth()), internal_format));
            texture->bind();
            texImage3D(image.is_compressed(), texture->getTarget(), 0, internal_format, image.get_width(), image.get_height(), image.get_depth(), image.get_format(), image.get_size(), image);

            for(unsigned int i = 0; i < image.get_num_mipmaps(); i++)
            {
                const CSurface& mipmap = image.get_mipmap(i);
                texImage3D(image.is_compressed(), texture->getTarget(), i+1, internal_format, mipmap.get_width(), mipmap.get_height(), mipmap.get_depth(), image.get_format(), mipmap.get_size(), mipmap);
            }

            texture->setMaxLevel(image.get_num_mipmaps());
        }
        else
        {
            ACGL::Utils::error() << _filename << " is not a 3D texture" << std::endl;
        }
    }
    else
    {
        ACGL::Utils::error() << "could not open " << _filename << std::endl;
    }

    return texture;
}

SharedTextureCubeMap loadTextureCubeMapFromDDS(const std::string& _filename, ColorSpace _colorSpace)
{
    CDDSImage image;
    SharedTextureCubeMap texture;

    if(image.load(_filename))
    {
        GLenum internal_format = getDDSInternalFormat(image.is_compressed(), image.get_format());

        if(image.is_cubemap())
        {
            // It is a cube map texture
            texture = SharedTextureCubeMap(new TextureCubeMap(glm::uvec2(image.get_width(), image.get_height()), internal_format));
            texture->bind();

            unsigned int numMipmapLevels = 1000;
            for(unsigned int i = 0; i < 6; ++i)
            {
                GLenum target = GL_INVALID_ENUM;
                switch(i)
                {
                    case 0: target = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
                    case 1: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
                    case 2: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
                    case 3: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
                    case 4: target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
                    case 5: target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
                }

                const CTexture& face = image.get_cubemap_face(i);
                texImage2D(image.is_compressed(), target, 0, internal_format, face.get_width(), face.get_height(), image.get_format(), face.get_size(), face);

                numMipmapLevels = std::min(numMipmapLevels, face.get_num_mipmaps());
                for(unsigned int j = 0; j < face.get_num_mipmaps(); j++)
                {
                    const CSurface& mipmap = face.get_mipmap(j);
                    texImage2D(image.is_compressed(), texture->getTarget(), j+1, internal_format, mipmap.get_width(), mipmap.get_height(), image.get_format(), mipmap.get_size(), mipmap);
                }
            }

            texture->setMaxLevel(numMipmapLevels);
        }
        else
        {
            ACGL::Utils::error() << _filename << " is not a cube map texture" << std::endl;
        }
    }
    else
    {
        ACGL::Utils::error() << "could not open " << _filename << std::endl;
    }

    return texture;
}

}
}


#endif

