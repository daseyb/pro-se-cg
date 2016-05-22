/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/ShaderParser.hh>
#include <ACGL/Base/Settings.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

ShaderParser::ShaderParser( const std::string &/*_filename */)
{
    // moved to parse
    // calling virtual functions in c'tor is quite the bad idea
}


bool ShaderParser::lineContainsVersion( const std::string &line, unsigned int &version)
{
    std::vector< std::string > token = StringHelpers::split( line, ' ' );

    if (token.size() >= 2 && token[0] == "#version") {
        version = StringHelpers::to<unsigned int>(token[1]);
        //debug() << "string " << line << " -> version " << version << std::endl;
        return true;
    }
    return false;
}

bool ShaderParser::lineContainsImport( const std::string &line, std::string &filename)
{
    std::vector< std::string > token = StringHelpers::split( line, ' ' );

    if (token.size() >= 3 && token[0] == "#pragma" && token[1] == "ACGLimport") {
        if (token[2].size() >= 2) {

            size_t startName = line.find('"',0);
            if (startName == std::string::npos) return false;
            size_t endName = line.find('"',startName+1);
            if (endName == std::string::npos) return false;

            filename = line.substr(startName+1, endName-startName-1); // remove the ""

            //debug() << "string " << line << " -> import " << filename << std::endl;
            return true;
        }
    }
    return false;
}

bool ShaderParser::lineContainsPragma(const std::string &line, std::vector<std::string> &pragmaToken)
{
    std::vector< std::string > token = StringHelpers::split( line, ' ' );

    if (token.size() >= 2 && token[0] == "#pragma")
    {
        pragmaToken.clear();
        pragmaToken.insert(pragmaToken.begin(), token.begin() + 1, token.end());

        return true;
    }
    return false;
}

std::string ShaderParser::getFileNamesPrintable() const
{
    std::string rString;
    for (size_t i = 0; i < mSourceFileNames.size(); ++i) {
        rString += " " + StringHelpers::toString<unsigned int>( (unsigned int)i+1) + " " + mSourceFileNames[i] + "\n";
    }
    return rString;
}

bool ShaderParser::existsSourcefile(const std::string &_filename)
{
    return std::find(mSourceFileNames.begin(), mSourceFileNames.end(), _filename) != mSourceFileNames.end();
}

void ShaderParser::parse(const std::string &_filename)
{
    mMaxVersion = 110;

    mSources.clear();
    mSourceFileNames.clear();

    mSources.push_back( "#version 330\n" );
    std::string path, file;
    StringHelpers::splitLastFileOrFolder( _filename, path, file );
    //readin( "./"+path+"/"+file );
    readin( _filename );

#ifndef ACGL_OPENGL_ES
    if (mMaxVersion > 110) {
        mSources[0] = "#version "+StringHelpers::toString(mMaxVersion)+"\n";
    }
#else
#if (ACGL_OPENGLES_VERSION == 20)
    mSources[0] = "#version 200 es\n";
#else
    mSources[0] = "#version 300 es\n";
#endif
    mSources[0] = mSources[0]+"#define ACGL_OPENGL_ES\n";
#endif
}

int ShaderParser::registerSourceFile(const std::string &_name)
{
    mSourceFileNames.push_back(_name);
    return (int) mSourceFileNames.size();
}

void ShaderParser::addSource(const std::string &_source)
{
    mSources.push_back(_source);
}

std::string ShaderParser::lineDirective(int _lineNumber, int _fileID) const
{
    return "#line "+StringHelpers::toString(_lineNumber)+" "+StringHelpers::toString(_fileID)+"\n";
}

bool ShaderParser::processPragma(std::vector<std::string> const&, std::string const&)
{
    return false;
}

void ShaderParser::readin( const std::string &_filename )
{
    std::string line = "";

    std::ifstream fileStream(_filename.c_str(), std::ifstream::in);

    unsigned int lineNumber = 1;
    unsigned int fileNumber = registerSourceFile(_filename);

    // define the file and line number to get correct errors from the shader compiler:
    std::string fileContent = lineDirective(lineNumber, fileNumber);

    //debug() << "parse file " << _filename << std::endl;

    if(fileStream.is_open())
    {
        while (fileStream.good())
        {
            std::getline(fileStream,line);

            unsigned int version;
            std::vector<std::string> pragmaToken;
            if ( lineContainsVersion(line, version) )
            {
                mMaxVersion = std::max( version, mMaxVersion );
                fileContent += "\n"; // remove the #version but keep a newline
                lineNumber++;
            }
            else if ( lineContainsPragma(line, pragmaToken) )
            {
                // push what we have till now:
                addSource( fileContent );
                // let subclasses process their own pragmas
                if ( !processPragma(pragmaToken, _filename) )
                    addSource(line + "\n"); // pragma not accepted? print it instead
                // continue with a fresh chunk of source
                lineNumber++;
                fileContent = lineDirective(lineNumber, fileNumber);
            }
            else
            {
                fileContent += line + "\n";
                lineNumber++;
            }
        }
        fileStream.close();
    }
    else
    {
        error() << "Failed to open file: " << _filename << std::endl;
        return;
    }

    //debug() << "file: " << fileContent << std::endl;

    addSource( fileContent );
}


IncludingShaderParser::IncludingShaderParser(const std::string &_filename) :
    ShaderParser(_filename)
{
}

bool IncludingShaderParser::processPragma(const std::vector<std::string> &_tokens, const std::string &_filename)
{
    if ( _tokens.size() == 2 && _tokens[0] == "import" )
    {
        std::string fileToImport(_tokens[1]);

        // strip '"' or '<' also allows mix of them, but not recommended
        bool absolute = false;
        size_t startName = fileToImport.find('"',0);
        if (startName == std::string::npos)
        {
            startName = fileToImport.find('<',0);
            absolute = true;
        }
        if (startName == std::string::npos) return false;
        size_t endName = fileToImport.find('"',startName+1);
        if (endName == std::string::npos) endName = fileToImport.find('>',startName+1);
        if (endName == std::string::npos) return false;

        fileToImport = fileToImport.substr(startName+1, endName-startName-1); // remove the ""

        // handle import
        if (fileToImport[0] != '/')
        {
            // it's a relative path:

            // remove "./" in case this was given explicitly:
            if(fileToImport.size() > 2 && fileToImport[0] == '.' && fileToImport[1] == '/')
                fileToImport = fileToImport.substr(2, fileToImport.size()-2);

            // absolute/relative handling
            std::string path, file;
            if ( absolute )
            {
                path = ACGL::Base::Settings::the()->getFullShaderPath();
                if ( path.size() == 0 )
                    path = ".";
            }
            else
                StringHelpers::splitLastFileOrFolder( _filename, path, file );

            fileToImport = path+"/"+fileToImport;
        }

        // "#pragma once"-behavior
        if ( !existsSourcefile(fileToImport) )
            readin( fileToImport ); // actual include

        // success
        return true;
    }
    else return ShaderParser::processPragma(_tokens, _filename);
}
