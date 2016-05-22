/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>

#include <ACGL/Resource/MultiFileBasedCreator.hh>
#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/Base/Settings.hh>

#include <vector>
#include <ACGL/OpenGL/Data/LocationMappings.hh>

namespace ACGL{
namespace OpenGL{

/**
 * @brief Factory for shader parser
 *
 * This can be used in in the shader program creator to create post-processes for shader
 *
 * For more detailed documentation, see ShaderParser.hh
 */
class ShaderParserFactory
{
    ACGL_NOT_COPYABLE(ShaderParserFactory)

public:
    ShaderParserFactory() { }
    virtual ~ShaderParserFactory() { }

    /**
     * @brief creates a Parser for a given source file
     * @param _filename filename of the shader that is going to be compiled
     * @return a new ShaderParser instance
     *
     * Returns the a default shader parse that is able to include other shader
     * Override this function to create your own functionality
     */
    virtual SharedShaderParser createParser(std::string const& _filename) = 0;
};
ACGL_SMARTPOINTER_TYPEDEFS(ShaderParserFactory)


/**
 * @brief A simple templated shader parser factory
 *
 * calls std::make_shared<ShaderParserT>(_filename), so make sure that the (string const&)-c'tor is publicly accessible
 */
template<typename ShaderParserT>
class SimpleShaderParserFactory : public ShaderParserFactory
{
public:
    /// see parent
    virtual SharedShaderParser createParser(std::string const& _filename)
    {
        //return std::make_shared<ShaderParserT>(_filename);
        return SharedShaderParser( new ShaderParserT(_filename) );
    }
};

} // OpenGL
} // ACGL
