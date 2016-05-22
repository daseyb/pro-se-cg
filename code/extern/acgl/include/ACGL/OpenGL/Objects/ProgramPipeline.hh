/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_PROGRAMPIPELINE_HH
#define ACGL_OPENGL_OBJECTS_PROGRAMPIPELINE_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>

#include <ACGL/OpenGL/Objects/ShaderProgram.hh>


#if (ACGL_OPENGL_VERSION >= 41)
namespace ACGL{
namespace OpenGL{

/**
 * OpenGL ProgramPipeline Objects (needs OpenGL 4.1)
 *
 * Multiple ShaderPrograms that are set to be separable can be attached to one ProgramPipeline.
 * Uniforms are still a ShaderProgram (not ProgramPipeline) state, so to update them you either have to
 * select the correct ShaderProgram first (glActiveShaderProgram) or use the setProgramUniform variants.
 *
 * When using multiple ShaderPrograms in one ProgramPipeline the varyings don't just match based on
 * the names any more, look up "Shader Interface Matching", cry, and rethink whether you still want to
 * play around with ProgramPipeline Objects.
 */

class ProgramPipeline {
    ACGL_NOT_COPYABLE(ProgramPipeline)
public:
    ProgramPipeline();
    ~ProgramPipeline(void);

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#ifdef ACGL_OPENGL_DEBUGGER_SUPPORT
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_PROGRAM_PIPELINE>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_PROGRAM_PIPELINE>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint getObjectName(void) const { return mObjectName; }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
    //! Set this ProgramPipeline active, will override active ShaderPrograms!
    inline void bind() const
    {
        glBindProgramPipeline( mObjectName );
    }

    //! unbinds the ProgramPipeline (if there is one bound), after that, ShaderPrograms can be used again directly
    inline static void unBind() { glBindProgramPipeline( 0 ); }

    //! Sets the given ShaderProgram (that should be part of the ProgramPipeline, which should be active) as target
    //! for setUniform() calls. Alternatively use the setProgramUniform calls!
    inline void setActiveShaderProgram( GLuint _shaderProgram ) { glActiveShaderProgram( mObjectName, _shaderProgram ); }

    //! Sets the given ShaderProgram (that should be part of the ProgramPipeline, which should be active) as target
    //! for setUniform() calls. Alternatively use the setProgramUniform calls!
    inline void setActiveShaderProgram( const SharedShaderProgram &_shaderProgram ) { glActiveShaderProgram( mObjectName, _shaderProgram->getObjectName() ); }

    //! _stages is a bitwise OR of GL_TESS_CONTROL_SHADER_BIT, GL_TESS_EVALUATION_SHADER_BIT, GL_VERTEX_SHADER_BIT, GL_GEOMETRY_SHADER_BIT,
    //! and/or GL_FRAGMENT_SHADER_BIT _or_ GL_ALL_SHADER_BITS
    void useProgramStages( GLbitfield _stages, SharedShaderProgram _shaderProgram ) {
        if (_shaderProgram) {
            glUseProgramStages( mObjectName, _stages, _shaderProgram->getObjectName() );
        } else {
            glUseProgramStages( mObjectName, _stages, 0 ); // deactivates that part of the pipeline (defined by _stages)
        }
    }

private:
    // TODO: save 5 SharedPointer to the 5 ShaderPrograms for the individual stages
    GLuint mObjectName;
};

ACGL_SMARTPOINTER_TYPEDEFS(ProgramPipeline)

} // OpenGL
} // ACGL

#endif // OpenGL >= 4.1

#endif // ACGL_OPENGL_OBJECTS_PROGRAMPIPELINE_HH
