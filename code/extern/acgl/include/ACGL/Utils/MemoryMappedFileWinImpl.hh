/***********************************************************************
* Copyright 2011-2014 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/
#pragma once

///////////////////////////////////////
//
// only compile on windows platform
//
///////////////////////////////////////

#ifdef _WIN32
#include "MemoryMappedFile.hh"
#include <Windows.h>
class MemoryMappedFileWinImpl
{

public:
    /*******************************************************************************
    * Windows implementation only allows path with length of 260 characters or less
    * If you need more, use wchar instead and convert the path as described by msdn
    * http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
    *
    * off_t is in windows a long integer so you can address 64 byte file mappings
    * when creating a file map two dword are used. 
    *******************************************************************************/
    MemoryMappedFileWinImpl(const char* _fileName,
        ACGL::Utils::MemoryMappedFile::accessMode _accessMode,
        ACGL::Utils::MemoryMappedFile::shareMode _shareMode,
        size_t _length = 0,
        off_t _offset = 0);

    char * data();
    const char * data() const;
    int errorCode();
    int errorCode() const;
    off_t pageOffset();
    off_t pageOffset() const;
    off_t length();
    off_t length() const;
    ~MemoryMappedFileWinImpl();

private:
    DWORD mErrorCode;
    char* mpData;
    off_t mPageOffset;
    off_t mLength;
    HANDLE mFileHandle;
    HANDLE mFileMappingHandle;
};
#endif