/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/Math/Math.hh>
#include <algorithm> // transform
#include <iomanip> // setfill

namespace ACGL{
namespace Utils{

namespace StringHelpers
{
    bool splitFileExtension(const std::string& _full, std::string& _file, std::string& _extension)
    {
        size_t found;
        found = _full.find_last_of(".");
        if(found == std::string::npos)
            return false;
        _file = _full.substr(0,found);
        _extension = _full.substr(found+1);
        return true;
    }

    bool splitLastFileOrFolder(const std::string& _full, std::string& _path, std::string& _fileOrFolder)
    {
        size_t found;
        found = _full.find_last_of("/");
        if(found == std::string::npos)
            return false;
        _path = _full.substr(0,found);
        _fileOrFolder = _full.substr(found+1);
        if(_path.size() > 2 && _path[0] == '.' && _path[1] == '/') {
            _path = _path.substr(2, _path.size()-2);
        }
        return true;
    }

    bool startsWith(const std::string& _string, const std::string& _prefix)
    {
        std::size_t prefixLen = _prefix.length();

        if(prefixLen > _string.length())
            return false;

        for(std::size_t i = 0; i < prefixLen; i++)
            if(_prefix[i] != _string[i])
                return false;
        return true;

        // a bit slower in some cases:
        //return (std::mismatch(_prefix.begin(), _prefix.end(), _string.begin()).first == _prefix.end());
    }

    bool startsWith(const std::string& _string, const char _prefix)
    {
        return ((_string.length() > 0) && (_string[0] == _prefix));
    }

    bool endsWith( const std::string &theString, const std::string &theEnding )
    {
        int32_t stringLenght = (int32_t) theString.size();
        int32_t endingLenght = (int32_t) theEnding.size();

        if ( stringLenght < endingLenght ) return false;

        std::string end = theString.substr( stringLenght - endingLenght, endingLenght );

        if ( end == theEnding ) return true;

        return false;
    }

    std::vector<std::string> split(const std::string& _string, char _splitChar, bool _skipEmptyStrings)
    {
        std::vector<std::string> stringList;
        std::stringstream ss(_string);
        std::string item;
        while(std::getline(ss, item, _splitChar))
        {
            //ignore empty elements if skip is defined
            if(_skipEmptyStrings ? item == "" : false)
                continue;

            stringList.push_back(item);
        }

        return stringList;
    }

    std::string stripOfCharacters( const std::string &_string, const std::string &_charsToStrip )
    {
        size_t posOfLastNonStripChar = _string.find_last_not_of( _charsToStrip );
        if ( posOfLastNonStripChar == std::string::npos ) {
            return ""; // the string was empty or had only _charsToStrip!
        }

        size_t posOfFirstNonStripChar = _string.find_first_not_of( _charsToStrip );
        // can't be npos

        return _string.substr( posOfFirstNonStripChar, posOfLastNonStripChar-posOfFirstNonStripChar+1 );
    }

    std::string stripOfWhiteSpaces( const std::string &_string )
    {
        std::string whitespaces(" \t\f\v\n\r");
        return stripOfCharacters( _string, whitespaces );
    }

    std::string intToString( const int _number, const int _maxPadding )
    {
        std::ostringstream stream;
        if (_maxPadding > 0) {
            stream << std::setfill('0') << std::setw(_maxPadding) << _number;
        } else {
            stream << _number;
        }
        return stream.str();
    }

    template<class T>
    std::string toString(const T& _t)
    {
        std::ostringstream stream;
        stream << _t;
        return stream.str();
    }

    template std::string toString<bool>(const bool& _t);
    template std::string toString<float>(const float& _t);
    template std::string toString<double>(const double& _t);
    template std::string toString<byte_t>(const byte_t& _t);
    template std::string toString<ubyte_t>(const ubyte_t& _t);
    template std::string toString<short_t>(const short_t& _t);
    template std::string toString<ushort_t>(const ushort_t& _t);
    template std::string toString<int_t>(const int_t& _t);
    template std::string toString<uint_t>(const uint_t& _t);
    template std::string toString<long_t>(const long_t& _t);
    template std::string toString<ulong_t>(const ulong_t& _t);

    // float vectors:
    template<> std::string toString(const glm::vec2& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::vec3& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << "," << _t.z << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::vec4& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << "," << _t.z << "," << _t.w << ")";
        return stream.str();
    }

    // unsigned int vectors:
    template<> std::string toString(const glm::uvec2& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::uvec3& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << "," << _t.z << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::uvec4& _t)
    {
        std::ostringstream stream;
        stream << "(" << _t.x << "," << _t.y << "," << _t.z << "," << _t.w << ")";
        return stream.str();
    }

    // square matrices:
    template<> std::string toString(const glm::mat2& _t)
    {
        std::ostringstream stream;
        stream << "(" << toString<glm::vec2>(_t[0]) << "," << toString<glm::vec2>(_t[1]) << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::mat3& _t)
    {
        std::ostringstream stream;
        stream << "(" << toString<glm::vec3>(_t[0]) << "," << toString<glm::vec3>(_t[1]) << "," << toString<glm::vec3>(_t[2]) << ")";
        return stream.str();
    }

    template<> std::string toString(const glm::mat4& _t)
    {
        std::ostringstream stream;
        stream << "(" << toString<glm::vec4>(_t[0]) << "," << toString<glm::vec4>(_t[1]) << "," << toString<glm::vec4>(_t[2]) << "," << toString<glm::vec4>(_t[3]) << ")";
        return stream.str();
    }


    // removes the brackets, removes whitespaces and splits at the colons:
    std::vector< std::string > glmStringParserHelper( const std::string& _string )
    {
        std::vector< std::string > token = split( _string, ',' ); // split at ','
        for (size_t i = 0; i < token.size(); i++) {
            token[i] = stripOfCharacters( token[i], " \t\f\v\n\r()" ); // strip of spaces and characters in one go
        }
        return token;
    }

    // parsing of glm types:
    glm::uvec2 toUvec2( const std::string& _string )
    {
        std::vector< std::string > token = glmStringParserHelper( _string );
        if (token.size() == 2) {
            unsigned int x = to<unsigned int>( token[0] );
            unsigned int y = to<unsigned int>( token[1] );

            return glm::uvec2( x,y );
        }
        ACGL::Utils::error() << "could not parse uvec2 " << _string << std::endl;
        return glm::uvec2(0);
    }

    glm::vec2 toVec2( const std::string& _string )
    {
        std::vector< std::string > token = glmStringParserHelper( _string );
        if (token.size() == 2) {
            float x = to<float>( token[0] );
            float y = to<float>( token[1] );

            return glm::vec2( x,y );
        }
        ACGL::Utils::error() << "could not parse vec2 " << _string << std::endl;
        return glm::vec2(0);
    }

    glm::vec3 toVec3( const std::string& _string )
    {
        std::vector< std::string > token = glmStringParserHelper( _string );
        if (token.size() == 3) {
            float x = to<float>( token[0] );
            float y = to<float>( token[1] );
            float z = to<float>( token[2] );

            return glm::vec3( x,y,z );
        }
        ACGL::Utils::error() << "could not parse vec3 " << _string << std::endl;
        return glm::vec3(0);
    }

    glm::vec4 toVec4( const std::string& _string )
    {
        std::vector< std::string > token = glmStringParserHelper( _string );
        if (token.size() == 4) {
            float x = to<float>( token[0] );
            float y = to<float>( token[1] );
            float z = to<float>( token[2] );
            float w = to<float>( token[3] );

            return glm::vec4( x,y,z,w );
        }
        ACGL::Utils::error() << "could not parse vec4 " << _string << std::endl;
        return glm::vec4(0);
    }

    glm::mat3 toMat3( const std::string& _string )
    {
        std::vector< std::string > token = glmStringParserHelper( _string );
        if (token.size() == 9) {
            glm::vec3 a,b,c;
            {
            float x = to<float>( token[0] );
            float y = to<float>( token[1] );
            float z = to<float>( token[2] );
            a = glm::vec3( x,y,z );
            }
            {
            float x = to<float>( token[3] );
            float y = to<float>( token[4] );
            float z = to<float>( token[5] );
            b = glm::vec3( x,y,z );
            }
            {
            float x = to<float>( token[6] );
            float y = to<float>( token[7] );
            float z = to<float>( token[8] );
            c = glm::vec3( x,y,z );
            }

            return glm::mat3( a,b,c );
        }
        ACGL::Utils::error() << "could not parse mat3 " << _string << std::endl;
        return glm::mat3(0);
    }

    std::string getFileEnding( const std::string &_fileName )
    {
        size_t lastDot = _fileName.rfind('.');
        if (lastDot == std::string::npos) {
            return "";
        }
        std::string ending = _fileName.substr( lastDot+1, _fileName.size() );
        std::transform(ending.begin(), ending.end(), ending.begin(), ::tolower);

        return ending;
    }
}

} // Utils
} // ACGL
