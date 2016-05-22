/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_SHADER_HH
#define ACGL_OPENGL_OBJECTS_SHADER_HH

/**
 * A Shader ist just one OpenGL shader like a fragment or vertex shader. To use these
 * a ShaderProgram is needed that links together multiple Shaders for the different
 * pipelinestages.
 *
 * So normally you want to work with ShaderPrograms instead of Shaders (switch Programs,
 * set uniforms etc).
 *
 * Custimizing shader parsing is done via custom ShaderParser classes, see Creator/ShaderParser.hh
 */

#include <vector>
#include <string>

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <ACGL/OpenGL/Creator/ShaderParser.hh>

namespace ACGL{
namespace OpenGL{

class Shader
{
    ACGL_NOT_COPYABLE(Shader)

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    Shader(GLenum _type)
    :   mObjectName(0),
        mType(_type)
    {
        mObjectName = glCreateShader(mType);
        if (mObjectName == 0) {
            ACGL::Utils::error() << "couldn't create Shader object! Requested type = " << (unsigned int) mType << std::endl;
        }
    }

    virtual ~Shader(void)
    {
        // "DeleteShader will silently ignore the value zero." - GL Spec
        glDeleteShader(mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_SHADER>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_SHADER>(getObjectName()); }
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGLES_VERSION >= 10))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_SHADER_OBJECT_EXT>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_SHADER_OBJECT_EXT>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint getObjectName(void) const { return mObjectName; }
    inline GLenum getType      (void) const { return mType;       }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    bool setFromFile (SharedShaderParser const& _sp);
    bool setSource   (const std::string &_source, bool _checkForCompileErrors = true);
    bool setSources  (const std::vector<std::string> &_sources, bool _checkForCompileErrors = true );

protected:
    // could get reactivated if needed, might get removed later (thus protected):
    //bool setFromFileNoImportParsing(const std::string& _filename);

    bool compile() const;
    // get a log and a bool whether the log contains an error (or just a warning), not done
    // automatically by compile() but called by all public source setting functions:
    void getCompileLog( std::string &_log, bool &_wasErrorLog ) const;

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLuint              mObjectName;
    GLenum              mType;
};


ACGL_SMARTPOINTER_TYPEDEFS(Shader)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_SHADER_HH
