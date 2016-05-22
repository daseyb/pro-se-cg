/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_UTILS_LOG_HH
#define ACGL_UTILS_LOG_HH

/*
 * Some classes, typedefs and defines to create a simple logging system:
 *
 * Can be used excactly like std::cout
 *
 * message stream:   log()     << "foo " << "bar" << var << std::endl;
 * warning stream:   warning() << "memory low" << std::endl;
 * error stream:     error()   << "shader compile failed: " << getErrorMsg() << std::endl;
 * debug stream:     debug()   << "i = " << i << std::endl;
 *
 * Streams can get muted and unmuted at runtime:
 *  debug().mute();
 *  debug() << "you will never see me!" << std::endl;
 *  debug().unmute();
 *
 * The Application can create own streams and set own prefixes:
 *
 *  log<6>().setPrefix("app specific: ");
 *  log<6>() << "logging" << std::endl;
 *
 * If no prefix was set, the number will be used as a prefix:
 *
 *  log<11>() << "up to 11!" << std::endl;
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Singleton.hh>

#include <string>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __ANDROID__
#include <android/log.h>
#endif


namespace ACGL{
namespace Utils{

/*
 * The stream buffer is internally used in the CoutLikeStream (see below).
 */
class CoutLikeStreamBuffer : public std::basic_streambuf<char, std::char_traits<char> >
{
    typedef std::basic_streambuf<char, std::char_traits<char> > base_type;

public:
    CoutLikeStreamBuffer();
    ~CoutLikeStreamBuffer();

    //! sets the prefix of each line
    void setPrefix( const std::string &_prefix );

    //! sets the filename of the file to mirror everything into, set to "" to disable mirroring
    void setFilename( const std::string &_filename);

private:
    //! gets called when multiple chars should get written
    virtual std::streamsize xsputn(const base_type::char_type* s, std::streamsize n);

    //! gets called when the buffer is full ans a single char gets written
    virtual base_type::int_type overflow(base_type::int_type ch);

    //! gets called for each endl:
    virtual int sync();

    //! used to mirror the output also into a file
    void mirrorToFile( const std::string &_token );

private:
    char  *mBuffer;        // string buffer
    std::string mPrefix;   // prefix used for each new line of this stream
    size_t mBufferSize;    // how many bytes are used in the buffer
    size_t mBufferMaxSize; // size of the buffer

    bool  mNewLineIsAboutToStart;
    bool  mMirrorToFile;
    std::string mFilename;
    
#ifdef __ANDROID__
    android_LogPriority mAndroidPriority;
public:
    void setAndroidPriority( android_LogPriority _priority ) { mAndroidPriority = _priority; }
#endif
};

/*
 * This is the stream itself that behaves like an std::ostream with some custom
 * extensions (like adding the debug level prefix).
 */
template < unsigned int DEBUG_LEVEL >
class CoutLikeStream : public std::ostream, public Base::Singleton<CoutLikeStream<DEBUG_LEVEL> >
{
public:
    CoutLikeStream() : std::ostream( NULL ), mStreamBuffer(NULL) {
        mStreamBuffer = new CoutLikeStreamBuffer();
        switch (DEBUG_LEVEL) {
            case 0: mStreamBuffer->setPrefix("Debug:   ");
            break;
            case 1: mStreamBuffer->setPrefix("Message: ");
            break;
            case 2: mStreamBuffer->setPrefix("Warning: ");
            break;
            case 3: mStreamBuffer->setPrefix("Error:   ");
            break;
            default: {
                mStreamBuffer->setPrefix("> ");
                std::ostringstream streamName;

                streamName << DEBUG_LEVEL << ": ";
                mStreamBuffer->setPrefix( streamName.str() );
            }
        }
#ifdef __ANDROID__
        switch (DEBUG_LEVEL) {
            case 0: mStreamBuffer->setAndroidPriority( ANDROID_LOG_DEBUG); break;
            case 1: mStreamBuffer->setAndroidPriority( ANDROID_LOG_INFO); break;
            case 2: mStreamBuffer->setAndroidPriority( ANDROID_LOG_WARN); break;
            case 3: mStreamBuffer->setAndroidPriority( ANDROID_LOG_ERROR); break;
            default: mStreamBuffer->setAndroidPriority( ANDROID_LOG_UNKNOWN);
        }
#endif

        unmute();
    }

    ~CoutLikeStream() {
        delete mStreamBuffer;
    }

    //! sets the prefix of each line
    void setPrefix( const std::string &_prefix ) {
        if (mStreamBuffer) {
            mStreamBuffer->setPrefix(_prefix);
        }
    }

    //! a filename of a text file to mirror all outputs into
    //! set a filename of "" to disable this
    void setFilename( const std::string &_filename ) {
        if (mStreamBuffer) {
            mStreamBuffer->setFilename(_filename);
        }
    }

    //! disable all output from this stream
    void mute()   { rdbuf( NULL ); }

    //! reenable all output
    void unmute() { rdbuf( mStreamBuffer ); }
private:
    CoutLikeStreamBuffer *mStreamBuffer;
};

/*
 * Defines the stream functions that should be used:
 */
inline CoutLikeStream<0>& debug()   { return (*CoutLikeStream<0>::the()); }
inline CoutLikeStream<1>& message() { return (*CoutLikeStream<1>::the()); }
inline CoutLikeStream<2>& warning() { return (*CoutLikeStream<2>::the()); }
inline CoutLikeStream<3>& error()   { return (*CoutLikeStream<3>::the()); }

/*
 * Generic streams: this way application specific streams can be created:
 */
template < unsigned int N >
inline CoutLikeStream<N>& log() { return (*CoutLikeStream<N>::the()); }

} // Utils
} // ACGL

#endif // ACGL_UTILS_LOG_HH
