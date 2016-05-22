/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/ShaderCreator.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/OpenGL/Tools.hh>

using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

SharedShader ShaderCreator::create()
{
    updateFileModificationTime();

    if (mType == GL_INVALID_ENUM)
    {
        mType = ACGL::OpenGL::getShaderTypeByFileEnding( mFilename );
        if (mType == GL_INVALID_ENUM) return SharedShader();
    }

    SharedShader shader(new Shader(mType));
    SharedShaderParser sp( mShaderParserFactory->createParser( mFullFilePath ) );
    sp->parse( mFullFilePath );
    if (shader->setFromFile( sp )) {
        unsigned int importedSourcesCount = sp->getNumberOfImportedFiles();
        mImportedShaders.reserve( importedSourcesCount );
        for (unsigned int i = 1; i <= importedSourcesCount; ++i) {
            ImportedShader is;
            is.fileName = sp->getFileName( i );
            is.modificatonTime = FileHelpers::getFileModificationTime( is.fileName );
            mImportedShaders.push_back( is );
        }
        return shader;
    }
    return SharedShader();
}

bool ShaderCreator::update(SharedShader& shader)
{
    bool shaderIsUpToDate = fileIsUpToDate();
    for (unsigned int i = 0; i < mImportedShaders.size(); ++i) {
        if ( Utils::FileHelpers::getFileModificationTime(mImportedShaders[i].fileName) != mImportedShaders[i].modificatonTime ) {
            // imported shader changed
            shaderIsUpToDate = false;
            break; // one changed imported shader is enough to justify a full shader reload
        }
    }

    if (shaderIsUpToDate) {
        return false; // false means nothing was updated
    }

    { // try to compile the source in another shader, only proceed if that worked!
        Shader dummy( shader->getType() );
        SharedShaderParser sp( mShaderParserFactory->createParser( mFullFilePath ) );
        sp->parse( mFullFilePath );
        if (!dummy.setFromFile( sp )) {

            // we had a shader compile error, update the timestamp to prevent a second try
            // of compiling the exact same (non-working) shader again
            updateFileModificationTime();
            return false;
        } else {
            // the newly loaded base shader might import other subshaders!
            unsigned int importedSourcesCount = sp->getNumberOfImportedFiles();
            mImportedShaders.clear();
            mImportedShaders.reserve( importedSourcesCount );
            for (unsigned int i = 1; i <= importedSourcesCount; ++i) {
                ImportedShader is;
                is.fileName = sp->getFileName( i );
                is.modificatonTime = FileHelpers::getFileModificationTime( is.fileName );
                mImportedShaders.push_back( is );
            }
        }
    }

    // it worked, so load the source in "our" shader:
    {
        SharedShaderParser sp( mShaderParserFactory->createParser( mFullFilePath ) );
        sp->parse( mFullFilePath );
        shader->setFromFile(sp);
    }

    updateFileModificationTime();
    return true;
}
