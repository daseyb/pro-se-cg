/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_FILEHELPERS_HH
#define ACGL_BASE_FILEHELPERS_HH

#include <string>
#include <vector>

// to get file modification times:
#include <time.h>
#ifdef _MSC_VER
# include <sys/utime.h>
#else
# include <utime.h>
#endif
#include <sys/stat.h>


#include <ACGL/ACGL.hh>
#include <ACGL/Utils/StringHelpers.hh>

namespace ACGL{
namespace Utils{

namespace FileHelpers
{
    /*
     * A small collection of helperfunctions to handle files. See StringHelpers for functions to
     * handle filenames.
     */

    /*
     * Reads a file to a char buffer created with new[], the caller has to delete the buffer.
     * Will return a NULL pointer if the file does not exist or there were other errors opening the file.
     */
    char *readTextFile( const std::string &fileName );

    bool rawData(const std::string& _filename, char*& _pData, ACGL::long_t& _size);

    /*
     * On Desktops this will return the input string. But on iOS it will calculate the absolute
     * path to the file which is needed to access it (it's bundle dependent)
     */
    std::string getDeviceDependentPathFor( const std::string &resource );

    // resource path is important on iOS, "" everywhere else
    std::string getDeviceResourcePath();
    
    typedef time_t FileModificationTime;

    /*
     * Will return the modification time of the file.
     */
    FileModificationTime getFileModificationTime( const std::string &fileName ) ;

    /*
     * Checks whether a file with the given filename exists and is readable.
     */
    bool fileExists( const std::string &fileName );
}

} // Base
} // ACGL

#endif // FILEHELPERS_H
