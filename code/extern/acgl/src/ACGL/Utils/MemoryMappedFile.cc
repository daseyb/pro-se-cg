/***********************************************************************
* Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
* All rights reserved.                                                *
* Distributed under the terms of the MIT License (see LICENSE.TXT).   *
**********************************************************************/

///////////////////////////////////////////////////////////////////////
//
// include os specific headers
//
///////////////////////////////////////////////////////////////////////
#include "ACGL/Utils/MemoryMappedFile.hh"
#ifdef _WIN32
class MemoryMappedFileWinImpl;
#include "ACGL/Utils/MemoryMappedFileWinImpl.hh"
#else
#include  "ACGL/Utils/MemoryMappedFilePosixImpl.hh"
#endif


// call the os specific implementation
// PIMPL idiom
ACGL::Utils::MemoryMappedFile::MemoryMappedFile(
    const char* _fileName, 
    accessMode _accessMode, 
    shareMode _shareMode, 
    size_t _length, 
    off_t _offset) : mpMMFileImpl(NULL)
{
#ifdef _WIN32
    mpMMFileImpl = new MemoryMappedFileWinImpl(_fileName, _accessMode, _shareMode, _length, _offset);
#else
    mpMMFileImpl = new MemoryMappedFilePosixImpl(_fileName, _accessMode, _shareMode, _length, _offset);
#endif
}

char * ACGL::Utils::MemoryMappedFile::data(){ return mpMMFileImpl->data(); }
const char * ACGL::Utils::MemoryMappedFile::data() const{ return mpMMFileImpl->data(); }
int ACGL::Utils::MemoryMappedFile::errorCode() { return mpMMFileImpl->errorCode(); }
int ACGL::Utils::MemoryMappedFile::errorCode() const{ return mpMMFileImpl->errorCode(); }
off_t ACGL::Utils::MemoryMappedFile::pageOffset(){ return mpMMFileImpl->pageOffset(); }
off_t ACGL::Utils::MemoryMappedFile::pageOffset() const{ return mpMMFileImpl->pageOffset(); }
off_t ACGL::Utils::MemoryMappedFile::length(){ return mpMMFileImpl->length(); }
off_t ACGL::Utils::MemoryMappedFile::length() const{ return mpMMFileImpl->length(); }
ACGL::Utils::MemoryMappedFile::~MemoryMappedFile(){ if (mpMMFileImpl) delete mpMMFileImpl; }

