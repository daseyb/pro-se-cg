/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh>

using namespace std;
using namespace ACGL::Utils;

#include <sstream>

namespace ACGL{
namespace OpenGL{

#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
// desktop with native support
GLDebugAnnotation::GLDebugAnnotation( const char *_message )
{
    GLuint messageID = 0;
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, messageID, -1, _message );
}

GLDebugAnnotation::~GLDebugAnnotation()
{
    glPopDebugGroup();
}

#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (GL_EXT_debug_marker == 1))
// OpenGL ES with extension:
GLDebugAnnotation::GLDebugAnnotation( const char *_message )
{
    glPushGroupMarkerEXT( 0, _message );
}

GLDebugAnnotation::~GLDebugAnnotation()
{
    glPopGroupMarkerEXT();
}
#else
GLDebugAnnotation::GLDebugAnnotation( const char *_message ){}
GLDebugAnnotation::~GLDebugAnnotation(){}
#endif

const char *debugSourceName( GLenum _source )
{
    if (_source == GL_DEBUG_SOURCE_API)             return "API";
    if (_source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)   return "Window System";
    if (_source == GL_DEBUG_SOURCE_SHADER_COMPILER) return "Shader Compiler";
    if (_source == GL_DEBUG_SOURCE_THIRD_PARTY)     return "Third Party";
    if (_source == GL_DEBUG_SOURCE_APPLICATION)     return "Application";
    if (_source == GL_DEBUG_SOURCE_OTHER)           return "Unknown";
    return "Unknown Source";
}

const char *debugTypeName( GLenum _type )
{
    if (_type == GL_DEBUG_TYPE_ERROR)               return "Error";
    if (_type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) return "Deprecated Behavior";
    if (_type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)  return "Undefined Behavior";
    if (_type == GL_DEBUG_TYPE_PORTABILITY)         return "Portability Issue";
    if (_type == GL_DEBUG_TYPE_PERFORMANCE)         return "Performance Issue";
    if (_type == GL_DEBUG_TYPE_MARKER)              return "Marker";
    if (_type == GL_DEBUG_TYPE_OTHER)               return "Issue";
    if (_type == GL_DEBUG_TYPE_POP_GROUP)           return "Debug group pop";
    if (_type == GL_DEBUG_TYPE_PUSH_GROUP)          return "Debug group push";
    return "Unknown Type";
}

const char *debugSeverityName( GLenum _type )
{
    if (_type == GL_DEBUG_SEVERITY_LOW)          return "low";
    if (_type == GL_DEBUG_SEVERITY_MEDIUM)       return "medium";
    if (_type == GL_DEBUG_SEVERITY_HIGH)         return "high";
    if (_type == GL_DEBUG_SEVERITY_NOTIFICATION) return "notification";
    return "Unknown Severity";
}

#if defined(ACGL_OPENGL_DEBUG_CALLBACK_SUPPORT)
void ACGLRegisterDefaultDebugCallback()
{
    if (getOpenGLVersionNumber() >= 43 || ACGL_KHR_debug()) {
        GLint v;
        glGetIntegerv( GL_CONTEXT_FLAGS, &v );
        if ((v & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
            debug() << "context was created with KHR_debug flag, register callback" << endl;
        } else {
            debug() << "context was created WITHOUT KHR_debug flag - registering a debug callback is possible but might not result in getting called even if errors occur!" << endl;
        }

        glDebugMessageCallback( ACGL_KHR_default_debug_callback, NULL );
    }
}
#else
void ACGLRegisterDefaultDebugCallback() {}
#endif

//! place a brakepoint in here to find the source of a problem!
void APIENTRY ACGL_KHR_default_debug_callback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei /*_length*/, const GLchar *_message, void* /* _userParam*/ )
{
    // yes, this could be implemented with less dublicated code, but the
    // point here is to have one code path which gets only called in case of an error
    // so we have a convenient place to put out brakepoint and haing the error message to
    // be printed first...
    if (_type == GL_DEBUG_TYPE_ERROR) {
        error() << "<" << _id << "> severity: " << debugSeverityName(_severity) << " source: " << debugSourceName(_source) << ": " << _message << endl;
    } else if (_type == GL_DEBUG_TYPE_POP_GROUP || _type == GL_DEBUG_TYPE_PUSH_GROUP) {
        // push and pop groups are ignored because they are intended for a debugger
    } else {
        debug() << "<" << _id << "> severity: " << debugSeverityName(_severity) << " source: " << debugSourceName(_source) << ": " << _message << endl;
    }

    // delete all errors to not create another error log for the same problem:
    while ( glGetError() != GL_NO_ERROR ) {}
}


GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber )
{
    GLenum currentError = glGetError();
    GLenum lastError    = currentError;

    // OpenGL does not forbit the implementation to stack up more than one error code
    // so we have to check those in a loop:
    while ( currentError != GL_NO_ERROR ) {
        ACGL::Utils::error() << "GL error in file " << _fileName << ":" << _lineNumber << " - " << acglErrorString( currentError ) << std::endl;

        lastError    = currentError;
        currentError = glGetError();
    }

    return lastError; // returns the last real error (in case there was at least one!)
}


} // OpenGL
} // ACGL
