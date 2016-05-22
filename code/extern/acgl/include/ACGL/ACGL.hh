/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_ACGL_HH
#define ACGL_ACGL_HH

#include <ACGL/Base/OSDetection.hh>

/*!
 * Include this in all ACGL (header)-files. It will include stuff that is used in
 * almost every file:
   * the used datatypes
     * including shared/weak pointers!
   * macros and defines used at compile-time
   * logging
 *
 * Also the librarys init function is defined here.
 */

/*!
 * ACGL needs shared and weak pointers that behave like the C++11 smartpointers.
 * The C++11, TR1 and boost variants should all be fine, you can also plug-in
 * your own compatible pointers here.
 *
 * Place them in the ptr:: namespace with aliases.
 *
 * A detection for TR1 is missing, it is assumed that a pre-C++11 compiler has TR1.
 * In case of porting this to a non-C++11/non-TR1 compiler add a check and e.g.
 * add the boost shared and smart pointers:
 *
 * #include <boost...>
 * namespace ptr = boost::tr1;
 *
 * Or roll your own pointers (in this case, add a compileflag and default to the
 * std pointers!):
 *
 * # include <myOwnAwsomeSharedPointer.hh>
 * namespace ptr = ACGL::Base;
 *
 */

// defines a one-macro version number for easy compare functions: e.g. gcc 4.6.2 -> 406020
// see http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers for macros to detect compilers
#if defined(__GNUC__)
# if defined(__GNUC_PATCHLEVEL__)
#  define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
# else
#  define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
# endif
#else
#  define __GNUC_VERSION__ 0
#endif

// defines a one-macro version number for easy compare functions: e.g. gcc 4.6.2 -> 406020
// see http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers for macros to detect compilers
#if defined(__GNUC__)
# if defined(__GNUC_PATCHLEVEL__)
#  define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
# else
#  define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
# endif
#else
#  define __GNUC_VERSION__ 0
#endif

#if ((__cplusplus >= 201103L) || (__STDC_VERSION__ >= 201112L) || defined(__GXX_EXPERIMENTAL_CXX0X__))
   // last C++11 draft or final C++11 standart or C++11 via -std=c++0x on gcc:
#  include <memory>
   namespace ptr = std;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 1
#  define CORRECT_PTR_INCLUDES_FOUND
#endif

#if (defined (__clang__))
// newer llvms on MacOS need this version:
#  include <memory>
namespace ptr = std;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 1
#  define CORRECT_PTR_INCLUDES_FOUND
#endif

#if (!defined(CORRECT_PTR_INCLUDES_FOUND) && ((__GNUC_VERSION__ >= 40400) ))
   // gcc 4.4 or newer without -std=c++0x or clang++
#  include <tr1/memory>
   namespace ptr = std::tr1;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 0
#  define CORRECT_PTR_INCLUDES_FOUND
#endif


#if (!defined(CORRECT_PTR_INCLUDES_FOUND) && (_MSC_VER >= 1600))
   // VStudio 2010 supports some C++11 features
#  include <memory>
   namespace ptr = std;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 1
#  define CORRECT_PTR_INCLUDES_FOUND
#endif
   
   
#if (!defined(CORRECT_PTR_INCLUDES_FOUND) && (_MSC_VER >= 1500))
   // VStudio 2008 supports some C++11 features
#  include <memory>
   namespace ptr = std::tr1;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 0
#  define CORRECT_PTR_INCLUDES_FOUND
#endif


#if (!defined(CORRECT_PTR_INCLUDES_FOUND) && defined(__INTEL_COMPILER))
   // intel icpc
#  include <memory>
   namespace ptr = std;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 0
#  define CORRECT_PTR_INCLUDES_FOUND
#endif


#if (!defined CORRECT_PTR_INCLUDES_FOUND)
   // guessing is needed
#  warning "can't detect C++ version or shared pointer variant supported by this compiler -> guessing"
   // hope for TR1 equivalents
#  include <tr1/memory>
   namespace ptr = std::tr1;
#  define ACGL_UNIQUE_POINTER_SUPPORTED 0
#endif



#include <ACGL/Base/CompileTimeSettings.hh>
#include <ACGL/Base/Macros.hh>
#include <ACGL/Types.hh>
#include <ACGL/Utils/Log.hh>

namespace ACGL
{

/*
 * This should get called as soon as a valid OpenGL context exists,
 * it will init glew (if used) or the internal GL function loader.
 * Call this before calling any OpenGL functions or OpenGL related
 * ACGL stuff.
 *
 * Returns false if a critical error occured, in that case the ACGL behavior is
 * not defined.
 *
 * parameter forDebugging: if true, register a debug callback for OpenGL and simulate
 * a debug context (slow) in case the application is not running in a native debug
 * context.
 */
bool init( bool forceDebuggingContext = true );
    
}

#endif // ACGL_ACGL_HH

