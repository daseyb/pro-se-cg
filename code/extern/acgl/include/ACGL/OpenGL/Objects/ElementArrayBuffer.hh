/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_ELEMENTBUFFERDATA_HH
#define ACGL_OPENGL_OBJECTS_ELEMENTBUFFERDATA_HH

/**
 * An ElementArrayBuffer is an index into ArrayBuffers and defines which
 * elements of that array should be drawn in which order.
 *
 * The combination of (multiple) attributes of (multiple) ArrayBuffers
 * and one (optional) ElementArrayBuffer is a VertexBufferObject or VertexArrayObject.
 *
 * Note: In some documents ElementArrayBuffer are called VertexBufferObjects, VBOs
 *       or IndexBufferObjects (IBOs). But ArrayBuffers can also be called VBOs...
 *       The original extension that introduced these two new buffer types was called
 *       ARB_vertex_buffer_object but the buffers itself are called ArrayBuffer and
 *       ElementArrayBuffer.
 *
 * WARNING: On word of warning about the EAB: Setting data (e.g. via setData() ) will
 *          of course have to bind this buffer first. The binding of an EAB is a state
 *          of the currently bound VAO, not a global OpenGL state so make sure you bind
 *          the corresponding VAO or VAO 0 first, otherwise there will be a side-effect
 *          of having the wrong EAB bound to some random VAO that just happened to be
 *          bound by chance.
 *          While this behaviour is also true for other buffers and textures, those are
 *          bound to a global OpenGL state that gets changed all the time anyway so you
 *          are probably used to rebind those before using while a VAO does not rebind
 *          its own EAB before drawing as the binding is a state of the VAO.
 *          In debug mode the VAO will check for this kind of inconsistancy that arise
 *          from wrong usage.
 *          Other reasons why a wrong EAB is set to a VAO: Driver bug (some older Intel
 *          drivers did not store the EAB as a VAO state), manual binding (calling of bare
 *          OpenGL commands).
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Objects/Buffer.hh>


namespace ACGL{
namespace OpenGL{

class ElementArrayBuffer : public Buffer
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    ElementArrayBuffer( GLenum _type = GL_UNSIGNED_INT)
        : Buffer(GL_ELEMENT_ARRAY_BUFFER), mType(_type)
    {}

    ElementArrayBuffer( SharedBufferObject _pBuffer, GLenum _type  = GL_UNSIGNED_INT)
         : Buffer(_pBuffer, GL_ELEMENT_ARRAY_BUFFER), mType(_type)
    {}

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    //! returns the index data type: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
    inline GLenum getType(void) const { return mType; }

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/
public:
    //! _type has to be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT and indicates the
    //! datatype of the indices
    inline void setType (GLenum _type) { mType = _type; }

    //! set data for this buffer for a given number of elements (size in bytes == _elements * size of mType)
    inline void setDataElements( uint_t _elements, const GLvoid *_pData = NULL, GLenum _usage = GL_STATIC_DRAW )
    {
        setData( mTarget, _elements * getGLTypeSize(mType), _pData, _usage );
    }

    //! Returns the number of indices:
    inline GLuint getElements() const
    {
        return (GLuint)( getSize() / getGLTypeSize(mType));
    }

    //! Overloaded from the base class to _prevent_ redefining of the binding target! (see Buffer)
    inline void setTarget( GLenum )
    {
        ACGL::Utils::error() << "DON'T redefine the target binding point of an ElementArrayBuffer" << std::endl;
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    //! index data type: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
    GLenum mType;
};

ACGL_SMARTPOINTER_TYPEDEFS(ElementArrayBuffer)


} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_ELEMENTBUFFERDATA_HH
