/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_ARRAYBUFFER_HH
#define ACGL_OPENGL_OBJECTS_ARRAYBUFFER_HH

/**
 * An ArrayBuffer holds an array of per-vertex data. In its simplest form an
 * array of one attribute, for example the vertex position or texture-coordinate.
 * An ArrayBuffer however can also hold multiple attributes in an interleaved
 * way.
 *
 * An ArrayBuffer can be drawn directly or indexed in combination with an
 * ElementArrayBuffer.
 *
 * The combination of (multiple) attributes of (multiple) ArrayBuffers
 * and one (optional) ElementArrayBuffer is a VertexBufferObject or VertexArrayObject.
 *
 * Note: In some documents ArrayBuffers (and sometimes ElementArrayBuffers) are
 *       called VertexBufferObjects, VBOs. The original extension that introduced
 *       these two new buffer types was called ARB_vertex_buffer_object but the buffers
 *       itself are called ArrayBuffer and ElementArrayBuffer.
 *
 ***************************************************************************************************************
 * Attributes:
 *************
 *
 * _type is the GL type
 * _size the number of elements in this attribute (1..4)
 * _normalized is the attribute normalization for int types
 *
 * Want to add tightly packed attributes in order?
 *  -> use defineAttribute()
 *
 * Want to add attributes with individual padding in order?
 *  -> use defineAttributeWithPadding()
 *
 * Want to add attributes out-of-order?
 *  -> use defineAttributeWithOffset()
 *
 * The stride size gets always set to the minimal stride size that covers all defined attributes (/w padding).
 * All define methods can get mixed!
 *
 *
 * ab->defineAttribute(            "pos",       GL_FLOAT, 3    ); // stride: 12 bytes
 * ab->defineAttributeWithPadding( "color",     GL_CHAR,  3, 1 ); // stride: 12 + 3 + 1 = 16 bytes
 * ab->defineAttributeWithOffset(  "colorNorm", GL_CHAR,  3, 12, GL_TRUE ); // stride is still 16 as 12+3 <= 16!
 *
 **************************************************************************************************************/

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>

#include <algorithm>
#include <string>
#include <vector>

#include <ACGL/OpenGL/Objects/Buffer.hh>

namespace ACGL{
namespace OpenGL{

class GeometryData;
ACGL_SMARTPOINTER_TYPEDEFS(GeometryData)

class ArrayBuffer : public Buffer
{
    // ==================================================================================================== \/
    // ============================================================================================ STRUCTS \/
    // ==================================================================================================== \/
public:
    //! Each attribute has a size (#components, e.g. normal with x/y/z => 3) and an offset in the stride (in bytes)
    struct Attribute
    {
        std::string name;       // human readable name, can be used to match the attribute to shader programs
        GLenum      type;       // GL_FLOAT, GL_UNSIGNED_BYTE etc.
        GLint       size;       // #elements per attribute, size in bytes would be: size*sizeof(type)
        GLuint      offset;     // offset in bytes into the array
        GLboolean   normalized; // int types can get normalzed to 0..1 / -1..1 by GL, useful e.g. for colors
        GLuint      divisor;    // vertex divisor for instancing, supported since OpenGL 3.3. Default is 0 (== off)
        GLboolean   isIntegerInShader; // should the data get read as vec or ivec ?
    };

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< Attribute > AttributeVec;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    //! creates an ArrayBuffer with a new OpenGL Buffer object
    ArrayBuffer()
        : Buffer(GL_ARRAY_BUFFER),
        mStride(0),
        mAttributes()
    {}

    //! creates an ArrayBuffer with a pre-existing OpenGL Buffer
    ArrayBuffer( SharedBufferObject _pBuffer )
        : Buffer(_pBuffer, GL_ARRAY_BUFFER),
        mStride(0),
        mAttributes()
    {}

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:

    //! elements is the number of vertices or instances (if the divisor is != 0):
    inline       GLsizei       getElements   (void) const { return (GLsizei) (mStride==0?0:(mSize/mStride)); }

    //! size in bytes of all attributes that make up one element (vertex):
    inline       GLsizei       getStride     (void) const { return mStride;     }

    //! Returns the definitions of the attributes:
    inline const AttributeVec& getAttributes (void) const { return mAttributes; }

    //! Returns one attribute:
    inline Attribute getAttribute( uint_t i ) const { return mAttributes[i]; }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! Adds the attribute at the end of the existing attributes, stride gets computed automatically
    void defineAttribute(            const std::string& _name, GLenum _type, GLint _size, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);
    void defineIntegerAttribute(     const std::string& _name, GLenum _type, GLint _size, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);

    //! Adds the attribute at the end of the existing attributes, stride gets computed automatically
    //! + extra padding in bytes at the end
    void defineAttributeWithPadding(        const std::string& _name, GLenum _type, GLint _size, GLuint _padding, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);
    void defineIntegerAttributeWithPadding( const std::string& _name, GLenum _type, GLint _size, GLuint _padding, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);

    //! Adds an attribute defined by an offset: this way an attribute can get added at arbitrary
    //! locations in the stride. If it's added at the end, the stride gets resized. This way attributes can even
    //! overlap, hope you know what you're doing...
    void defineAttributeWithOffset(        const std::string& _name, GLenum _type, GLint _size, GLuint _offset, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);
    void defineIntegerAttributeWithOffset( const std::string& _name, GLenum _type, GLint _size, GLuint _offset, GLboolean _normalized = GL_FALSE, GLuint _divisor = 0);

    //! Takes care of a valid stride size and adds the attribute
    void defineAttribute( const Attribute &_attribute );

    //! Returns the index of a named attribute
    int32_t getAttributeIndexByName(const std::string& _nameInArray) const;

    //! Setting of the stride size explicitly is not needed if the attributes are defined correctly (with padding)
    inline void setStride( GLsizei _stride ) { mStride = _stride; }

    //! Sets all data (attributes, stride, size) to those specified in _geometryData. All previous data are overwritten.
    void setGeometryData( SharedGeometryData _geometryData );

    //! removes all attributes
    inline void removeAttributes(void)
    {
        mStride = 0;
        mAttributes.clear();
    }

    //! Set data for this buffer for a given number of elements (_elements*mStride == size in bytes)
    //! Use only after all attributes have been defined
    inline void setDataElements( uint_t _elements, const GLvoid *_pData = NULL, GLenum _usage = GL_STATIC_DRAW ) {
        setData( mTarget, _elements * mStride, _pData, _usage );
    }

    //! Overloaded from the base class to _prevent_ redefining of the binding target! (see Buffer)
    inline void setTarget( GLenum ) {
        ACGL::Utils::error() << "DON'T redefine the target binding point of an ArrayBuffer" << std::endl;
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLsizei      mStride;
    AttributeVec mAttributes;
};

ACGL_SMARTPOINTER_TYPEDEFS(ArrayBuffer)


} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_ARRAYBUFFER_HH
