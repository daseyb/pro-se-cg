/***********************************************************************
* Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/
#pragma once
#if defined (__unix)||defined (__APPLE__)
#include "MemoryMappedFile.hh"
        class MemoryMappedFilePosixImpl
        {

            public:
                MemoryMappedFilePosixImpl(  const  char* _fileName,
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
                ~MemoryMappedFilePosixImpl();

            private:
                int mFileHandle;
                off_t mPageOffset;
                off_t mLength;
                char* mpData;
                int mErrorCode;
        };
#endif
