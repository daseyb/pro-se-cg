/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <sstream>
using namespace std;

namespace ACGL{
namespace OpenGL{

GLuint g_ACGL_defaultFramebuffer = 0;

void setDefaultFramebuffer( GLuint _id )
{
    g_ACGL_defaultFramebuffer = _id;
}

void bindDefaultFramebuffer()
{
    glBindFramebuffer( GL_FRAMEBUFFER, g_ACGL_defaultFramebuffer );
}

#if (ACGL_OPENGL_VERSION >= 30)
bool doesSupportExtension( const char *_extensionName )
{
    // move beginning of the extension string by 3 chars if the requested extension name does not include the mandatory "GL_"
    // move by 0 (don't move) otherwise:
    size_t pointerOffset = (strncmp(_extensionName, "GL_", 3) == 0)?0:3;
    GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (GLint i = 0; i < n; i++)
    {
        const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
        extension += pointerOffset;
        if (strcmp(extension, _extensionName) == 0) return true;
    }
    return false;
}
#endif

GLfloat ACGL_MAX_TEXTURE_MAX_ANISOTROPY = -1.0f;
void initRuntimeDependentLimits()
{
    if ( ACGL_EXT_texture_filter_anisotrophic() ) {
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ACGL_MAX_TEXTURE_MAX_ANISOTROPY );
    } else {
        ACGL_MAX_TEXTURE_MAX_ANISOTROPY = 0.0f;
    }
}

//! returns the size in bytes of the common gl types named by there GLenums.
GLint getGLTypeSize ( GLenum _type )
{
    switch(_type)
    {
        case GL_BYTE:           return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
        case GL_SHORT:          return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_INT:            return sizeof(GLint);
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        case GL_FLOAT:          return sizeof(GLfloat);
#ifndef ACGL_OPENGL_ES
        case GL_DOUBLE:         return sizeof(GLdouble);
#endif

        //case GL_INT64_NV:       return sizeof(GLint64);
        //case GL_UNSIGNED_INT64_NV: return sizeof(GLuint64);
        //case GL_HALF_FLOAT:     return sizeof(GLhalf);
        case GL_FIXED:          return 4; // fixed are packed in a 32bit int (see 2.1.2 of the spec.)
    }
    return 0;
}

GLuint getNumberOfChannels( GLenum _format )
{
    // GLES formats:
    if (_format == GL_ALPHA || _format == GL_DEPTH_COMPONENT) return 1;
    if (_format == GL_RGB)  return 3;
    if (_format == GL_RGBA) return 4;
#ifdef ACGL_OPENGL_CORE_PROFILE
    // removed from core:
    if (_format == GL_LUMINANCE)  return 1;
    if (_format == GL_LUMINANCE_ALPHA)  return 2;
#endif

#ifndef ACGL_OPENGL_ES
    // additional desktop formats:
    if ( _format == GL_R8I  || _format == GL_R16I  || _format == GL_R32I
                        || _format == GL_R8UI || _format == GL_R16UI || _format == GL_R32UI
                                              || _format == GL_R16F  || _format == GL_R32F )   return 1;
    if (_format == GL_RED_INTEGER || _format == GL_GREEN_INTEGER || _format == GL_BLUE_INTEGER ) return 1;

    if (_format == GL_RG || _format == GL_RG8I  || _format == GL_RG16I  || _format == GL_RG32I
                         || _format == GL_RG8UI || _format == GL_RG16UI || _format == GL_RG32UI
                                                || _format == GL_RG16F  || _format == GL_RG32F )   return 2;
    if (_format == GL_RG_INTEGER ) return 2;

    if (_format == GL_RGB8I  || _format == GL_RGB16I  || _format == GL_RGB32I
                          || _format == GL_RGB8UI || _format == GL_RGB16UI || _format == GL_RGB32UI
                                                  || _format == GL_RGB16F  || _format == GL_RGB32F )   return 3;
    if (_format == GL_BGR  || _format == GL_RGB_INTEGER  || _format == GL_BGR_INTEGER)  return 3;

    if ( _format == GL_RGBA8I  || _format == GL_RGBA16I  || _format == GL_RGBA32I
                           || _format == GL_RGBA8UI || _format == GL_RGBA16UI || _format == GL_RGBA32UI
                                                    || _format == GL_RGBA16F  || _format == GL_RGBA32F )   return 4;
    if (_format == GL_BGRA || _format == GL_RGBA_INTEGER || _format == GL_BGRA_INTEGER) return 4;
#endif

    return 1; // unknown number of channels, assume 1
}

//
// This is a "private" function that should not be called from outside of this file.
//
// glGetIntegerv(GL_MAJOR_VERSION... and glGetIntegerv(GL_MINOR_VERSION... are great, but
// require OpenGL 3.0 and are not supported on ES 2 :-( so the VERSION string has to get parsed...
//
// OpenGL spec:
// The VERSION ... strings are laid out as follows:
// <version number><space><vendor-specific information>
//
// OpenGL ES spec:
// The VERSION string is laid out as follows:
// "OpenGL ES N.M vendor-specific information"
//
// both specs:
// The version number is either of the form major_number.minor_number or
// major_number.minor_number.release_number, where the numbers all have one or more digits.
//
uint32_t privateGetOpenGLVersion( int _type )
{
    static uint32_t OGLminor   = 0;
    static uint32_t OGLmajor   = 0;
    static uint32_t OGLversion = 0;

    if (OGLversion == 0) {
        // calculate the version numbers once:
        // NOTE: similar to GLEW we assume here, that the minor and major numbers
        //       only have one digit. We also ignore release numbers. This will fail e.g. for OpenGL 10.0
        const GLubyte* versionString;
        versionString = glGetString(GL_VERSION);

        if (versionString == NULL) {
            ACGL::Utils::error() << "cannot get OpenGL version" << std::endl;
            return 0;
        }
        int positionOfFirstDot = 0;
        while ((versionString[positionOfFirstDot] != '\0') && (versionString[positionOfFirstDot] != '.')) ++positionOfFirstDot;

        OGLmajor = versionString[positionOfFirstDot-1] - '0';
        OGLminor = versionString[positionOfFirstDot+1] - '0';

        if (OGLmajor > 9) OGLmajor = 0;
        if (OGLminor > 9) OGLminor = 0;

        OGLversion = OGLmajor*10 + OGLminor;
    }
    switch (_type) {
        case 0: return OGLminor;
        case 1: return OGLmajor;
        default: return OGLversion;
    };
}

uint32_t getOpenGLMinorVersionNumber()
{
    return privateGetOpenGLVersion( 0 );
}

uint32_t getOpenGLMajorVersionNumber()
{
    return privateGetOpenGLVersion( 1 );
}

uint32_t getOpenGLVersionNumber()
{
    return privateGetOpenGLVersion( 2 );
}

// added in ES 3.2 and desktop 3.2
bool doesSupportGeometryShader()
{
#if defined(ACGL_OPENGL_ES)
    #if (ACGL_OPENGL_ES_VERSION < 32)
        return false;
    #else
        return true;
    #endif
#else
    return (ACGL_EXT_geometry_shader4() || ACGL_ARB_geometry_shader4() || (getOpenGLVersionNumber() >= 32));
#endif
}

// added in ES 3.2 and desktop 4.0
bool doesSupportTessellationShader()
{
#if defined(ACGL_OPENGL_ES)
    #if (ACGL_OPENGL_ES_VERSION < 32)
        return false;
    #else
        return true;
    #endif
#else
    return ( ACGL_ARB_tessellation_shader() || (getOpenGLVersionNumber() >= 40));
#endif
}

// added in ES 3.1 and desktop 4.3
bool doesSupportComputeShader()
{
#if defined(ACGL_OPENGL_ES)
    #if (ACGL_OPENGL_ES_VERSION < 31)
        return false;
    #else
        return true;
    #endif
#else
    return ( ACGL_ARB_compute_shader() || (getOpenGLVersionNumber() >= 43));
#endif
}


GLenum getShaderTypeByFileEnding( const std::string _fileName, bool _ignoreUnsupportedShaderTypes )
{
    std::string fileEnding = ACGL::Utils::StringHelpers::getFileEnding( _fileName );
    if( fileEnding.size() == 0 ) return GL_INVALID_ENUM;

    GLenum foundType = GL_INVALID_ENUM;

    // guess the shader type:
    for (unsigned int ending = 0; ending < sizeof(sShaderEndings) / sizeof(ShaderEndings); ++ending)
    {
        if ( fileEnding == sShaderEndings[ending].ending )
        {
            foundType = sShaderEndings[ending].type;
            break;
        }
    }

    if (_ignoreUnsupportedShaderTypes) {
        if (foundType == GL_GEOMETRY_SHADER        && !doesSupportGeometryShader()    ) return GL_INVALID_ENUM;
        if (foundType == GL_TESS_CONTROL_SHADER    && !doesSupportTessellationShader()) return GL_INVALID_ENUM;
        if (foundType == GL_TESS_EVALUATION_SHADER && !doesSupportTessellationShader()) return GL_INVALID_ENUM;
        if (foundType == GL_COMPUTE_SHADER         && !doesSupportComputeShader()     ) return GL_INVALID_ENUM;
    }

    return foundType;
}

const GLubyte* acglShaderTypeString( GLenum _shaderType )
{
    if      (_shaderType == GL_VERTEX_SHADER)          { return (GLubyte*) "vertex shader"; }
    else if (_shaderType == GL_TESS_CONTROL_SHADER)    { return (GLubyte*) "tessellation control shader"; }
    else if (_shaderType == GL_TESS_EVALUATION_SHADER) { return (GLubyte*) "tessellation evaluation shader"; }
    else if (_shaderType == GL_GEOMETRY_SHADER)        { return (GLubyte*) "geometry shader"; }
    else if (_shaderType == GL_FRAGMENT_SHADER)        { return (GLubyte*) "fragment shader"; }
    else if (_shaderType == GL_COMPUTE_SHADER)         { return (GLubyte*) "compute shader"; }
    else {
        return (GLubyte*) "unknown shader type";
    }
}

const GLubyte* acglErrorString( GLenum _errorCode )
{
    // no gluErrorString on iOS, problems on visual studio...
    // Only 3.2+ Core and ES 2.0+ errors belong here:
    if      (_errorCode == GL_INVALID_ENUM)                  { return (GLubyte*) "GL_INVALID_ENUM"; }
    else if (_errorCode == GL_INVALID_VALUE)                 { return (GLubyte*) "GL_INVALID_VALUE"; }
    else if (_errorCode == GL_INVALID_OPERATION)             { return (GLubyte*) "GL_INVALID_OPERATION"; }
    else if (_errorCode == GL_INVALID_FRAMEBUFFER_OPERATION) { return (GLubyte*) "GL_INVALID_FRAMEBUFFER_OPERATION"; }
    else if (_errorCode == GL_OUT_OF_MEMORY)                 { return (GLubyte*) "GL_OUT_OF_MEMORY"; }
    else if (_errorCode == GL_NO_ERROR)                      { return (GLubyte*) "GL_NO_ERROR"; }
    else {
        return (GLubyte*) "unknown error";
    }
}



} // OpenGL
} // ACGL

