/***********************************************************************
 * Copyright 2013 Computer Graphics Group RWTH Aachen University.      *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

/*
 * Pushes the message onto the debug message stack from KHR_debug
 * and pops it when the scope ends (== this object gets destroyed).
 *
 * Will be visible in debuggers.
 */
class GLDebugAnnotation
{
public:
    GLDebugAnnotation( const char *_message );
    ~GLDebugAnnotation();
};


#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
// only for internal use!
// THE_GL_TYPE has to be:
// GL_BUFFER, GL_SHADER, GL_PROGRAM, GL_VERTEX_ARRAY, GL_QUERY, GL_PROGRAM_PIPELINE,
// GL_TRANSFORM_FEEDBACK, GL_SAMPLER, GL_TEXTURE, GL_RENDERBUFFER or GL_FRAMEBUFFER
template <unsigned int THE_GL_TYPE>
void setObjectLabelT( GLuint _objectName, const std::string &_label ) {
    //ACGL::Utils::debug() << "label " << _objectName << " as " << _label << std::endl;
    glObjectLabel( THE_GL_TYPE, _objectName, -1, _label.c_str() );
}

template <unsigned int THE_GL_TYPE>
std::string getObjectLabelT( GLuint _objectName )
{
    GLsizei labelLenght;
    glGetObjectLabel(THE_GL_TYPE, _objectName, 0, &labelLenght, NULL);
    GLchar *tmp = new GLchar[labelLenght+1]; // +1 to have space for the 0-termination

    glGetObjectLabel(THE_GL_TYPE, _objectName, labelLenght+1, NULL, tmp);
    std::string labelName(tmp);
    delete[] tmp;

    return labelName;
}
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (GL_EXT_debug_label == 1))
    // OpenGL ES with extensions:
    template <unsigned int THE_GL_TYPE>
    void setObjectLabelT( GLuint _objectName, const std::string &_label ) {
        glLabelObjectEXT( THE_GL_TYPE, _objectName, 0, _label.c_str() );
    }
    
    template <unsigned int THE_GL_TYPE>
    std::string getObjectLabelT( GLuint _objectName )
    {
        GLsizei labelLenght;
        glGetObjectLabelEXT(THE_GL_TYPE, _objectName, 0, &labelLenght, NULL);
        GLchar *tmp = new GLchar[labelLenght+1]; // +1 to have space for the 0-termination
        
        glGetObjectLabelEXT(THE_GL_TYPE, _objectName, labelLenght+1, NULL, tmp);
        std::string labelName(tmp);
        delete[] tmp;
        
        return labelName;
    }
#endif

//! converts a KHR debug source enum to a human readable string
const char *debugSourceName( GLenum _source );

//! converts a KHR debug type enum to a human readable string
const char *debugTypeName( GLenum _type );

//! converts a KHR debug severity enum to a human readable string
const char *debugSeverityName( GLenum _type );

//! tries to register the default debug callback:
//! applications can register alternative callbacks with glDebugMessageCallback !
void ACGLRegisterDefaultDebugCallback();

// APIENTRY might not be defined if external GL headers are used, e.g. on OpenGL ES. In this
// case KHR_debug callbacks are likely only emulated anyways.
#ifndef APIENTRY
#define APIENTRY
#endif
//! default debug callback
void APIENTRY ACGL_KHR_default_debug_callback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar *_message, void *_userParam);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Error checking, mostly for internal use. Use Debug callbacks if available!
///
///////////////////////////////////////////////////////////////////////////////////////////////////
/// ///////////////////////////////////////////////////////////////////////////////////////////////

// for every OpenGL error enum this will return a human readable version of it
// similar to gluErrorString, but that function is not available on all plattforms
// (read: iOS)
const GLubyte* acglErrorString( GLenum _errorCode );

/*
 * This function can be used outside of the ACGL framework to check always(!) for
 * OpenGL errors. It will print the errors and return the error code of the last one.
 * Each OpenGL command can only throw one error, errors can only stack up if this
 * function or glGetError was not called often enough (and some OpenGL implementations
 * will forget old errors if new ones occur).
 */
#define openGLError() openGLError_( __FILE__, __LINE__ )

/*
 * This function is used internally in ACGL - but not directly. It gets called from all
 * other rare/common/critical error checks. The __FILE__ __LINE macros have to be used in
 * those to get the correct values from the caller file, if we would use the macro
 * above we could get the file/line from this file, which isn't helping.
 */
GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber );

/*
 * NOTE: Explicit error checks are not needed anymore on desktop systems! Use KHR_debug
 *       callback instead (ACGL registers a default callback automatically).
 *
 *
 *
 * Inside of ACGL we distinguish between rare, common and critical errors. Each kind can be
 * switched off which turns the function into nothing after compiler optimization. If an
 * error check is turned off it will always behave as if there was no error, even if there
 * is one...
 *
 * Per default a debug build enables critical and common errors, a release build only critical
 * ones.
 *
 * We have two functions for each kind:
 * openGL____Error()        will print error messages and return the last error (or GL_NO_ERROR)
 * openGL____ErrorOccured() will print error messages and retrun true if there was an error
 *
 * The definition of rare/common/critical is a bit fuzzy:
 *
 * critical: Errors which can occur even in a bug-free app, like out-of-memory errors.
 *           These checks are rare, stuff like glCreate calls get checked here.
 *           Tests for critical errors should not impact the performance of the app measurably.
 *
 * common:
 *           Errors which are quite common while developing. Misuse of the library etc.
 *           Setting uniforms which don't exist or shader compile errors are candidates for
 *           common errors. Places where OpenGL resources are calles by a string etc.
 *
 * rare:     Errors which are so uncommon that we don't even check those ina normal debug build.
 *           Switching these on can impact performance as there can be alot of them. If theres a
 *           OpenGL error somewhere in the code, switch these checks on to find the correct spot.
 *
 * OpenGL error checks that are more complicated than just a glGetError call will be wrapped in the
 * same defines, this can be used in application code as well.
 *
 */

inline GLenum openGLErrorDummy()        { return GL_NO_ERROR; }
inline bool   openGLErrorOccuredDummy() { return false; }

#define openGLCheckError()    ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
#define openGLErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)

#ifdef ACGL_CHECK_CRITICAL_GL_ERRORS
# define openGLCriticalError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLCriticalErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLCriticalError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLCriticalErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif

#ifdef ACGL_CHECK_COMMON_GL_ERRORS
# define openGLCommonError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLCommonErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLCommonError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLCommonErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif

#ifdef ACGL_CHECK_RARE_GL_ERRORS
# define openGLRareError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLRareErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLRareError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLRareErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif

} // OpenGL
} // ACGL




