/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/Shader.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

/*
bool Shader::setFromFileNoImportParsing(const std::string& _filename)
{
    std::string line = "";
    std::string fileContent = "";
    
    std::ifstream fileStream(_filename.c_str(), std::ifstream::in);
    
    if(fileStream.is_open())
    {
        while (fileStream.good())
        {
            std::getline(fileStream,line);
            fileContent += line + "\n";
        }
        fileStream.close();
    }
    else
    {
        error() << "Failed to open file: " << _filename << std::endl;
        return false;
    }
    
    bool compileErrors = true;
    if ( setSource(fileContent, false) ) { // don't check for errors, we will do that on our own:
        std::string compileLog;
        getCompileLog( compileLog, compileErrors );
        if (compileLog.size() > 0) {
            if (compileErrors) {
                error() << "\nIn file: " << _filename << ": \n" << compileLog << "\n" << std::endl;
            } else {
                warning() << "\nIn file: " << _filename << ": \n" << compileLog << "\n" << std::endl;
            }
        }
    }
    return !compileErrors; // return true iff there were no errors
}*/

bool Shader::setFromFile(SharedShaderParser const& _sp)
{
    assert(_sp->getSources().size() > 0); // did you forget to call _sp->parse(...)?

    bool compileErrors = true;
    if ( setSources( _sp->getSources(), false) ) { // don't check for errors, we will do that on our own:
        std::string compileLog;
        getCompileLog( compileLog, compileErrors );
        if (compileLog.size() > 0) {
            if (compileErrors) {
                error() << "\nIn files: \n" << _sp->getFileNamesPrintable() << compileLog << "\n" << std::endl;
            } else {
                warning() << "\nIn files: \n" << _sp->getFileNamesPrintable() << compileLog << "\n" << std::endl;
            }
        }
    }

    setObjectLabel( _sp->getFileName(0).c_str() );

    return !compileErrors; // return true iff there were no errors
}

bool Shader::setSource(const std::string& _source, bool _checkForCompileErrors)
{
    const char *pProgramString = _source.c_str();
    glShaderSource(mObjectName, 1, &pProgramString, NULL); // can't create OpenGL errors
    if (!compile()) {
        return false;
    }
    // the compile call should work even if there are compile errors itself:
    bool compileErrors = false;
    if (_checkForCompileErrors) {
        std::string compileLog;
        getCompileLog( compileLog, compileErrors );
        if (compileLog.size() > 0) {
            if (compileErrors) {
                error() << compileLog << std::endl;
            } else {
                warning() << compileLog << std::endl;
            }
        }
    }
    return !compileErrors; // return true iff there were NO errors
}

bool Shader::setSources(const std::vector<std::string> &_sources, bool _checkForCompileErrors )
{
    unsigned int numberOfStrings = (unsigned int) _sources.size();
    const char **pProgramStrings = new const char*[ numberOfStrings ];

    for (unsigned int i = 0; i < numberOfStrings; ++i) {
        pProgramStrings[i] = _sources[i].c_str();
    }

    glShaderSource(mObjectName, numberOfStrings, pProgramStrings, NULL); // can't create OpenGL errors
    delete[] pProgramStrings;

    if (!compile()) {
        return false;
    }
    // the compile call should work even if there are compile errors itself:
    bool compileErrors = false;
    if (_checkForCompileErrors) {
        std::string compileLog;
        getCompileLog( compileLog, compileErrors );
        if (compileLog.size() > 0) {
            if (compileErrors) {
                error() << compileLog << std::endl;
            } else {
                warning() << compileLog << std::endl;
            }
        }
    }
    return !compileErrors; // return true iff there were NO errors
}

bool Shader::compile() const
{
    glCompileShader(mObjectName);
    return true;
}

void Shader::getCompileLog( std::string &_log, bool &_wasErrorLog ) const
{
    // check for shader compile errors:
    GLint shaderError;
    glGetShaderiv(mObjectName, GL_COMPILE_STATUS, &shaderError);
    if(shaderError != GL_TRUE)
    {
        // yes, errors
        _wasErrorLog = true;
    } else {
        _wasErrorLog = false;
    }

    // the log could be warnings:
    GLsizei length = 0;
    glGetShaderiv(mObjectName, GL_INFO_LOG_LENGTH, &length);
    if(length > 1) // null terminated, so 1 could also be empty
    {
        // a compile log can get produced even if there were no errors, but warnings!
        GLchar* pInfo = new char[length];
        glGetShaderInfoLog(mObjectName,  length, &length, pInfo);
        //error() << "Compile log: " << std::string(pInfo) << std::endl;
        _log = std::string( pInfo );
        delete[] pInfo;
    } else {
        _log = "";
    }
}
