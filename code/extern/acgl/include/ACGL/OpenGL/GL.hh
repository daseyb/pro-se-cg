/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_GL_HH
#define ACGL_OPENGL_GL_HH

#include <ACGL/Base/OSDetection.hh>

/*
 * This simple OpenGL wrapper is used to include OpenGL (and if needed GLEW)
 * on different platforms.
 *
 * While these includes are located in a subdirectory of GL/ on most systems,
 * Apple hides them on different locations.
 *
 *
 * This wrapper can get configured with external defines:
 * ACGL_OPENGL_PROFILE_CORE      : if defined: if possible include/load only core OpenGL functions
 *                                 if not defined: support for CORE and deprecated functions
 *                                 (NOTE: the OpenGL context itself is not created by ACGL!)
 * ACGL_OPENGLES_VERSION_20      : if defined: OpenGL ES 2.0
 * ACGL_OPENGLES_VERSION_30      : if defined: OpenGL ES 3.0
 * ACGL_OPENGL_VERSION_41        : (or other versions): minimal OpenGL version that can be assumed to be present.
 *                                 The app can't run on older contexts and will probably terminate at startup.
 *                                 Set this to a low version and it will run on lost machines
 *                                 Set this to a high version and less run-time checks have to be performed to
 *                                 work around missing features (like querying extensions etc).
 * ACGL_OPENGL_INCLUDE_LATEST_GL : If this is set and GL function loading is done with the internal loader, all
 *                                 functions of the latest GL version are loaded (if available) and the header
 *                                 of the latest version gets included. ACGL however will only assume the version
 *                                 set by ACGL_OPENGL_VERSION_XY to be present.
 *
 * ACGL_USE_GLEW                 : if this is set, GLEW gets used to load the GL functions,
 *                                 otherwise an internal GL loader based on glLoadGen gets used on desktop systems.
 *
 * Note: If nothing is defined, core 3.2 gets defined but all functions are loaded.
 *       If *just* ACGL_OPENGL_INCLUDE_LATEST_GL is defined, the latest GL version gets defined *and* loaded
 *       (full/compatibility profile).
 */

#if (defined(ACGL_PLATFORM_IOS) || defined(ACGL_PLATFORM_ANDROID))
#   define ACGL_OPENGL_ES
#endif

// To compare the OpenGL version number we define a new ACGL_OPENGL_VERSION XY define here
// analog to ACGL_OPENGL_VERSION_XY

// OpenGL ES 3.0 is the default for embedded:
#ifdef ACGL_OPENGL_ES
#  if   defined (ACGL_OPENGLES_VERSION_30)
#       define   ACGL_OPENGLES_VERSION 30
#  elif defined (ACGL_OPENGLES_VERSION_31)
#       define   ACGL_OPENGLES_VERSION 31
#  elif defined (ACGL_OPENGLES_VERSION_32)
#       define   ACGL_OPENGLES_VERSION 32
#  else
#       define ACGL_OPENGLES_VERSION_30
#       define ACGL_OPENGLES_VERSION 30
#  endif
#  define ACGL_OPENGL_VERSION 0
#else
    // Desktop:
    #define ACGL_OPENGLES_VERSION 0
    #if   defined (ACGL_OPENGL_VERSION_21)
    #     define   ACGL_OPENGL_VERSION 21
    #elif defined (ACGL_OPENGL_VERSION_30)
    #     define   ACGL_OPENGL_VERSION 30
    #elif defined (ACGL_OPENGL_VERSION_31)
    #     define   ACGL_OPENGL_VERSION 31
    #elif defined (ACGL_OPENGL_VERSION_32)
    #     define   ACGL_OPENGL_VERSION 32
    #elif defined (ACGL_OPENGL_VERSION_33)
    #     define   ACGL_OPENGL_VERSION 33
    #elif defined (ACGL_OPENGL_VERSION_40)
    #     define   ACGL_OPENGL_VERSION 40
    #elif defined (ACGL_OPENGL_VERSION_41)
    #     define   ACGL_OPENGL_VERSION 41
    #elif defined (ACGL_OPENGL_VERSION_42)
    #     define   ACGL_OPENGL_VERSION 42
    #elif defined (ACGL_OPENGL_VERSION_43)
    #     define   ACGL_OPENGL_VERSION 43
    #elif defined (ACGL_OPENGL_VERSION_44)
    #     define   ACGL_OPENGL_VERSION 44
    #else
	#if defined (ACGL_OPENGL_INCLUDE_LATEST_GL)
        #define ACGL_OPENGL_VERSION_44
        #define ACGL_OPENGL_VERSION 44
	    #define ACGL_OPENGL_PROFILE_FULL
	#else
	    // fallback:
      #ifdef _MSC_VER
        #pragma message("NO ACGL_OPENGL_VERSION_XY SET! Default to 3.2 core (but load all available functions)")
      #else
        #warning "NO ACGL_OPENGL_VERSION_XY SET! Default to 3.2 core (but load all available functions)"
	    #endif
      
      // *we* only assume OpenGL 3.2 core...
	    #define ACGL_OPENGL_VERSION_32
	    #define ACGL_OPENGL_VERSION 32
	    #define ACGL_OPENGL_PROFILE_CORE
	    // ... but include the latest header and try to load all function in case the app needs more:
	    // (only if the internal loader gets used, but GLEW will also try to load everything)
	    #define ACGL_OPENGL_INCLUDE_LATEST_GL
	#endif // latest GL
    #endif // version checks
#endif // OpenGL ES


#if (ACGL_OPENGL_VERSION < 32) && defined(ACGL_OPENGL_PROFILE_CORE)
    #warning "OpenGL prior to 3.2 did not have core or compatibility profiles"
    #undef ACGL_OPENGL_PROFILE_CORE
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Include the right headers with the OpenGL functions and defines:
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if ACGL_OPENGL_VERSION > 32
    // prevents QT from redefining the debug functions
    #define GL_ARB_debug_output
    #define GL_KHR_debug

    // debug label, debug groups:
    #define ACGL_OPENGL_DEBUGGER_SUPPORT

    // debug callbacks:
    #define ACGL_OPENGL_DEBUG_CALLBACK_SUPPORT
#endif

void CHECKGLERROR();

#ifdef ACGL_OPENGL_ES
  // ES does not know 64bit ints but we need them for the Buffer class:
  #include <stdint.h>
  typedef int64_t GLint64;
  
    #if defined (ACGL_PLATFORM_IOS)
        //iOS:
        #if defined (ACGL_OPENGLES_VERSION_20)
            #import <OpenGLES/ES1/gl.h>
            #import <OpenGLES/ES1/glext.h>
            #import <OpenGLES/ES2/gl.h>
            #import <OpenGLES/ES2/glext.h>
        #elif defined (ACGL_OPENGLES_VERSION_30)
            #import <OpenGLES/ES3/gl.h>
            #import <OpenGLES/ES3/glext.h>
            #define ACGL_OPENGL_DEBUGGER_SUPPORT
        #else
            #error "location of ES headers not known"
        #endif
    #elif defined (ACGL_PLATFORM_ANDROID)
        // Android:
        #if defined (ACGL_OPENGLES_VERSION_20)
            #include <GLES2/gl2.h>
            #include <GLES2/gl2ext.h>
        #else
            #include <GLES3/gl3.h>
            #include <GLES3/gl3ext.h>
        #endif
    #else
        #error "UNKNOWN mobile plattform! Don't know what to include!"
    #endif

#else // ACGL_OPENGL_ES
    // desktop:
    #ifndef __glew_h__
        // if glew was already included, don't include anything else
        // not the recommended way to do it but possible
        #if (defined(__APPLE__) || defined(MACOSX)) && defined(ACGL_OPENGL_PROFILE_FULL)
            #if ACGL_OPENGL_VERSION > 21
                #warning "On MacOS X only core profile is supported for OpenGL >= 3.2"
                #undef ACGL_OPENGL_PROFILE_FULL
                #define ACGL_OPENGL_PROFILE_CORE
            #endif
        #endif
        #ifdef ACGL_USE_GLEW
            #define ACGL_EXTENSION_LOADER_GLEW
            #if defined(__APPLE__) || defined(MACOSX)
                #include <OpenGL/glew.h>
            #else
                #include <GL/glew.h>
            #endif
        #else
            // use the internal loader:
            #define ACGL_EXTENSION_LOADER_GLLOADGEN
            #if defined(__gl_h_) || defined(__GL_H__) || defined(__glext_h_) || defined(__GLEXT_H_) || defined(__gl_ATI_h_) || defined(__gl3_h_)
            #error ACGL/GL.hh has to be the first OpenGL related file to include!
            #endif

            //
            // Include the right header which has just what is needed to catch compatibility problems at compiletime.
            // The selection could also be done with some preprocessor magic but it confuses most IDEs.
            //
            #ifdef ACGL_OPENGL_PROFILE_CORE
                #if defined (ACGL_OPENGL_VERSION_32)
                    #include <ACGL/OpenGL/glloaders/gl_core_32.hh>
                #elif defined (ACGL_OPENGL_VERSION_33)
                    #include <ACGL/OpenGL/glloaders/gl_core_33.hh>
                #elif defined (ACGL_OPENGL_VERSION_40)
                    #include <ACGL/OpenGL/glloaders/gl_core_40.hh>
                #elif defined (ACGL_OPENGL_VERSION_41)
                    #include <ACGL/OpenGL/glloaders/gl_core_41.hh>
                #elif defined (ACGL_OPENGL_VERSION_42)
                    #include <ACGL/OpenGL/glloaders/gl_core_42.hh>
                #elif defined (ACGL_OPENGL_VERSION_43)
                    #include <ACGL/OpenGL/glloaders/gl_core_43.hh>
                #elif defined (ACGL_OPENGL_VERSION_44)
                    #include <ACGL/OpenGL/glloaders/gl_core_44.hh>
                #endif
            #else
                // compatibility profile:
                #if defined (ACGL_OPENGL_VERSION_21)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_21.hh>
                #elif defined (ACGL_OPENGL_VERSION_30)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_30.hh>
                #elif defined (ACGL_OPENGL_VERSION_31)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_31.hh>
                #elif defined (ACGL_OPENGL_VERSION_32)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_32.hh>
                #elif defined (ACGL_OPENGL_VERSION_33)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_33.hh>
                #elif defined (ACGL_OPENGL_VERSION_40)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_40.hh>
                #elif defined (ACGL_OPENGL_VERSION_41)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_41.hh>
                #elif defined (ACGL_OPENGL_VERSION_42)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_42.hh>
                #elif defined (ACGL_OPENGL_VERSION_43)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_43.hh>
                #elif defined (ACGL_OPENGL_VERSION_44)
                    #include <ACGL/OpenGL/glloaders/gl_compatibility_44.hh>
                #endif
            #endif // ACGL_OPENGL_PROFILE_CORE
            // prevent other GL headers from getting included and redefine GL:
            #define __gl3_h_
        #endif // ACGL_USE_GLEW
    #else
        #define ACGL_USE_GLEW
        #define ACGL_EXTENSION_LOADER_GLEW
    #endif // __GLEW__
#endif // ACGL_OPENGL_ES

//
// define some known OpenGL feature availability based on the GL / GLES version
//
#if (ACGL_OPENGL_VERSION >= 20)
    // for DDS support, DXT texture compression
    #define ACGL_OPENGL_SUPPORTS_S3TC
#endif

#if (ACGL_OPENGL_VERSION >= 31)
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_BUFFER
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_RECTANGLE
#endif

#if (ACGL_OPENGL_VERSION >= 20)
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_1D
#endif

#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 20))
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_3D
#endif

#if (ACGL_OPENGL_VERSION >= 30)
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_1D_ARRAY
#endif

#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 30))
    #define ACGL_OPENGL_SUPPORTS_TEXTURE_2D_ARRAY
#endif


#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 30))
    #define ACGL_OPENGL_SUPPORTS_VAO
#endif


#ifndef ACGL_EXTENSION_LOADER_GLLOADGEN
//
// Define some constants to compile our helper functions.
// If our own loader was used we know that they are available.
//

#ifndef GL_RGB
#define GL_RGB 0x1907
#endif
#ifndef GL_RGB
#define GL_RGB 0x1908
#endif
#ifndef GL_RGB8
#define GL_RGB8 0x8051
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif
#ifndef GL_SRGB
#define GL_SRGB 0x8C40
#endif
#ifndef GL_SRGB8
#define GL_SRGB8 0x8C41
#endif
#ifndef GL_SRGB_ALPHA
#define GL_SRGB_ALPHA 0x8C42
#endif
#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

#ifndef GL_COMPRESSED_RGB
#define GL_COMPRESSED_RGB 0x84ED
#endif
#ifndef GL_COMPRESSED_RGBA
#define GL_COMPRESSED_RGBA 0x84EE
#endif
#ifndef GL_COMPRESSED_SRGB
#define GL_COMPRESSED_SRGB 0x8C48
#endif
#ifndef GL_COMPRESSED_SRGB_ALPHA
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#endif

// define some KHR_debug constants in case they were not set
// this way our functions which convert the constants to strings
// will at least compile everywhere
#ifndef GL_DEBUG_SEVERITY_HIGH_ARB
    // Extension: ARB_debug_output
    #define GL_DEBUG_SEVERITY_HIGH_ARB       0x9146
    #define GL_DEBUG_SEVERITY_LOW_ARB        0x9148
    #define GL_DEBUG_SEVERITY_MEDIUM_ARB     0x9147
    #define GL_DEBUG_SOURCE_API_ARB          0x8246
    #define GL_DEBUG_SOURCE_APPLICATION_ARB  0x824A
    #define GL_DEBUG_SOURCE_OTHER_ARB        0x824B
    #define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB 0x8248
    #define GL_DEBUG_SOURCE_THIRD_PARTY_ARB  0x8249
    #define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB 0x8247
    #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB 0x824D
    #define GL_DEBUG_TYPE_ERROR_ARB          0x824C
    #define GL_DEBUG_TYPE_OTHER_ARB          0x8251
    #define GL_DEBUG_TYPE_PERFORMANCE_ARB    0x8250
    #define GL_DEBUG_TYPE_PORTABILITY_ARB    0x824F
    #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB 0x824E
#endif

#ifndef GL_DEBUG_SEVERITY_HIGH
    // Extension: KHR_debug
    #define GL_DEBUG_SEVERITY_HIGH           0x9146
    #define GL_DEBUG_SEVERITY_LOW            0x9148
    #define GL_DEBUG_SEVERITY_MEDIUM         0x9147
    #define GL_DEBUG_SEVERITY_NOTIFICATION   0x826B
    #define GL_DEBUG_SOURCE_API              0x8246
    #define GL_DEBUG_SOURCE_APPLICATION      0x824A
    #define GL_DEBUG_SOURCE_OTHER            0x824B
    #define GL_DEBUG_SOURCE_SHADER_COMPILER  0x8248
    #define GL_DEBUG_SOURCE_THIRD_PARTY      0x8249
    #define GL_DEBUG_SOURCE_WINDOW_SYSTEM    0x8247
    #define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
    #define GL_DEBUG_TYPE_ERROR              0x824C
    #define GL_DEBUG_TYPE_MARKER             0x8268
    #define GL_DEBUG_TYPE_OTHER              0x8251
    #define GL_DEBUG_TYPE_PERFORMANCE        0x8250
    #define GL_DEBUG_TYPE_POP_GROUP          0x826A
    #define GL_DEBUG_TYPE_PORTABILITY        0x824F
    #define GL_DEBUG_TYPE_PUSH_GROUP         0x8269
    #define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#endif

#endif // ACGL_EXTENSION_LOADER_GLLOADGEN

#endif // ACGL_OPENGL_GL_HH
