/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/
#include <ACGL/OpenGL/GL.hh>

#include <iostream>
using namespace std;

// don't do anything if an other extension loader (e.g. GLEW) should get used:
#ifdef ACGL_EXTENSION_LOADER_GLLOADGEN

#if (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

#ifdef ACGL_OPENGL_PROFILE_CORE
    #if defined (ACGL_OPENGL_VERSION_32)
        #include "gl_core_32.inc"
    #elif defined (ACGL_OPENGL_VERSION_33)
        #include "gl_core_33.inc"
    #elif defined (ACGL_OPENGL_VERSION_40)
        #include "gl_core_40.inc"
    #elif defined (ACGL_OPENGL_VERSION_41)
        #include "gl_core_41.inc"
    #elif defined (ACGL_OPENGL_VERSION_42)
        #include "gl_core_42.inc"
    #elif defined (ACGL_OPENGL_VERSION_43)
        #include "gl_core_43.inc"
    #elif defined (ACGL_OPENGL_VERSION_44)
        #include "gl_core_44.inc"
    #else
        #error "unsupported core GL version requested"
    #endif
#else
    // compatibility profile:
    #if defined (ACGL_OPENGL_VERSION_21)
        #include "gl_compatibility_21.inc"
    #elif defined (ACGL_OPENGL_VERSION_30)
        #include "gl_compatibility_30.inc"
    #elif defined (ACGL_OPENGL_VERSION_31)
        #include "gl_compatibility_31.inc"
    #elif defined (ACGL_OPENGL_VERSION_32)
        #include "gl_compatibility_32.inc"
    #elif defined (ACGL_OPENGL_VERSION_33)
        #include "gl_compatibility_33.inc"
    #elif defined (ACGL_OPENGL_VERSION_40)
        #include "gl_compatibility_40.inc"
    #elif defined (ACGL_OPENGL_VERSION_41)
        #include "gl_compatibility_41.inc"
    #elif defined (ACGL_OPENGL_VERSION_42)
        #include "gl_compatibility_42.inc"
    #elif defined (ACGL_OPENGL_VERSION_43)
        #include "gl_compatibility_43.inc"
    #elif defined (ACGL_OPENGL_VERSION_44)
        #include "gl_compatibility_44.inc"
    #else
        #error "unsupported GL version requested"
    #endif
#endif // ACGL_OPENGL_PROFILE_CORE

#if (__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace ACGL{
namespace OpenGL{

void *loadOpenGLFunctionPointer( const char *_name) {
    // IntGetProcAddress is a define which was defined in one of the *.inc files from glLoadGen
    return (void*) IntGetProcAddress( _name );
}

} // namespace
} // namespace

#endif // ACGL_EXTENSION_LOADER_GLLOADGEN
