/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

/**
 * Basic checks if an extension is available.
 * Depending on whether GLEW, the internal loader or anything else was
 * used to get the OpenGL function pointers and extensions, the implementation
 * will differ.
 * Only the extensions are listed here that are needed inside of ACGL itself,
 * it is not a complete list! It can also change and checks can get removed
 * if they are not needed inside of ACGL.
 * Applications should perform there own checks depending on the extension
 * loader choosen for the app and not rely on these checks!
 *
 * Each check will return true if the extension is available.
 **/

#ifndef ACGL_OPENGL_EXTENSIONS_HH
#define ACGL_OPENGL_EXTENSIONS_HH

#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

// anisotrophic texture filtering
inline bool ACGL_EXT_texture_filter_anisotrophic() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_EXT_texture_filter_anisotropic != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_EXT_texture_filter_anisotropic;
    #else
        // if needed define the constants so the code will compile, it
        // will never be run anyway as this check will return false!
        #ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
        #define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
        #endif
        #ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
        #endif
        return false;
    #endif
}

// EXT_geometry_shader4 & ARB_geometry_shader4 are identical and may not be available if the context is 3.2+ anyway!
inline bool ACGL_EXT_geometry_shader4() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_EXT_geometry_shader4 != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_EXT_geometry_shader4;
    #else
        return false;
    #endif
}

// EXT_geometry_shader4 & ARB_geometry_shader4 are identical and may not be available if the context is 3.2+ anyway!
inline bool ACGL_ARB_geometry_shader4() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_ARB_geometry_shader4 != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_ARB_geometry_shader4;
    #else
        return false;
    #endif
}

inline bool ACGL_ARB_tessellation_shader() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_ARB_tessellation_shader != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_ARB_tessellation_shader;
    #else
        return false;
    #endif
}

inline bool ACGL_ARB_compute_shader() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_ARB_compute_shader != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_ARB_compute_shader;
    #else
        return false;
    #endif
}

// ARB debug output
inline bool ACGL_ARB_debug_output() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_ARB_debug_output != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_ARB_debug_output;
    #else
        return false;
    #endif
}

// KHR debug (core in OpenGL 4.3) this tests only the extension!
inline bool ACGL_KHR_debug() {
    #ifdef ACGL_EXTENSION_LOADER_GLLOADGEN
        return (ogl_ext_KHR_debug != ogl_LOAD_FAILED);
    #elif ACGL_EXTENSION_LOADER_GLEW
        return GLEW_KHR_debug;
    #else
        return false;
    #endif
}


} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_EXTENSIONS_HH
