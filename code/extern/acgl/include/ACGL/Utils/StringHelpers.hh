/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_STRINGOPERATIONS_HH
#define ACGL_BASE_STRINGOPERATIONS_HH

/*
 * Provides a set of primitive functions which operate on std::strings which are
 * used at multiple points within the library.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>

#include <sstream>
#include <vector>
#include <string>

namespace ACGL{
namespace Utils{

namespace StringHelpers
{
    //! splits the filename _full into the filename without the extension and just the extension
    bool splitFileExtension    (const std::string& _full, std::string& _file, std::string& _extension   );
    bool splitLastFileOrFolder (const std::string& _full, std::string& _path, std::string& _fileOrFolder);

    //! returns true iff the _string starts with a given prefix
    bool startsWith            (const std::string& _string, const std::string& _prefix);

    //! returns true iff the _string starts with a given single char prefix (quicker than the string version above)
    bool startsWith            (const std::string& _string, const char _prefix);

    /*
     * Checks if a string ends with a certain ending. Can be used to check for fileendings:
     * if (endsWith( "foobar.txt", ".txt" )) cout << "textfile";
     */
    bool endsWith( const std::string &theString, const std::string &theEnding );
    
    std::vector<std::string> split (const std::string& _string, char _splitChar, bool _skipEmptyStrings = true);

    //! strips a string of all leading and trailing whitespaces (but leaves the ones in between)
    std::string stripOfWhiteSpaces( const std::string &_string );

    //! strips a string of all leading and trailing characters out of a given list (but leaves the ones in between)
    std::string stripOfCharacters( const std::string &_string, const std::string &_charsToStrip );

    //! converts an int to a string but adds leading zeros (e.g. _maxPadding = 2, 1 -> 01)
    std::string intToString( const int _number, const int _maxPadding = 0 );

    //! Convert a primitive type to a string (e.g. string s = toString(1.5f)), also supports some GLM types (but not complete)
    template<class T>
    std::string toString(const T& _t);

    //! Convert a string to a primitive type (e.g. float f = to<float>("1.5"))
    template<class T>
    T to(const std::string& _string, bool* _OK = NULL)
    {
        std::stringstream stream(_string);
        T t;
        stream >> t;
        if(_OK)
            *_OK = !stream.fail();
        return t;
    }

    //! converts a string in the form "(23,42)" to a glm::uvec2. Reverses the functionality of toString<glm::uvec2>( foo ).
    glm::uvec2 toUvec2( const std::string& _string );

    //! converts a string in the form "(23.0,42.0)" to a glm::vec2. Reverses the functionality of toString<glm::vec2>( foo ).
    glm::vec2 toVec2( const std::string& _string );
    glm::vec3 toVec3( const std::string& _string );
    glm::vec4 toVec4( const std::string& _string );

    glm::mat3 toMat3( const std::string& _string );

    /*
     * Returns the lowercase fileending if the filename has a '.' in it, otherwise
     * an empty string.
     */
    std::string getFileEnding( const std::string &_fileName );
}

} // Utils
} // ACGL

#endif // ACGL_BASE_STRINGOPERATIONS_HH
