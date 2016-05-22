/***********************************************************************
* Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/

#if defined (__unix)||defined (__APPLE__)
#include "ACGL/Utils/MemoryMappedFilePosixImpl.hh"
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

MemoryMappedFilePosixImpl::MemoryMappedFilePosixImpl(
    const char* _fileName,
    ACGL::Utils::MemoryMappedFile::accessMode _accessMode,
    ACGL::Utils::MemoryMappedFile::shareMode _shareMode,
    size_t _length,
    off_t _offset):
    mFileHandle(0),
    mPageOffset(0),
    mpData(nullptr),
    mErrorCode(0)
{
    int pageAccess = 0;
    int pageShare = 0;

    if(_shareMode == ACGL::Utils::MemoryMappedFile::MAPPING_SHARED)
        pageShare = MAP_SHARED;
    else
        pageShare = MAP_PRIVATE;

    if(_accessMode == ACGL::Utils::MemoryMappedFile::READ_ONLY)
    {
        mFileHandle = open(_fileName,O_RDONLY);
        pageAccess = PROT_READ;
    }
    else
    {
        mFileHandle = open(_fileName, O_RDWR);
        pageAccess = PROT_READ | PROT_WRITE;
    }

    if(mFileHandle == 0)
    {
        mErrorCode = 2;
        ACGL::Utils::error()<<"Could not map the File: "<<_fileName<<" to Ram. File was not found"<<std::endl;
        return;
    }

    if(_length <= 0)
    {
        struct stat fileStat;
        stat(_fileName, &fileStat);
        mLength = fileStat.st_size;
    }

    if(_offset > 0)
    {
        mPageOffset = _offset % sysconf(_SC_PAGE_SIZE);
        mLength -= _offset;
    }

    mpData = reinterpret_cast<char*>(mmap(nullptr, mLength, pageAccess, pageShare,
                      mFileHandle, _offset - mPageOffset));
    if(!mpData)
    {
        ACGL::Utils::error()<<"could not map the File: "<<_fileName<<" to Ram. Error when calling mmap (are you shure offset and length are correct?)"<<std::endl;
    }
}
char * MemoryMappedFilePosixImpl::data()
{
    return mpData;
}
const char * MemoryMappedFilePosixImpl::data() const
{
    return mpData;
}
int MemoryMappedFilePosixImpl::errorCode()
{
    return mErrorCode;
}
int MemoryMappedFilePosixImpl::errorCode() const
{
    return mErrorCode;
}
off_t MemoryMappedFilePosixImpl::pageOffset()
{
    return mPageOffset;
}
off_t MemoryMappedFilePosixImpl::pageOffset() const
{
    return mPageOffset;
}
off_t MemoryMappedFilePosixImpl::length()
{
    return mLength;
}
off_t MemoryMappedFilePosixImpl::length() const
{
    return mLength;
}
MemoryMappedFilePosixImpl::~MemoryMappedFilePosixImpl()
{
    if(mpData)
        munmap(mpData,mLength);
    if(mFileHandle)
        close(mFileHandle);
}
#endif
