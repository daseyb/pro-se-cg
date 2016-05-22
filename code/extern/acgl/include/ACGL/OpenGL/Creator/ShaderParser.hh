/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <vector>
#include <string>

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>

namespace ACGL{
namespace OpenGL{

/**
 * @brief ShaderParser for processing shader source code
 *
 * Subclass this and provide alternative ShaderParserFactories to Creators in order to implement custom shader processing
 * Override processPragma in order to create custom pragma handling
 *
 * For an overview and an example, see http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/Custom_shader_parser
 */
class ShaderParser
{
    //ACGL_NOT_COPYABLE(ShaderParser)

public:
    ShaderParser( const std::string &_filename );
    virtual ~ShaderParser() { }

    std::vector< std::string > getSources() const { return mSources; }
    std::string getFileNamesPrintable() const;

    //! the imported sources don't count the original file!
    unsigned int getNumberOfImportedFiles() const { return (unsigned int) mSourceFileNames.size()-1; }

    //! 0 == original file
    std::string getFileName( unsigned int i ) const { return mSourceFileNames[i]; }

    /**
     * @brief checks if a given Sourcefile name exists
     * @param _filename filename to check
     * @return true, iff mSourceFileNames contains _filename
     */
    bool existsSourcefile(std::string const& _filename);

    /**
     * @brief parses a complete file
     * @param _filename the file to parse
     */
    void parse(std::string const& _filename);

protected:

    /**
     * @brief registers a new SourceFile
     * @param _name name of the file (may be an actual filename or a name that indicates automatic generation)
     * @return an ID that is to be used in #line statements and getFileName-Index.
     */
    int registerSourceFile(std::string const& _name);

    /**
     * @brief adds a chunk of source to the shader
     * @param _source source string
     *
     * Does not have to correspond to files
     */
    void addSource(std::string const& _source);

    /**
     * @brief constructs a line directive
     * @param _lineNumber line number (starts at 1)
     * @param _fileID file ID (usually from registerSourceFile)
     * @return a string with the format "#line lineNr fileID\n"
     */
    std::string lineDirective(int _lineNumber, int _fileID) const;

    /**
     * @brief processes a custom Pragma directive
     * @param _tokens vector of tokens after #pragma (e.g. "#pragma myPragma 5" results in _tokens = { "myPragma", "5" })
     * @param _filename name of the current source file (changes for different includes, does not reflect custom file names)
     *
     * Override this function to implement custom pragma handling (no need to call base function)
     *
     * Recommended behavior:
     * - Check if pragma is valid, otherwise call base::processPragma
     * - Start with lineNumber 1
     * - Register appropriate source file (using registerSourceFile(...))
     * - Initialize source content with lineDirective(lineNumber, fileID)
     * - Add custom source content
     * - Call addSource(...) with your source content
     * - Return true
     */
    virtual bool processPragma(std::vector<std::string> const& _tokens, std::string const& _filename);

    /**
     * @brief reads in a source file from file system
     * @param _filename filename of the source file
     *
     * Override this function to implement custom file reading functionality
     */
    virtual void readin( const std::string &_filename );

private:
    bool lineContainsVersion( const std::string &line, unsigned int &version);
    bool lineContainsImport( const std::string &line, std::string &filename);
    bool lineContainsPragma( const std::string &line, std::vector<std::string> &pragmaToken);

    std::vector< std::string > mSources;
    std::vector< std::string > mSourceFileNames; // holds at least one filename
    unsigned int mMaxVersion;
};
ACGL_SMARTPOINTER_TYPEDEFS(ShaderParser)

/**
 * @brief A ShaderParser that recognizes shader includes
 *
 * e.g.
 * #pragma import "matrices.glsl"
 */
class IncludingShaderParser : public ShaderParser
{
public:
    /// c'tor
    IncludingShaderParser( std::string const& _filename );

    /**
     * Processes "#pragma import filename"
     */
    virtual bool processPragma(std::vector<std::string> const& _tokens, std::string const& _filename);
};
ACGL_SMARTPOINTER_TYPEDEFS(IncludingShaderParser)

} // OpenGL
} // ACGL
