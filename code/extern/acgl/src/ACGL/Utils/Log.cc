/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Utils/Log.hh>
#include <ACGL/Math/Math.hh>

#include <cstdio>
#include <iostream>
#include <fstream>
using namespace ACGL::Utils;

CoutLikeStreamBuffer::CoutLikeStreamBuffer() : base_type() {
    mBufferSize    = 0;
    mBufferMaxSize = 256;
    mBuffer        = new char[ mBufferMaxSize+1 ];
    mNewLineIsAboutToStart = true;
    mMirrorToFile  = false;
}

CoutLikeStreamBuffer::~CoutLikeStreamBuffer() {
    if ( mBufferSize > 0 ) {
        overflow( base_type::traits_type::eof() );
        sync();
    }
    delete[] mBuffer;
}

void CoutLikeStreamBuffer::setPrefix( const std::string &_prefix ) {
    mPrefix = _prefix;
}

void CoutLikeStreamBuffer::setFilename( const std::string &_filename ) {
    mFilename = _filename;
    if (_filename == "") {
        mMirrorToFile = false;
    } else {
        mMirrorToFile = true;
    }
}

void CoutLikeStreamBuffer::mirrorToFile( const std::string &_token ) {
    if (mMirrorToFile == false) {
        return;
    }

    // opening and closing the file each time is slow but we don't
    // loose much in case the app crashes and debugging is the main
    // usecase for this function!
    std::ofstream file;
    file.open( mFilename.c_str(), std::ios::app );
    file << _token;
    file.close();
}

std::streamsize CoutLikeStreamBuffer::xsputn(const base_type::char_type* s, std::streamsize n) {
    size_t charsWritten = 0;

    while (charsWritten < (size_t)n) {
        int maxToCopy = (int) std::min( (size_t) n-charsWritten, mBufferMaxSize-mBufferSize );

        memcpy( mBuffer+mBufferSize, s+charsWritten, maxToCopy );
        charsWritten += maxToCopy;
        mBufferSize  += maxToCopy;

        if (charsWritten < (size_t)n) {
            overflow( (base_type::int_type) s[charsWritten] );
            charsWritten++;
        }
    };

    return n;
}

CoutLikeStreamBuffer::base_type::int_type CoutLikeStreamBuffer::overflow(base_type::int_type ch) {
    // print buffer
    if ((mBufferSize >= mBufferMaxSize) || ( base_type::traits_type::eq_int_type(ch, base_type::traits_type::eof()))) {

        if (mNewLineIsAboutToStart) {
#ifndef __ANDROID__
            std::cout << mPrefix;
            mirrorToFile( mPrefix );
#endif
            mNewLineIsAboutToStart = false;
        }
        if (mBufferSize > 0) {
            mBuffer[mBufferSize] = (char) 0;
#ifdef __ANDROID__
	    __android_log_print( mAndroidPriority, mPrefix.c_str(), "%s", mBuffer );
#else
            std::cout << mBuffer;
            mirrorToFile( mBuffer );
#endif
            mBufferSize = 0;
        }
    }

    if (!( base_type::traits_type::eq_int_type(ch, base_type::traits_type::eof()))) {
        mBuffer[ mBufferSize++ ] = (char) ch;
    }
    return base_type::traits_type::not_eof( ch );
}

// for each endl:
int CoutLikeStreamBuffer::sync() {
    overflow( base_type::traits_type::eof() );
    std::cout.flush();
    mNewLineIsAboutToStart = true;
    return base_type::sync();
}



