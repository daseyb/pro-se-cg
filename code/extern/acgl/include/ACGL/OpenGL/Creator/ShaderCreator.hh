/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Can create one Shader from one file.
 * Most of the time using this directly is not useful as the creation
 * of a whole ShaderProgam is desired.
 *
 * See ShaderProgramCreator for more information!
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/Base/Settings.hh>
#include <ACGL/OpenGL/Objects/Shader.hh>
#include <ACGL/OpenGL/GL.hh>

#include <ACGL/OpenGL/Creator/ShaderParserFactory.hh>

namespace ACGL{
namespace OpenGL{

class ShaderCreator : public Resource::SingleFileBasedCreator<Shader>
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    ShaderCreator(const std::string& _filename)
    :   Resource::SingleFileBasedCreator<Shader>(_filename, Base::Settings::the()->getFullShaderPath()),
        mType(GL_INVALID_ENUM)
    {
        mShaderParserFactory = SharedShaderParserFactory( new SimpleShaderParserFactory<IncludingShaderParser>() );
    }
    virtual ~ShaderCreator() {}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    inline ShaderCreator& type (GLenum _type) { mType = _type;  return *this; }

    ShaderCreator& setResourceName(const std::string &_resourceName) { mResourceName = _resourceName; return *this; }

    //
    // Override shader processing
    //

    /// Sets the shader parser factory
    /// For more detailed documentation, see ShaderParser.hh
    inline ShaderCreator& shaderParserFactory(SharedShaderParserFactory const& _factory) { mShaderParserFactory = _factory; return *this; }

    // ===================================================================================================== \/
    // ============================================================================================ OVERRIDE \/
    // ===================================================================================================== \/
public:
    virtual SharedShader create();
    virtual bool update(SharedShader& shader);

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLenum mType;

    SharedShaderParserFactory mShaderParserFactory;

    struct ImportedShader {
        std::string fileName;
        Utils::FileHelpers::FileModificationTime modificatonTime = 0;
    };

    std::vector< ImportedShader > mImportedShaders; // in case the shader imports additional shaders files
};

} // OpenGL
} // ACGL


