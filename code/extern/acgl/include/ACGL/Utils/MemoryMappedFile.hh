/***********************************************************************
* Copyright 2011-2014 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/
#pragma once

#include <ACGL/ACGL.hh>

/////////////////////////////////////////////////////////////
//
// forward declaration of platform specific implementation
//
/////////////////////////////////////////////////////////////

#ifdef _WIN32
class MemoryMappedFileWinImpl;
#else
class MemoryMappedFilePosixImpl;
#endif

namespace ACGL{
    namespace Utils{

        /**********************************************************************
        * Provide a platform independant mechanism to map a large file into
        * random access memory. This implementation only supports files, no 
        * images or anonymous mapping is supported.
        *
        * For more information about the concept of FileMapping see:
        * http://msdn.microsoft.com/en-us/library/windows/desktop/aa366556(v=vs.85).aspx
        * or
        * http://en.wikipedia.org/wiki/Memory-mapped_file
        * 
        * For the sake of compatibility, huge pages is not yet used.
        * Also consider the plattform specific limitations (e.G. size of off_t)
        **********************************************************************/
        class MemoryMappedFile
        {
            private:

                #ifdef _WIN32
                    MemoryMappedFileWinImpl* mpMMFileImpl;
                #else
                    MemoryMappedFilePosixImpl* mpMMFileImpl;
                #endif

            public:
                ////////////////////////////////////////////////////////////////
                //
                // enum to determine if the mapped memory is available for 
                // other processes
                //
                ////////////////////////////////////////////////////////////////
                enum shareMode
                {
                    MAPPING_PRIVATE,
                    MAPPING_SHARED
                };

                ////////////////////////////////////////////////////////////////
                //
                // enum to determine how the mapped memory is accessed
                //
                ////////////////////////////////////////////////////////////////
                enum accessMode
                {
                    READ_ONLY,
                    READ_WRITE
                };

                /********************************************************************
                * opens the file specified by _filename and maps it into RAM.
                * The file stays mapped and the handle opened, until the 
                * MemoryMappedFile instance is destroyed. Keep in mind, that other
                * processes may not access the memory region after the file was 
                * unmapped.
                *
                * parameters:
                *
                * char* _filename   -   the file to be mapped
                * 
                * _accessMode       -   determines if the memory has read or write 
                *                       access. readonly mappings cause a sigsegv
                *                       when trying to write them.
                * 
                * _shareMode        -   determine if other processes may access the 
                *                       mapped file the current process owns the 
                *                       mapping.
                *
                * size_t _length    -   the length of the file to be mapped use 0
                *                       to map the entire file. if you want to map
                *                       only a part of the file use 
                *                       _offset + desired length in bytes
                *
                * off_t _offset     -   use 0 if you want to map the entire file.
                *                       otherwise use the byte offset. Keep in mind
                *                       the mapping is page aligned.
                ********************************************************************/
                MemoryMappedFile(   const char* _fileName,
                                    accessMode _accessMode = accessMode::READ_ONLY, 
                                    shareMode _shareMode = shareMode::MAPPING_PRIVATE,
                                    size_t _length = 0, 
                                    off_t _offset = 0);
                
                //pointer to the mapped data
                char * data();
                //const pointer to the mapped data
                const char * data() const;

                // use the error code to determine if mapping was successful
                int errorCode();
                int errorCode() const;

                //the offset to page aligned memory (will be 0 if you map the entire file)
                off_t pageOffset();
                off_t pageOffset() const;

                //length of the file mapping
                off_t length();
                off_t length() const;
                
                //unmaps and closes file handle
                ~MemoryMappedFile();
        };

    }
}
