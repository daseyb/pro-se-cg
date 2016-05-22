/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Data/TextureDataLoadStore.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace ACGL{
namespace OpenGL{

SharedTexture2D loadTexture2D(const std::string& _filename, ColorSpace _colorSpace)
{
    std::string fileEnding = StringHelpers::getFileEnding(_filename);
#if defined(ACGL_OPENGL_SUPPORTS_S3TC)
    if(fileEnding == "dds")
    {
        return loadTexture2DFromDDS(_filename, _colorSpace);
    }
    else
#endif
    {
        SharedTextureData data = loadTextureData(_filename, _colorSpace);

        if (data) {
            //SharedTexture2D texture = std::make_shared<Texture2D>(data->getRecommendedInternalFormat());
            SharedTexture2D texture = SharedTexture2D( new Texture2D(data->getRecommendedInternalFormat()) );
            texture->setImageData( data );
            texture->generateMipmaps(); // calculates all remaining mipmap levels
            texture->setObjectLabel( _filename );

            return texture;
        } else {
            ACGL::Utils::error() << "can't create Texture from file " << _filename << " creating small empty texture instead." << std::endl;
            //SharedTexture2D dummy = std::make_shared<Texture2D>();
            SharedTexture2D dummy = SharedTexture2D(new Texture2D);
            dummy->resize( glm::uvec2(4,4) );
            return dummy;
        }
    }
}

}
}
