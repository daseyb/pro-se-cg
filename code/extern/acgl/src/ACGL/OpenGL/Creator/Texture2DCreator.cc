/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Data/TextureDataLoadStore.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;

SharedTexture2D Texture2DCreator::create()
{
    return loadTexture2D( getFullFilePath() );
}

bool Texture2DCreator::update(SharedTexture2D& _texture)
{
    if (fileIsUpToDate() || !_texture)
        return false;

    SharedTextureData data = loadTextureData( getFullFilePath() );
    if(!data)
        return false;

    _texture->setImageData( data );

    // if the min filter is one of the mip-mapped filters, generate mipmaps:
    GLenum minFilter = _texture->getMinFilter();
    if (   minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_NEAREST
         ||minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_LINEAR) {
        _texture->generateMipmaps();
    }

    updateFileModificationTime();
    return true;
}
