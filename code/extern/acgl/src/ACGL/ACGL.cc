/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>

namespace ACGL
{

bool init( bool forceDebuggingContext )
{
    #ifdef ACGL_DEBUG
        Utils::debug() << "ACGL was compiled for debug" << std::endl;
    #endif

    //
    // init GLEW or own extension loader, do nothing on ES
    //
#ifdef ACGL_USE_GLEW
#   ifdef ACGL_OPENGL_PROFILE_CORE
        glewExperimental = GL_TRUE;
#   endif
        GLenum errorCode = glewInit();
        if ((errorCode != GLEW_OK) || (openGLErrorOccured())) {
            Utils::error() << "could not init GLEW!" << std::endl;
            #ifdef ACGL_OPENGL_PROFILE_CORE
            Utils::error() << "Make sure your version of GLEW is compatible with core contexts!" << std::endl;
            #endif
            return false;
        }
#elif defined( ACGL_EXTENSION_LOADER_GLLOADGEN )
    int loaded = ogl_LoadFunctionsForDebug( GL_TRUE, forceDebuggingContext );
    if (loaded == ogl_LOAD_FAILED)
    {
        Utils::error() << "could not load OpenGL functions!" << std::endl;
        return false;
    }
#endif // GLEW

    //
    // check OpenGL version
    //
#ifdef ACGL_OPENGL_ES
    Utils::debug() << "OpenGL ES Version: ";
    unsigned int versionToTest = ACGL_OPENGLES_VERSION ;
#else
    Utils::debug() << "OpenGL Version: ";
    unsigned int versionToTest = ACGL_OPENGL_VERSION ;
#endif
    Utils::debug() << OpenGL::getOpenGLMajorVersionNumber() << "." << OpenGL::getOpenGLMinorVersionNumber() << std::endl;
    
    if (OpenGL::getOpenGLVersionNumber() < versionToTest) {
       Utils::error() << "At compile time an OpenGL context of version " << versionToTest
                      << " was requested, but the current context only supports " << OpenGL::getOpenGLVersionNumber() << std::endl;
       return false;
    }

    // gets runtime limits to be used internally:
    OpenGL::initRuntimeDependentLimits();

    // if possible, register debug callback:
    //OpenGL::ACGLRegisterDefaultDebugCallback();


    return true;
}
    
}

