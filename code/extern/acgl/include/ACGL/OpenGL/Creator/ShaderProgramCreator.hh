/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Used to create a ShaderProgram from a given set of file names:
 * e.g.:
 *      SharedShaderProgram prog = ShaderProgramCreator("file.vsh").andFile("foobar.fsh").create();
 *
 * The shadertype will be guessed by the extensions.
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Resource/MultiFileBasedCreator.hh>
#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/Base/Settings.hh>

#include <vector>
#include <ACGL/OpenGL/Data/LocationMappings.hh>

#include <ACGL/OpenGL/Creator/ShaderParserFactory.hh>

namespace ACGL{
namespace OpenGL{

class ShaderProgramCreator : public Resource::MultiFileBasedCreator<ShaderProgram>
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    //! The filenames (sorted and concatenated) will also be the name of the resource (can be changed by setResourceName() below).
    //! If the filename has a dot in it (or _type is set), it will be treated as a single file, otherwise all
    //! files starting with that string will be used.
    //! The empty c'tor is supported for containers and everything that needs one
    ShaderProgramCreator(const std::string& _fileName = "", GLenum _type = GL_INVALID_VALUE )
        :   Resource::MultiFileBasedCreator<ShaderProgram>(),
        mShaderType(),
        mAttributeLocations(new LocationMappings),
        mFragmentDataLocations(new LocationMappings),
        mUniformBufferLocations(new LocationMappings) //,
        //mShaderParserFactory(std::make_shared<SimpleShaderParserFactory<IncludingShaderParser> >())
    {
        mShaderParserFactory = SharedShaderParserFactory( new SimpleShaderParserFactory<IncludingShaderParser>() );

        // the base path is only needed for updating the time stamps, as the shaders itself get loaded via ShaderCreators
        // which itself will add the base path! (read: mFileNames will _NOT_ store the base path!)
        mBasePath = Base::Settings::the()->getFullShaderPath();

        if ( _type != GL_INVALID_VALUE ) {
            andFile( _fileName, _type );
            return;
        }
        // only add the first name if it is a valid file name
        if ( _fileName.find( "." ) != std::string::npos ) {
            andFile( _fileName );
        } else {
            autoFiles( _fileName );
        }
    }

    virtual ~ShaderProgramCreator() {}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    ShaderProgramCreator& setResourceName(const std::string &_resourceName) { mResourceName = _resourceName; return *this; }

    //
    // Override shader processing
    //

    /// Sets the shader parser factory
    /// For more detailed documentation, see ShaderParser.hh
    inline ShaderProgramCreator& shaderParserFactory(SharedShaderParserFactory const& _factory) { mShaderParserFactory = _factory; return *this; }

    //
    // Adding files:
    //
    //! adds a single file, the shader type will be guessed by the ending:
    inline ShaderProgramCreator& andFile              (const std::string &_fileName)               { addFile( _fileName ); mShaderType.push_back( GL_INVALID_VALUE ); return *this; }

    //! adds a single file, the shader type is explicitly given and must be one of:
    //! GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER
    inline ShaderProgramCreator& andFile              (const std::string &_fileName, GLenum _type) { addFile( _fileName ); mShaderType.push_back( _type ); return *this; }

    //! adds all files begining with the given name, the shader type will be guessed by the ending:
           ShaderProgramCreator& autoFiles            (const std::string &_fileName);

    //
    // Adding attribute locations:
    //
    //! adds an attribute location to the next free location number:
    inline ShaderProgramCreator& attributeLocation    (const std::string &_attributeName)                      { mAttributeLocations->setLocation(_attributeName);                 return *this; }

    //! adds an attribute location to the given location number:
    inline ShaderProgramCreator& attributeLocation    (const std::string &_attributeName, GLuint _location)    { mAttributeLocations->setLocation(_attributeName,_location);       return *this; }

    //! adds a whole list of mappings
    inline ShaderProgramCreator& attributeLocations   (const SharedLocationMappings &_mapping) { mAttributeLocations->addLocations(_mapping);    return *this; }

    //! links to the external mapping object, earlyer mapping definitions get ignored, following
    //! mappings will also change the external object!
    inline ShaderProgramCreator& externAttributeLocations   (SharedLocationMappings _mapping) { mAttributeLocations = _mapping; return *this; }

    //
    // Adding fragment output locations:
    //
    //! adds a fragment output location to the next free location number:
    inline ShaderProgramCreator& fragmentDataLocation (const std::string &_fragmentDataName)                   { mFragmentDataLocations->setLocation(_fragmentDataName);           return *this; }

    //! adds a fragment output location to the given location number:
    inline ShaderProgramCreator& fragmentDataLocation (const std::string &_fragmentDataName, GLuint _location) { mFragmentDataLocations->setLocation(_fragmentDataName,_location); return *this; }

    //! adds a whole list of mappings
    inline ShaderProgramCreator& fragmentDataLocations(const SharedLocationMappings &_mapping) { mFragmentDataLocations->addLocations(_mapping); return *this; }

    //! links to the external mapping object, earlyer mapping definitions get ignored, following
    //! mappings will also change the external object!
    inline ShaderProgramCreator& externFragmentDataLocations (SharedLocationMappings _mapping) { mFragmentDataLocations = _mapping; return *this; }

    //
    // Adding uniform buffer locations:
    //
    //! adds an attribute location to the next free location number:
    inline ShaderProgramCreator& uniformBufferLocation(const std::string &_uniformBufferName)                  { mUniformBufferLocations->setLocation(_uniformBufferName);         return *this; }

    //! adds an attribute location to the given location number:
    inline ShaderProgramCreator& uniformBufferLocation(const std::string &_uniformBufferName, GLuint _location){ mUniformBufferLocations->setLocation(_uniformBufferName,_location);return *this; }

    //! adds a whole list of mappings
    inline ShaderProgramCreator& uniformBufferLocations(SharedLocationMappings _mapping) { mUniformBufferLocations->addLocations(_mapping);return *this; }

    inline ShaderProgramCreator& externUniformBufferLocations(SharedLocationMappings _mapping) { mUniformBufferLocations = _mapping; return *this; }

    // ===================================================================================================== \/
    // ============================================================================================ OVERRIDE \/
    // ===================================================================================================== \/
public:
    virtual SharedShaderProgram create();
    virtual bool update(SharedShaderProgram &_shaderProgram);

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    std::vector<GLenum>      mShaderType;

    SharedLocationMappings mAttributeLocations;
    SharedLocationMappings mFragmentDataLocations;
    SharedLocationMappings mUniformBufferLocations;

    SharedShaderParserFactory mShaderParserFactory;

private:
    // set attribute, UBO & fragdata locations and links the program
    bool setBindings(SharedShaderProgram &_shaderProgram);
};

} // OpenGL
} // ACGL
