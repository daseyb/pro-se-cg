/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/Log.hh>

#include <cstdio> // fopen etc.
#include <string>
#include <cctype>    // tolower
#include <vector>
#include <sstream>
#include <iostream>

using namespace ACGL::Utils;

namespace ACGL{
namespace Utils{

namespace FileHelpers
{
    char *readTextFile( const std::string &fileName )
    {
        FILE *fp;
        char *content = NULL;

        int32_t count = 0;
        if ( fileName != "" ) {
            fp = fopen( fileName.c_str() ,"rt" );
            if ( fp != NULL ) {
                fseek( fp, 0, SEEK_END );
                count = (int32_t) ftell( fp );
                rewind( fp );

                if ( count > 0 ) {
                    content = new char[ count+1 ];
                    count = (int32_t) fread( content, sizeof(char), count, fp );
                    content[ count ] = '\0';
                }
                fclose( fp );
            }
        }
        // if the file was not readable we will return a NULL pointer
        return content;
    }

    bool rawData(const std::string& _filename, char*& _pData, long_t& _size)
    {
        FILE* pFile;
        size_t result;

        pFile = fopen(_filename.c_str(), "rb");
        if(pFile==NULL)
        {
            warning() << "Opening error!" << std::endl;
            return false;
        }

        // obtain file size:
        fseek(pFile, 0, SEEK_END);
        _size = ftell(pFile);
        rewind(pFile);

    #ifdef __ANDROID__
        _pData = new char[_size];
    #else
        try
        {
            // allocate memory to contain the whole file:
            _pData = new char[_size];
        }
        catch(...)
        {
            error() << "Memory error!" << std::endl;
            fclose (pFile);
            return false;
        }
    #endif

        // copy the file into the buffer:
        result = fread(_pData, 1, _size, pFile);
        if ( (long_t)result != _size)
        {
            error() << "Reading error!" << std::endl;
            fclose (pFile);
            return false;
        }

        /* the whole file is now loaded in the memory buffer. */

        // terminate
        fclose (pFile);
        return true;
    }

    #ifndef ACGL_PLATFORM_IOS
    std::string getDeviceDependentPathFor( const std::string &resource )
    {
        return resource;
    }
    
    std::string getDeviceResourcePath() {return "";}
    #endif


    FileModificationTime getFileModificationTime( const std::string &fileName )
    {
        struct stat fileStats;

        stat(fileName.c_str(), &fileStats);
        return fileStats.st_mtime;
    }

    bool fileExists(const std::string &fileName)
    {
        std::ifstream file( fileName.c_str() );
        return file.good();
    }
}

} // Base
} // ACGL

