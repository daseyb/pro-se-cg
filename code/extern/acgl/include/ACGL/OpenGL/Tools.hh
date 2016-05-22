/***********************************************************************
 * Copyright 2011-2014 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_TOOLS_HH
#define ACGL_OPENGL_TOOLS_HH

/*
 * Some OpenGL related helper functions.
 * Error checking and debugging is in Debug.hh.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Debug.hh>


namespace ACGL{
namespace OpenGL{

//! the set framebuffer will replace framebuffer 0
void setDefaultFramebuffer( GLuint _id );

//! can replace glBindFramebuffer( GL_FRAMEBUFFER, 0 ) if the window toolkit needs to use a FB != 0 as the default
void bindDefaultFramebuffer();

//! returns the size in bytes of the common gl types named by there GLenums (GL_BYTE, GL_UNSIGNED_INT etc.)
//! returns 0 for unknown types, so be careful ;-)
GLint getGLTypeSize ( GLenum _type );

//! returns the number of channels (1,2,3 or 4) for a GL format (e.g. GL_RGB, GL_RGB_INTEGER)
//! returns 1 for unknown formats
GLuint getNumberOfChannels( GLenum _format );

//! OpenGL X.Y -> returns Y
uint32_t getOpenGLMinorVersionNumber();

//! OpenGL X.Y -> returns X
uint32_t getOpenGLMajorVersionNumber();

//! returns the combined version number as 10*major + minor for easy comparing
//! OpenGL X.Y -> returns XY
uint32_t getOpenGLVersionNumber();

// query support for specific shader stages:
bool doesSupportGeometryShader();
bool doesSupportTessellationShader();
bool doesSupportComputeShader();

#if (ACGL_OPENGL_VERSION >= 30)
//! Test if a given extension is supported. Note that features which were added to
//! OpenGL itself are not required to get also supported as extensions (sometimes there
//! are minor differences and at least the names of the functions end enums are different).
//!
//! Note that this function is slow and should only be used at initialisation! It does not rely on
//! prefetched extension lists from the function loader (glLoadGen, GLEW etc) which has to be known
//! at compile time but calls the GL functions and thus can detect new extensions.
bool doesSupportExtension( const char *_extensionName );
#endif

#if defined(ACGL_EXTENSION_LOADER_GLLOADGEN)
//! Can load an OpenGL function pointer and is only needed for extensions which are not supported by
//! the extension loader ACGL was compiled with. It is based on the OS specific loader from glLoadGen
//! and thus only works if ACGL was compiled with it. This is also why the implementation is located
//! in gl_select.cc !
void *loadOpenGLFunctionPointer( const char *_name);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Mostly internally used functions:
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////

//! queries some limits of the runtime which are used in ACGL internally. Client apps should not rely on these values, they are subject to changes. Gets called by ACGL::init()
void initRuntimeDependentLimits();
extern GLfloat ACGL_MAX_TEXTURE_MAX_ANISOTROPY;

// Define the shader kinds for older and embedded GL versions so the file extension detection
// can work correctly.
#ifndef GL_GEOMETRY_SHADER
#define GL_GEOMETRY_SHADER 0x8DD9
#endif
#ifndef GL_TESS_CONTROL_SHADER
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_TESS_CONTROL_SHADER 0x8E88
#endif
#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

struct ShaderEndings
{
    const char *ending;
    GLenum      type;
};
const ShaderEndings sShaderEndings[  ] = {
    {"vsh",  GL_VERTEX_SHADER},
    {"vert", GL_VERTEX_SHADER},
    {"tcsh", GL_TESS_CONTROL_SHADER},
    {"tcs",  GL_TESS_CONTROL_SHADER},
    {"tesh", GL_TESS_EVALUATION_SHADER},
    {"tes",  GL_TESS_EVALUATION_SHADER},
    {"gsh",  GL_GEOMETRY_SHADER},
    {"geo",  GL_GEOMETRY_SHADER},
    {"fsh",  GL_FRAGMENT_SHADER},
    {"frag", GL_FRAGMENT_SHADER},
    {"csh",  GL_COMPUTE_SHADER},
    {"cs",   GL_COMPUTE_SHADER}
};

// returns the GL_TESS_CONTROL_SHADER GL_TESS_EVALUATION_SHADER GL_GEOMETRY_SHADER GL_COMPUTE_SHADER
// GL_VERTEX_SHADER GL_FRAGMENT_SHADER or GL_INVALID_VALUE in case the shadertype was not detected
// if _ignoreUnsupportedShaderTypes is true, types unsupported by the current runtime will return
// an GL_INVALID_ENUM also.
GLenum getShaderTypeByFileEnding( const std::string _fileName, bool _ignoreUnsupportedShaderTypes = true );

// looks up the enum and gives a human readable version
const GLubyte* acglShaderTypeString( GLenum _shaderType );

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_TOOLS_HH

