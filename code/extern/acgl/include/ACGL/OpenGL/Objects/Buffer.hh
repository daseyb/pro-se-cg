/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_BUFFER_HH
#define ACGL_OPENGL_OBJECTS_BUFFER_HH

/**
 * A generic OpenGL Buffer Object
 *
 * Mostly an OpenGL Buffer Wrapper: names of OpenGL calls are stripped of the
 * 'gl' and 'Buffer' tokens and setters got a 'set' prefix.
 *
 * Calls that give the target the buffer should get bound to have an alternative
 * call that uses the last used or set target.
 *
 * Note: Most methods will bind this buffer!
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>

#include <string>
#include <vector>

namespace ACGL{
namespace OpenGL{


/**
 * A minimal(!) wrapper to allow multiple Buffer objects pointing to the same
 * OpenGL resource (like an ArrayBuffer and a TransformFeedbackBuffer).
 *
 * This has to be an extra object so all Buffer types can inherit from Buffer
 * below to allow a unified API.
 */
class BufferObject {
    ACGL_NOT_COPYABLE(BufferObject)
public:
    BufferObject()
    {
        mObjectName = 0;
        glGenBuffers(1, &mObjectName);
    }

    ~BufferObject(void)
    {
        // buffer 0 will get ignored by OpenGL
        glDeleteBuffers(1, &mObjectName);
    }
    GLuint mObjectName;

    //! has the side effect of binding this buffer.
    //! returned value is in bytes
    GLsizei getSize( GLenum _asTarget ) {
        glBindBuffer( _asTarget, mObjectName );
        GLint value;
        glGetBufferParameteriv( _asTarget, GL_BUFFER_SIZE, &value );
        return value;
    }
};
typedef ptr::shared_ptr<BufferObject> SharedBufferObject;


/**
 * Buffers are general OpenGL Buffer Wrapper.
 * The OpenGL resource itself is attached via a shared pointer (GLBufferObject).
 * This was multiple Buffers can use internaly the same OpenGL resource, this is
 * useful if one resource should get interpreted as _different_ buffer types
 * so in that case the same GLBufferObject will get attached to different
 * Subclass Objects.
 *
 * Note: Subclasses should set the mTarget in there constructors!
 */
class Buffer
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    //! Most common default: a new Buffer corresponds to a new GL resource. You might decide on a binding point
    //! now or later.
    Buffer( GLenum _target ) : mSize(0), mTarget(_target)
    {
        mBuffer = SharedBufferObject( new BufferObject() );
    }

    /**
     * Init with a given, external GL resource.
     *
     * Calling this with:
     * Buffer b( SharedGLBufferObject(NULL) );
     * Is the way to _explicitly_ state that a real OpenGL resource will get added later.
     * In this case no GL wrapper calls should ever get called until one gets set!
     */
    Buffer( SharedBufferObject _pBuffer, GLenum _target )
        : mBuffer( _pBuffer ),
		  mTarget(_target)
    {
        mSize = (!mBuffer)?0:mBuffer->getSize( mTarget );
    }

    virtual ~Buffer(){}

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_BUFFER>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_BUFFER>(getObjectName()); }
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGLES_VERSION >= 10))
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_BUFFER_OBJECT_EXT>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_BUFFER_OBJECT_EXT>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif


    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint getObjectName (void) const { return mBuffer->mObjectName; }
    inline bool   isValid       (void) const { return (mBuffer && glIsBuffer( mBuffer->mObjectName ) ); }
    inline SharedBufferObject getBufferObject () const { return mBuffer; }

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/

    //! the GL buffer can get changed at any time
    void setBufferObject( SharedBufferObject _pBuffer ) { mBuffer = _pBuffer; mSize = (!mBuffer)?0:mBuffer->getSize( mTarget ); }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
private:
    inline GLint getParameter( GLenum _parameter ) const
    {
        bind( mTarget );
        GLint value;
        glGetBufferParameteriv( mTarget, _parameter, &value );
        return value;
    }

#if ( !defined(ACGL_OPENGL_ES) || (ACGL_OPENGLES_VERSION >= 30))
// desktop or ES 3.0 onwards:

#if (ACGL_OPENGL_VERSION >= 32)
    inline GLint64 getParameter64( GLenum _parameter ) const
    {
        bind( mTarget );
        GLint64 value;
        glGetBufferParameteri64v( mTarget, _parameter, &value );
        return value;
    }

public:
    //! not side effect free! will bind this buffer to it's last target!
    //! caching of these values on RAM could be a good idea if needed very(!) often (as it's done with the size)!
    //inline GLint64   getSize()        const { return             getParameter64( GL_BUFFER_SIZE         ); }
    inline GLint64   getMapOffset()   const { return             getParameter64( GL_BUFFER_MAP_OFFSET   ); }
    inline GLint64   getMapLength()   const { return             getParameter64( GL_BUFFER_MAP_LENGTH   ); }
#else // OpenGL pre 3.2 or OpenGL ES 3.0:

public:
    //inline GLint     getSize()        const { return             getParameter  ( GL_BUFFER_SIZE         ); }
    inline GLint     getMapOffset()   const { return             getParameter  ( GL_BUFFER_MAP_OFFSET   ); }
    inline GLint     getMapLength()   const { return             getParameter  ( GL_BUFFER_MAP_LENGTH   ); }
#endif // OpenGL >= 3.2
#if (ACGL_OPENGL_VERSION >= 20)
    inline GLenum    getAccess()      const { return (GLenum)    getParameter  ( GL_BUFFER_ACCESS       ); }
#endif // not on ES
    inline GLint     getAccessFlags() const { return (GLint)     getParameter  ( GL_BUFFER_ACCESS_FLAGS ); }
    inline GLboolean isMapped()       const { return (GLboolean) getParameter  ( GL_BUFFER_MAPPED       ); }

#endif // desktop & ES 3
public:
    inline GLenum    getUsage()       const { return (GLenum)    getParameter  ( GL_BUFFER_USAGE        ); }
    
    //! the size is in bytes
    inline GLint64   getSize()        const { return mSize; }

    //! Bind this buffer
    inline void bind( GLenum _target ) const
    {
        glBindBuffer( _target, mBuffer->mObjectName );
    }

    //! Bind this buffer to its target
    inline void bind() const
    {
        glBindBuffer( mTarget, mBuffer->mObjectName );
    }

    //! Set data for this buffer. Use only to init the buffer!
    //! Note: The function is not const, because it changes the corresponding GPU data
    inline void setData( GLenum _target, GLsizeiptr _sizeInBytes, const GLvoid *_pData = NULL, GLenum _usage = GL_STATIC_DRAW ) {
        mSize = _sizeInBytes;
        bind( _target );
        glBufferData( _target, _sizeInBytes, _pData, _usage );
    }

    //! Set data for this buffer at the last used target. Use only to init the buffer!
    inline void setData( GLsizeiptr _sizeInBytes, const GLvoid *_pData = NULL, GLenum _usage = GL_STATIC_DRAW ) {
        setData( mTarget, _sizeInBytes, _pData, _usage );
    }

    //! Use this to modify the buffer
    inline void setSubData( GLenum _target, GLintptr _offset,
                            GLsizeiptr _sizeInBytes, const GLvoid *_pData ) {
        bind( _target );
        glBufferSubData( _target, _offset, _sizeInBytes, _pData );
    }

    //! Use this to modify the buffer
    inline void setSubData( GLintptr _offset, GLsizeiptr _sizeInBytes, const GLvoid *_pData ) {
        setSubData( mTarget, _offset, _sizeInBytes, _pData );
    }


#if ((ACGL_OPENGL_VERSION >= 30) || (ACGL_OPENGLES_VERSION >= 30))
    /** Map a part of the buffer to client memory
     * _offset & _length are values in bytes relative to the buffer
     * _access must contain one (or both) of:
     *      GL_MAP_READ_BIT and GL_MAP_WRITE_BIT
     *  and optionally:
     *      GL_MAP_INVALIDATE_RANGE_BIT GL_MAP_INVALIDATE_BUFFER_BIT
     *      GL_MAP_FLUSH_EXPLICIT_BIT GL_MAP_UNSYNCHRONIZED_BIT
     */
    GLvoid *mapRange( GLenum _target, GLintptr _offset, GLsizeiptr _length, GLbitfield _access ) {
        bind( _target );
        GLvoid *ret = glMapBufferRange( _target, _offset, _length, _access );
        return ret;
    }

    inline GLvoid *mapRange( GLintptr _offset, GLsizeiptr _length, GLbitfield _access ) {
        return mapRange( mTarget, _offset, _length, _access );
    }

    /**
     * Spec:
     * If a buffer is mapped with the GL_MAP_FLUSH_EXPLICIT_BIT flag, modifications
     * to the mapped range may be indicated by calling this.
     * _offset and _length indicate a modified subrange of the mapping, in byte. The specified
     * subrange to flush is relative to the start of the currently mapped range of buffer.
     * This can be called multiple times to indicate distinct subranges
     * of the mapping which require flushing.
     */
    void flushMappedRange( GLenum _target, GLsizeiptr _offset, GLsizeiptr _length ) {
        bind( _target );
        glFlushMappedBufferRange( _target, _offset, _length );
    }

    inline void flushMappedRange( GLintptr _offset, GLsizeiptr _length ) {
        flushMappedRange( mTarget, _offset, _length );
    }

    //! valid targets are only GL_TRANSFORM_FEEDBACK_BUFFER and GL_UNIFORM_BUFFER
    inline void bindBufferRange( GLenum _target, GLuint _index, GLintptr _offset, GLsizeiptr _size ) {
        glBindBufferRange( _target, _index, mBuffer->mObjectName, _offset, _size );
    }

    //! maps a subset of the buffer defined by _offset and _size
    //! valid targets are only GL_TRANSFORM_FEEDBACK_BUFFER and GL_UNIFORM_BUFFER
    inline void bindBufferRange( GLuint _index, GLintptr _offset, GLsizeiptr _size ) {
        glBindBufferRange( mTarget, _index, mBuffer->mObjectName, _offset, _size );
    }

    //! maps the full buffer to the given index (binding point)
    //! valid targets are only GL_TRANSFORM_FEEDBACK_BUFFER and GL_UNIFORM_BUFFER
    inline void bindBufferBase( GLenum _target, GLuint _index ) {
        glBindBufferBase( _target, _index, mBuffer->mObjectName );
    }

    //! maps the full buffer to the given index (binding point)
    //! valid targets are only GL_TRANSFORM_FEEDBACK_BUFFER and GL_UNIFORM_BUFFER
    inline void bindBufferBase( GLuint _index ) {
        glBindBufferBase( mTarget, _index, mBuffer->mObjectName );
    }

#endif // OpenGL >= 3.0

#if (ACGL_OPENGL_VERSION >= 20)
    //! Maps the whole buffer, if using GL 3+, better use mapRange!
    //! _access is GL_READ_ONLY GL_WRITE_ONLY or GL_READ_WRITE
    GLvoid *map( GLenum _target, GLenum _access ) {
        bind( _target );
        GLvoid *ret = glMapBuffer( _target, _access );
        return ret;
    }
    inline GLvoid *map( GLenum _access ) {
        return map( mTarget, _access );
    }

    GLboolean unmap( GLenum _target ) {
        bind( _target );
        GLboolean ret = glUnmapBuffer( _target );
        return ret;
    }

    inline GLboolean unmap() {
        return unmap( mTarget );
    }
#endif

    // TODO: CopyBufferSubData

    /**
     * _target must be one of:
        GL_ARRAY_BUFFER
        GL_ATOMIC_COUNTER_BUFFER
        GL_COPY_READ_BUFFER
        GL_COPY_WRITE_BUFFER
        GL_DRAW_INDIRECT_BUFFER
        GL_ELEMENT_ARRAY_BUFFER
        GL_PIXEL_PACK_BUFFER
        GL_PIXEL_UNPACK_BUFFER
        GL_TEXTURE_BUFFER
        GL_TRANSFORM_FEEDBACK_BUFFER
        GL_UNIFORM_BUFFER
     * Can be changed at any time.
     *
     * Subclasses should overload this with a non-working function (+ a warning)
     * because an X-Buffer should not be attached _per default_ to Y!
     * Subclass buffers can however always use the method calls / binds with an
     * _explicit_ target (that doesn't match there one ones):
     *
     * XBuffer x;
     * x.bind( Y ); // ok, hope the programmer knowns what s/he does
     *
     * x.setTarget( Y ); // this is just calling for unintended side-effects!
     * x.bind();
     */
    virtual inline void setTarget( GLenum _target ) { mTarget = _target; }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLint64            mSize; // as this might get queried often (e.g. ArrayBuffer) we will explicitly mirror it in RAM - bytes
    SharedBufferObject mBuffer;
    GLenum             mTarget;
};

ACGL_SMARTPOINTER_TYPEDEFS(Buffer)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_BUFFER_HH
