/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>

#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>
#include <ACGL/Base/Settings.hh>

namespace ACGL{
namespace OpenGL{

class Texture2DCreator : public Resource::SingleFileBasedCreator<Texture2D>
{
public:
    Texture2DCreator(const std::string& _filename)
        : Resource::SingleFileBasedCreator<Texture2D>(_filename, Base::Settings::the()->getFullTexturePath() ) {}
    virtual ~Texture2DCreator() {}

    Texture2DCreator& setResourceName(const std::string &_resourceName) { mResourceName = _resourceName; return *this; }

    //! try to create a 2D mip-mapped texture
    virtual SharedTexture2D create();

    //! update the texture data, create mipmaps if mipmapping is on. Don't change any other settings
    //! (e.g. filtering settings)
    virtual bool update(SharedTexture2D& _texture);
};

} // OpenGL
} // ACGL
