/***********************************************************************
* Copyright 2011-2014 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/

#ifdef _WIN32
#include "ACGL/Utils/MemoryMappedFileWinImpl.hh"

MemoryMappedFileWinImpl::MemoryMappedFileWinImpl(
    const char* _fileName, 
    ACGL::Utils::MemoryMappedFile::accessMode _accessMode, 
    ACGL::Utils::MemoryMappedFile::shareMode _shareMode, 
    size_t _length, 
    off_t _offset) :
    mErrorCode(0),
    mPageOffset(0),
    mFileHandle(nullptr),
    mFileMappingHandle(NULL)
{
    DWORD fileAccess = 0;
    DWORD fileShare = 0;
    DWORD fileSize = 0;
	(void)fileSize;
    DWORD offsetLow = 0;
    DWORD offsetHigh = 0;

    DWORD mapAccess = 0;

    DWORD mapViewAccess = 0;
    DWORD mapSize = 0;

    /////////////////////////////////////////////////////
    //
    // initialize flags
    //
    /////////////////////////////////////////////////////

    if (_shareMode == ACGL::Utils::MemoryMappedFile::MAPPING_SHARED)
    {
        fileShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
                
    }

    
    if (_accessMode == ACGL::Utils::MemoryMappedFile::READ_ONLY)
    {
        //msdn example says if we want to read the file we need to specify share_read
        fileAccess = GENERIC_READ;
        fileShare |= FILE_SHARE_READ;
        mapAccess = PAGE_READONLY;
        mapViewAccess = FILE_MAP_READ;
    }
    else
    {
        fileAccess = GENERIC_READ | GENERIC_WRITE;
        mapAccess = PAGE_READWRITE;
        mapViewAccess = FILE_MAP_READ | FILE_MAP_WRITE;
    }

    //////////////////////
    //
    // open the file
    //
    //////////////////////
    mFileHandle = CreateFileA(_fileName, fileAccess, fileShare,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (mFileHandle == INVALID_HANDLE_VALUE)
    {
        std::cout << _fileName << std::endl;
        mErrorCode = GetLastError();
        return;
    }
    
        //determine the size of the mapped view
       // fileSize = GetFileSize(mFileHandle, NULL);
        mapSize = GetFileSize(mFileHandle, NULL);
        if (_offset > 0)
        {
            offsetLow = _offset;
            offsetHigh = ((_offset >> 16)>>16);            
            SYSTEM_INFO info;
            GetSystemInfo(&info);
            mPageOffset = _offset % info.dwAllocationGranularity;
            mapSize -= (_offset);
        }
        if (_length > 0 )
        {
            mapSize = _length;
        }

        //create the file mapping handle
        mFileMappingHandle = CreateFileMapping(mFileHandle, NULL, mapAccess, 0, 0, NULL);
        if (mFileMappingHandle == NULL)
        {
            mErrorCode = GetLastError();
            return;
        }
        //create the file mapping view
        mpData = reinterpret_cast<PCHAR>
            (MapViewOfFile(mFileMappingHandle, mapViewAccess, offsetHigh, offsetLow, mapSize));
        if (!mpData)
        {
            mErrorCode = GetLastError();
            return;
        }
        mLength = mapSize;
}
char * MemoryMappedFileWinImpl::data()
{ 
    return mpData; 
}
const char * MemoryMappedFileWinImpl::data() const
{ 
    return mpData; 
}
int MemoryMappedFileWinImpl::errorCode() 
{ 
    return mErrorCode; 
}
int MemoryMappedFileWinImpl::errorCode() const
{ 
    return mErrorCode; 
}
off_t MemoryMappedFileWinImpl::pageOffset()
{ 
    return mPageOffset; 
}
off_t MemoryMappedFileWinImpl::pageOffset() const
{ 
    return mPageOffset; 
}
off_t MemoryMappedFileWinImpl::length()
{ 
    return mLength; 
}
off_t MemoryMappedFileWinImpl::length() const
{ 
    return mLength; 
}
MemoryMappedFileWinImpl::~MemoryMappedFileWinImpl()
{
    if (mpData)
        UnmapViewOfFile(mpData);
    if (mFileMappingHandle)
        CloseHandle(mFileMappingHandle);
    if (mFileHandle != INVALID_HANDLE_VALUE)
        CloseHandle(mFileHandle);
}
#endif       
 
