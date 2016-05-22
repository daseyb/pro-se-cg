/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Creator/ShaderCreator.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Resource/FileManager.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/Base/Settings.hh>
#include <ACGL/Utils/FileHelpers.hh>

#include <sstream>
#include <algorithm>

using namespace ACGL::Utils;
using namespace ACGL::Base;
using namespace ACGL::OpenGL;

ShaderProgramCreator& ShaderProgramCreator::autoFiles(const std::string &_fileName)
{
    //Utils::debug() << "autoFiles: " << _fileName << std::endl;
    std::string baseFileName = _fileName + ".";
    for (unsigned int ending = 0; ending < sizeof(sShaderEndings) / sizeof(ShaderEndings); ++ending)
    {
        std::string fileName = baseFileName + sShaderEndings[ending].ending;
        if ( FileHelpers::fileExists( mBasePath + fileName ) )
        {
            addFile( fileName ); // DON'T store a version with the full shader path
            mShaderType.push_back( sShaderEndings[ending].type );
            //Utils::debug() << "autoFiles: added " << fileName << " - type: " << sShaderEndings[ending].ending << std::endl;
        }
    }
    return *this;
}

SharedShaderProgram ShaderProgramCreator::create()
{
    SharedShaderProgram shaderProgram(new ShaderProgram());

    std::vector<std::string>::size_type numberOfFiles = mFileNames.size();
    if (numberOfFiles == 0) {
        Utils::error() << "Can't create ShaderProgram from 0 files!" << std::endl;
        return SharedShaderProgram();
    }

    // some of the files have correct types already, some not yet -> try to guess the rest
    for (std::vector<std::string>::size_type file = 0; file < numberOfFiles; ++file) {

        if ( mShaderType[file] == GL_INVALID_VALUE ) {
            // guess the shader type:
            mShaderType[file] = ACGL::OpenGL::getShaderTypeByFileEnding( mFileNames[file] );
        }
    }

    for (std::vector<std::string>::size_type i = 0; i < numberOfFiles; ++i) {
        // check for problems:

        if ( mShaderType[i] == GL_INVALID_VALUE ) {
            Utils::error() << "file extension of file " << mFileNames[i] << " not recognized - ignored" << std::endl;
        }

        if (! FileHelpers::fileExists( Settings::the()->getFullShaderPath() + mFileNames[i] ) ) {
            Utils::warning() << "file " << mFileNames[i] << " does not exist - ignored ( full path: "
                             << Settings::the()->getFullShaderPath() + mFileNames[i] << " )" << std::endl;
            mShaderType[i] = GL_INVALID_VALUE;
        }

        if ( (mShaderType[i] == GL_GEOMETRY_SHADER) && !OpenGL::doesSupportGeometryShader() ) {
            Utils::error() << "file " << mFileNames[i] << " ignored, hardware does not support geometry shader" << std::endl;
            mShaderType[i] = GL_INVALID_VALUE;
        }

        if (    ((mShaderType[i] == GL_TESS_CONTROL_SHADER) || (mShaderType[i] == GL_TESS_EVALUATION_SHADER))
             && !OpenGL::doesSupportTessellationShader() ) {
            Utils::error() << "file " << mFileNames[i] << " ignored, hardware does not support tessellation shader" << std::endl;
            mShaderType[i] = GL_INVALID_VALUE;
        }

        // attach shader
        if ( mShaderType[i] != GL_INVALID_VALUE ) {
            ConstSharedShader shader = ShaderFileManager::the()->get(ShaderCreator( mFileNames[i] ).type( mShaderType[i] ).shaderParserFactory(mShaderParserFactory) );
            if ( shader ) {
                shaderProgram->attachShader( shader );
            } else {
                Utils::error() << "could not attach shader " << mFileNames[i] << std::endl;
            }
        }
    }

    if (!setBindings( shaderProgram )) {
        return SharedShaderProgram(); // e.g. linking failed
    }

    updateFileModificationTimes();
    return shaderProgram;
}

// will get called from the create and update functions:
bool ShaderProgramCreator::setBindings(SharedShaderProgram &_shaderProgram)
{
    _shaderProgram->link();

#   if (ACGL_OPENGL_VERSION >= 30)
        _shaderProgram->setFragmentDataLocations( mFragmentDataLocations ); // might relink on it's own

        SharedLocationMappings oldAttributeMap = _shaderProgram->getAttributeLocations();
        mAttributeLocations->addLocations( oldAttributeMap ); // add as many old locations as possible without destoying the location map
        _shaderProgram->setAttributeLocations( mAttributeLocations ); // might relink on it's own
#   else
        if ( (mAttributeLocations->getSize() > 0) && (mFragmentDataLocations->getSize() > 0) ) {
            Utils::error() << "can't set explicit attribute/fragdata locations on OpenGL < 3.0" << std::endl;
        }
#   endif

    // in case the attribute/fragdata locations did not change, those calls will not call glLinkProgram
    if (_shaderProgram->link() == false) return false;

    // uniform block binding have to be set after linking!
    if ( mUniformBufferLocations->getSize() > 0) {
#       if (ACGL_OPENGL_VERSION >= 31)
            LocationMappings::LocationMap map = mUniformBufferLocations->getLocations();
            LocationMappings::LocationMap::const_iterator end = map.end();
            for (LocationMappings::LocationMap::const_iterator it = map.begin(); it != end; ++it) {
                _shaderProgram->setUniformBlockBinding( it->first, it->second );
            }
#       else
            Utils::error() << "can't set uniform buffer locations on OpenGL < 3.1" << std::endl;
#       endif
    }

    return true;
}

bool ShaderProgramCreator::update(SharedShaderProgram &_shaderProgram)
{
    bool update = false;

    for (std::vector<std::string>::size_type i = 0; i < mFileNames.size(); ++i) {
        if ( mShaderType[i] != GL_INVALID_VALUE ) {
            update |= ShaderFileManager::the()->update( mFileNames[i] );
        }
    }

    if ((!filesAreUpToDate() ) || (update))
    {
        // at least one shader was updated, so update the program:
        // keep the old mappings and relink:
        Utils::debug() << "updating " << mResourceName << std::endl;
        bool shaderProgramOK = setBindings( _shaderProgram ); // will relink, and set UBO bindings (don't relink after this!)
        updateFileModificationTimes();
        return shaderProgramOK;
    }

    return false;
}
