/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_QUERY_HH
#define ACGL_OPENGL_OBJECTS_QUERY_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>

// only exclude ES 2 for now:
#if ((ACGL_OPENGLES_VERSION > 20) || (ACGL_OPENGL_VERSION > 20))

namespace ACGL{
namespace OpenGL{

/**
 * A generic OpenGL asynchronous query, target types can be:
 * SAMPLES_PASSED
 * ANY_SAMPLES_PASSED
 * PRIMITIVES_GENERATED
 * TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
 * TIME_ELAPSED
 *
 * See specialized queries below.
 *
 * Note: * Indexed queries are not jet supported.
 *       * Only one query per type is alowed to be active at any time.
 *       * Before the result can get read out, the query must end() !
 */
class AsynchronousQuery {
public:
    AsynchronousQuery( GLenum _defaultTarget )
        : mTarget(_defaultTarget)
    {
        glGenQueries( 1, &mObjectName );
    }

    virtual ~AsynchronousQuery() {
        glDeleteQueries( 1, &mObjectName );
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_QUERY>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_QUERY>(getObjectName()); }
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGLES_VERSION >= 10))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_QUERY_OBJECT_EXT>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_QUERY_OBJECT_EXT>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    //! start the query, only one query per type is allowed to be active at any time.
    void begin(void) {
        glBeginQuery( mTarget, mObjectName );
    }

    //! end the query
    void end(void) {
        glEndQuery( mTarget );
    }

    //! returns true if the result of the query is available, if not, trying to get the result will stall the CPU
    GLboolean isResultAvailable(void) {
#if (ACGL_OPENGLES_VERSION >= 30)
        GLuint resultAvailable;
        glGetQueryObjectuiv(mObjectName, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
#else
        GLint resultAvailable;
        glGetQueryObjectiv(mObjectName, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
#endif
        return (GLboolean) resultAvailable;
    }

    //! get the query result, what it is depents on the query target
    GLuint getResult(void) {
        GLuint queryResult;
        glGetQueryObjectuiv( mObjectName, GL_QUERY_RESULT, &queryResult );
        return queryResult;
    }


    //! get the query result in 64 bit, what it is depents on the query target
    GLuint64 getResult64(void) {
#if (ACGL_OPENGL_VERSION >= 33)
        GLuint64 queryResult;
        glGetQueryObjectui64v( mObjectName, GL_QUERY_RESULT, &queryResult );
        return queryResult;
#else
        return (GLuint64) getResult(); // default to 32 bit version on pre GL 3.3 systems
#endif
    }

    //! returns the raw object name to be used directly in OpenGL functions
    inline GLuint getObjectName(void) const { return mObjectName; }

protected:
    GLuint mObjectName;
    GLenum mTarget;
};
ACGL_SMARTPOINTER_TYPEDEFS(AsynchronousQuery)

/**
 * Occlusion queries count the fragments that pass the z-test.
 *
 * There are two variants:
 * GL_SAMPLES_PASSED     - will count the fragments
 * GL_ANY_SAMPLES_PASSED - will just tell whether fragments have passed the z-test, not how many (0 or any number)
 */
    
// On ES only ANY_SAMPLES is supported, so use this as a substitute:
#if ( !defined(GL_SAMPLES_PASSED) && (ACGL_OPENGLES_VERSION >= 30))
#define GL_SAMPLES_PASSED GL_ANY_SAMPLES_PASSED
#endif
    
class OcclusionQuery : public AsynchronousQuery {
public:
    OcclusionQuery() : AsynchronousQuery( GL_SAMPLES_PASSED ) {}
    OcclusionQuery( GLenum _queryType ) : AsynchronousQuery( _queryType ) {
        setType( _queryType );
    }

    //! _queryType has to be GL_SAMPLES_PASSED or GL_ANY_SAMPLES_PASSED
    void setType( GLenum _queryType ) {
#if (ACGL_OPENGL_VERSION <= 32)
        #define ACGL_ANY_SAMPLES_PASSED  0x8C2F
        if (_queryType == ACGL_ANY_SAMPLES_PASSED) _queryType = GL_SAMPLES_PASSED; // GL_ANY_SAMPLES_PASSED is OpenGL 3.3 or later! But GL_SAMPLES_PASSED is a good substitute
#endif
        if (_queryType != GL_SAMPLES_PASSED) {
            Utils::error() << "OcclusionQuery type " << _queryType << " not supported" << std::endl;
            _queryType = GL_SAMPLES_PASSED;
        }
        mTarget = _queryType;
    }

    //! get the actual number of fragments, unless the type is GL_ANY_SAMPLES_PASSED, than it only tells 0 or any value
    GLuint samplesPassed(void) {
        return getResult();
    }
};
ACGL_SMARTPOINTER_TYPEDEFS(OcclusionQuery)


#if (ACGL_OPENGL_VERSION >= 33)
/**
 * TimerQueries can get the GPU timestamp and measure GPU execution speed.
 *
 * Only available since OpenGL 3.3 or GL_ARB_timer_query (on OpenGL 3.2)
 */
class TimerQuery : public AsynchronousQuery {
public:
    TimerQuery() : AsynchronousQuery( GL_TIME_ELAPSED ) {}

    //! Mark the moment in the pipeline of which the time should get queried.
    void saveTimestamp(void) {
        glQueryCounter( mObjectName, GL_TIMESTAMP );
    }

    //! Get the current GPU timestamp.
    GLint64 getCurrentTimestamp(void) {
        GLint64 time;
        glGetInteger64v( GL_TIMESTAMP, &time );
        return time;
    }

    //! Get the timestamp saved by 'saveTimestamp'.
    GLuint64 getSavedTimestamp(void) {
        return getResult64();
    }
};
ACGL_SMARTPOINTER_TYPEDEFS(TimerQuery)
#endif // OpenGL >= 3.3

#if (ACGL_OPENGL_VERSION >= 31)
/**
 * Primitive queries count the number of processed geometry. Sounds trivial as the app should
 * know the number from the glDraw* calls, but this query will also count geometry generated
 * by geometry/tessellation shaders.
 *
 * During transform feedback let one query of each type run and compare the results: if more
 * primitives were generated than written to the TF buffer, the buffer overflowd.
 */
class PrimitiveQuery : public AsynchronousQuery {
public:
    PrimitiveQuery() : AsynchronousQuery( GL_PRIMITIVES_GENERATED ) {}
    PrimitiveQuery( GLenum _queryType ) : AsynchronousQuery( _queryType ) {
        setType( _queryType );
    }

    //! _queryType has to be GL_PRIMITIVES_GENERATED or GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    void setType( GLenum _queryType ) {
        if ((_queryType != GL_PRIMITIVES_GENERATED) && (_queryType != GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)) {
            Utils::error() << "PrimitiveQuery type " << _queryType << " not supported" << std::endl;
            _queryType = GL_PRIMITIVES_GENERATED;
        }
        mTarget = _queryType;
    }
};
ACGL_SMARTPOINTER_TYPEDEFS(PrimitiveQuery)
#endif // OpenGL >= 3.1

} // OpenGL
} // ACGL

#endif // ES 2.0

#endif // ACGL_OPENGL_OBJECTS_QUERY_HH
