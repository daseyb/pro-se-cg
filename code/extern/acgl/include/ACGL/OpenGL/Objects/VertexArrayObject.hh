/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_VERTEXARRAYOBJECT_HH
#define ACGL_OPENGL_OBJECTS_VERTEXARRAYOBJECT_HH

/**
 * A VertexArrayObject is a predefined combination of (multiple) attributes of
 * (multiple) ArrayBuffers and optionally one ElementArrayBuffer.
 *
 * It's only present in OpenGL 3.0 onwards. For older implementations (or
 * embedded systems) see VertexBufferObject which is a softwareimplementation
 * of the same idea).
 * Alternatively, there are the GL_APPLE_vertex_array_object and
 * GL_ARB_vertex_array_object extensions for OpenGL 2.1.
 * OES_vertex_array_object for OpenGL ES (e.g. iOS 4.0+)
 *
 * A VAO will cache the enabled vertex attributes (set with glEnableVertexAttribArray)
 * and vertex attribute pointer calls (glVertexAttribPointer).
 * Binding a VAO will restore that state (saving a lot of gl calls to do that
 * manually).
 *
 * Note: Primitive restart state (enable/disable, primitive restart index) is NOT
 *       part of the VAO state!
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#if defined (ACGL_OPENGL_SUPPORTS_VAO)

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/Tools.hh>

#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>
#include <ACGL/OpenGL/Objects/ElementArrayBuffer.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>

#include <vector>

namespace ACGL{
namespace OpenGL{

// if a VAO is present either because GL 3.0+ is present or because extensions are expected, signal this using this define
// for classes that expect VAO to be present!
class VertexArrayObject
{
    ACGL_NOT_COPYABLE(VertexArrayObject)

    // ==================================================================================================== \/
    // ============================================================================================ STRUCTS \/
    // ==================================================================================================== \/
public:
    struct Attribute
    {
        ConstSharedArrayBuffer arrayBuffer; // the ArrayBuffer to use
        int32_t                attributeID; // the attribute from that ArrayBuffer
        // more Attribute properties can be looked up in the ArrayBuffer (like the name)
    };

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< Attribute > AttributeVec; // position in the vector == attrib location

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    VertexArrayObject( GLenum _mode = GL_TRIANGLES );

    virtual ~VertexArrayObject(void)
    {
        // as always, OpenGL will ignore object name 0
        glDeleteVertexArrays(1, &mObjectName);
    }

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
    void setObjectLabel( const std::string &_label );
    std::string getObjectLabel();

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/
public:
    /**
     * Sets the default mode of drawing.
     * Must be one of: GL_POINTS
     *                 GL_LINE_STRIP
     *                 GL_LINE_LOOP
     *                 GL_LINES
     *                 GL_TRIANGLE_STRIP
     *                 GL_TRIANGLES_FAN
     *                 GL_TRIANGLES
     * OpenGL >= 3.2 : GL_LINE_STRIP_ADJACENCY
     *                 GL_LINES_ADJACENCY
     *                 GL_TRIANGLE_STRIP_ADJACENCY
     *                 GL_TRIANGLES_ADJACENCY
     * OpenGL >= 4.0 : GL_PATCHES
     *
     */
    inline void setMode(GLenum _mode) { mMode = _mode; }

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline       GLuint                        getObjectName()         const { return mObjectName; }
    inline const AttributeVec&                 getAttributes()         const { return mAttributes; }
    inline       GLenum                        getMode()               const { return mMode;       }
    inline       ConstSharedElementArrayBuffer getElementArrayBuffer() const { return mpElementArrayBuffer; }
    inline       GLuint                        getElements()           const { return mAttributes[0].arrayBuffer->getElements(); }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    /**
     * Set the given ElementArrayBuffer, if a NULL pointer is given, an existing EAB will get unset.
     * Will restore the previously bound VAO (DSA style)
     */
    void attachElementArrayBuffer( const ConstSharedElementArrayBuffer& _elementArrayBuffer );

    //! sets the EAB to NULL if there was one
    inline void detachElementArrayBuffer() { attachElementArrayBuffer( ConstSharedElementArrayBuffer() ); }

    /**
     * Will set the attribute _arrayBufferAttribute of ArrayBuffer _arrayBuffer to the given attribute location.
     * If that location was already used it will get overwritten.
     * The _attributeLocation has to be lower than GL_MAX_VERTEX_ATTRIBS
     * An attribute location of -1 indicates that the attribute should get the next free location
     */
    inline void attachAttribute( const ConstSharedArrayBuffer& _arrayBuffer,
                                 uint32_t _arrayBufferAttribute,
                                 GLint    _attributeLocation = -1)
    {
        Attribute newAttribute = { _arrayBuffer, (int32_t) _arrayBufferAttribute };
        attachAttribute( newAttribute, _attributeLocation );
    }

    /**
     * Will set the attribute named _arrayBufferAttributeName of ArrayBuffer _arrayBuffer to the given attribute location.
     * If that location was already used it will get overwritten.
     * The _attributeLocation has to be lower than GL_MAX_VERTEX_ATTRIBS
     * An attribute location of -1 indicates that the attribute should should get the next free location
     */
    void attachAttribute( const ConstSharedArrayBuffer& _arrayBuffer,
                          const std::string& _arrayBufferAttributeName,
                                GLint        _attributeLocation = -1);

    //! if the location was already in use, the new attribute will replace it, if a free loc should be used but no location is free,
    //! nothing will get attached.
    void attachAttribute( const Attribute &_attribute, GLint _location );

    /**
     * Attaches all attributes defined by an ArrayBuffer
     * The attributes are attached to the next free location.
     * Afterwards, you might want to automatically wire up the attributes with a ShaderProgram, using setAttributeLocationsByShaderProgram
     */
    void attachAllAttributes( const ConstSharedArrayBuffer& _arrayBuffer );

    /**
     * Will detach the Attribute with the given attribute location.
     */
    void detachAttribute( GLuint _location );

    /**
     * Will detach the first found Attribute with the given name.
     */
    void detachAttribute( const std::string &_name );

    /**
     * Will detach all currently attached Attributes
     */
    void detachAllAttributes();

    /**
     * Query the attribute names from the ArrayBuffers.
     * If they match a name from _locationMappings, use the location from _locationMappings.
     * All remaining locations will get a unique location as well (how this is calculated is undefined).
     */
    void setAttributeLocations( ConstSharedLocationMappings _locationMappings );

    //! get a list of attribute locations and names of this VAO. That list can then be used to set up a ShaderProgram.
    SharedLocationMappings getAttributeLocations() const;

private:
    //! Sets the vertex attribute pointer for the current VAO according to the specified attribute (index into mAttributes)
    //! Note: expects that this VAO is currently bound
    //! Note: will bind the ArrayBuffer referenced by _attribute.arrayBuffer
    void setAttributePointer( GLuint _index );

    //! returns a free attribute location between 0 .. MAX_ATTRIB_LOCATIONS unless no location is free, in that case returns -1
    GLint getFreeAttributeLocation();

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:
    //! Bind this VAO
    //! some consistancy checks are made
    void bind() const;

    //! Nothing has to be prepared for a render call
    inline void render (void)
    {
        storeOldVAOandBind();
        draw();
        restoreOldVAO();
    }

    //! Will select the matching draw call. Remember to bind() first!
    void draw( GLsizei _primcount = 1 ) const
    {
        if (mpElementArrayBuffer) {
            drawElements( _primcount );
        } else {
            drawArrays( _primcount );
        }
    }

    // ===================================================================================================== \/
    // ========================================================================================= DRAW ARRAYS \/
    // ===================================================================================================== \/

    //! stores parameters for drawArraysIndirect
    typedef struct {
        GLuint count;
        GLuint primCount;
        GLuint first;
        GLuint baseInstance; // must be 0 pre OpenGL 4.2
    } DrawArraysIndirectCommand;

    //! returns the number of vertices in one of the array buffers. NOTE: only useful if all array buffer have the same number of elements!
    inline GLsizei getArrayVertexCount() const {
        int firstAttribute = getFirstAttributeIndex(); // will be -1 if no attribte is set.
        return ( (firstAttribute == -1)?0:mAttributes[firstAttribute].arrayBuffer->getElements());
    }

    //! draws the whole VAO
    inline void drawArrays(void) const {
        glDrawArrays( mMode, 0, getArrayVertexCount() );
    }

    //! draws the whole VAO, primcount is the number used for instancing
    //! baseinstance does not modify gl_InstanceID
    inline void drawArrays(GLsizei primcount, GLuint baseinstance = 0) const
    {
#if (ACGL_OPENGL_VERSION >= 42)
        glDrawArraysInstancedBaseInstance( mMode, 0, getArrayVertexCount(), primcount, baseinstance );
#elif (ACGL_OPENGL_VERSION >= 31)
        if (baseinstance != 0) ACGL::Utils::error() << "baseinstance is only supported on OpenGL 4.2 and later!" << std::endl;
        glDrawArraysInstanced( mMode, 0, getArrayVertexCount(), primcount );
#else
        if ((baseinstance != 0) || (primcount != 1)) ACGL::Utils::error() << "baseinstance and primcount are only supported on OpenGL 3.1 and later!" << std::endl;
        glDrawArrays( mMode, 0, getArrayVertexCount() );
#endif
    }

#if (ACGL_OPENGL_VERSION >= 20)
    // not on OpenGL ES
    //! dispatch multiple draw commands with different parameters, for drawing the same object multiple times, use drawArrays to use instancing
    //! first and count are arrays of the size primcount (so primcount chunks are beeing rendered)
    inline void multiDrawArrays( GLint *first, GLsizei *count, GLsizei primcount ) const
    {
        glMultiDrawArrays( mMode, first, count, primcount );
    }
#endif

#if (ACGL_OPENGL_VERSION >= 40)
    //! with a parameter struct:
    inline void drawArraysIndirect( const DrawArraysIndirectCommand *_indirect ) const {
        glDrawArraysIndirect( mMode, (void*) _indirect );
    }
    //! with an offset assuming a buffer is bound to GL_DRAW_INDIRECT_BUFFER
    inline void drawArraysIndirectBufferOffset( GLsizeiptr _offset = 0 ) const {
        glDrawArraysIndirect( mMode, (void*) _offset );
    }
#endif

    // ===================================================================================================== \/
    // ======================================================================================= DRAW ELEMENTS \/
    // ===================================================================================================== \/
    /*
      NOTE: no wrappers yet for:
      glDrawElementsBaseVertex
      glDrawRangeElements
      glDrawRangeElementsBaseVertex
     */

    typedef struct {
        GLuint count;
        GLuint primCount;
        GLuint firstIndex;
        GLint  baseVertex;
        GLuint baseInstance; // must be 0 pre OpenGL 4.2
    } DrawElementsIndirectCommand;

    //! returns the number of vertices indexed by the element array buffer (0 if there is none)
    inline GLsizei getIndexCount() const { return (mpElementArrayBuffer)?mpElementArrayBuffer->getElements():0; }

    //! returns the index data type: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT or GL_INVALID_ENUM if no index is set
    inline GLenum getIndexType() const { return (mpElementArrayBuffer)?mpElementArrayBuffer->getType():GL_INVALID_ENUM; }

    //! Can be called directly instead of draw() iff the caller knows this is the correct call!
    //! Draws all elements
    inline void drawElements(void) const
    {
        glDrawElements(mMode, getIndexCount(), getIndexType(), (GLvoid*)NULL);
    }

#if (ACGL_OPENGL_VERSION >= 20)
    // not on OpenGL ES
    //! count and indices are arrays of the size primcount (so primcount chunks are beeing rendered)
    inline void multiDrawElements( GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount ) const
    {
        glMultiDrawElements( mMode, count, type, indices, primcount );
    }
#endif

    inline void drawRangeElements( GLuint start, GLsizei count )
    {
        glDrawRangeElements( mMode, start, getIndexCount(), count, getIndexType(), NULL);
    }

#if (ACGL_OPENGL_VERSION >= 32)
    inline void multiDrawElements( GLsizei *count, GLenum type, GLvoid **indices, GLsizei primcount, GLint *basevertex ) const
    {
        glMultiDrawElementsBaseVertex( mMode, count, type, indices, primcount, basevertex );
    }
#endif

    //! for instanced rendering: primcount is the number of instances
    //! baseinstance does not modify gl_InstanceID
    inline void drawElements( GLsizei primcount, GLint basevertex = 0, GLuint baseinstance = 0 ) const {
#if (ACGL_OPENGL_VERSION >= 42)
        glDrawElementsInstancedBaseVertexBaseInstance( mMode, getIndexCount(), getIndexType(), (GLvoid*)NULL, primcount, basevertex, baseinstance );
#elif (ACGL_OPENGL_VERSION >= 32)
        if (baseinstance != 0) ACGL::Utils::error() << "baseinstance is only supported on OpenGL 4.2 and later!" << std::endl;
        glDrawElementsInstancedBaseVertex( mMode, getIndexCount(), getIndexType(), (GLvoid*)NULL, primcount, basevertex );
#elif (ACGL_OPENGL_VERSION >= 31)
        if ((baseinstance != 0) || (basevertex != 0)) ACGL::Utils::error() << "baseinstance and basevertex are only supported on OpenGL 3.2 and later!" << std::endl;
        glDrawElementsInstanced( mMode, getIndexCount(), getIndexType(), (GLvoid*)NULL, primcount );
#else
        if ((baseinstance != 0) || (basevertex != 0) || (primcount != 1)) ACGL::Utils::error() << "drawElements not supported in this OpenGL version!" << std::endl;
        drawElements();
#endif
    }

#if (ACGL_OPENGL_VERSION >= 40)
    //! with a parameter struct:
    inline void drawElementsIndirect( const DrawElementsIndirectCommand *_indirect ) const {
        glDrawElementsIndirect( mMode, getIndexType(), (void*) _indirect );
    }
    //! with an offset assuming a buffer is bound to GL_DRAW_INDIRECT_BUFFER
    inline void drawElementsIndirect( GLsizeiptr _offset = 0) const {
        glDrawElementsIndirect( mMode, getIndexType(), (void*) _offset );
    }
#endif

private:
    //! Bind this VAO and remember the previously bound VAO
    //! Note: every call to this method must be paired with a corresponding call to disable()
    inline void storeOldVAOandBind(void)
    {
        // remember old VAO
        glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &mPreviousVAOName );
        if (mObjectName != (GLuint)mPreviousVAOName) bind();
    }

    //! Bind the VAO that was bound before the most recent enable() call
    inline void restoreOldVAO(void)
    {
        if (mObjectName != (GLuint)mPreviousVAOName) glBindVertexArray((GLuint)mPreviousVAOName);
    }

    //! Returns the first attribute that has a non-NULL ArrayBuffer attached
    inline int_t getFirstAttributeIndex() const
    {
        // TODO: cache this value?
        for (unsigned int index = 0; index < mAttributes.size(); ++index)
            if (mAttributes[index].arrayBuffer)
                return index;
        return -1;
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
private:
    ConstSharedElementArrayBuffer mpElementArrayBuffer; // optional EAB
    AttributeVec                  mAttributes;          // vertex attributes
    GLuint                        mObjectName;          // OpenGL object name
    GLenum                        mMode;                // primitive type to render (e.g. GL_TRIANGLES)
    GLint                         mPreviousVAOName;     // the VAO that was bound before the last enable() call
};

ACGL_SMARTPOINTER_TYPEDEFS(VertexArrayObject)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_SUPPORTS_VAO

#endif // ACGL_OPENGL_OBJECTS_VERTEXARRAYOBJECT_HH
