/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>
#include <ACGL/OpenGL/Data/GeometryData.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;

//
// float attributes:
//
void ArrayBuffer::defineAttribute(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLboolean _normalized,
    GLuint _divisor )
{
    GLuint offset = mStride;
    Attribute attribute = { _name, _type, _size, offset, _normalized, _divisor, GL_FALSE };
    defineAttribute(attribute);
}

void ArrayBuffer::defineAttributeWithPadding(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLuint _padding,
    GLboolean _normalized,
    GLuint _divisor )
{
    GLuint offset = mStride;
    Attribute attribute = { _name, _type, _size, offset, _normalized, _divisor, GL_FALSE };
    defineAttribute(attribute);
    // defineAttribute will shift the mStride to the end of this attribute, so we only have to
    // add the explicit padding:
    mStride += _padding;
}

void ArrayBuffer::defineAttributeWithOffset(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLuint _offset,
    GLboolean _normalized,
    GLuint _divisor )
{
    Attribute attribute = { _name, _type, _size, _offset, _normalized, _divisor, GL_FALSE };
    defineAttribute(attribute);
}

//
// integer attributes:
//
void ArrayBuffer::defineIntegerAttribute(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLboolean _normalized,
    GLuint _divisor )
{
    GLuint offset = mStride;
    Attribute attribute = { _name, _type, _size, offset, _normalized, _divisor, GL_TRUE };
    defineAttribute(attribute);
}

void ArrayBuffer::defineIntegerAttributeWithPadding(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLuint _padding,
    GLboolean _normalized,
    GLuint _divisor )
{
    GLuint offset = mStride;
    Attribute attribute = { _name, _type, _size, offset, _normalized, _divisor, GL_TRUE };
    defineAttribute(attribute);
    // defineAttribute will shift the mStride to the end of this attribute, so we only have to
    // add the explicit padding:
    mStride += _padding;
}

void ArrayBuffer::defineIntegerAttributeWithOffset(
    const std::string& _name,
    GLenum _type,
    GLint  _size,
    GLuint _offset,
    GLboolean _normalized,
    GLuint _divisor )
{
    Attribute attribute = { _name, _type, _size, _offset, _normalized, _divisor, GL_TRUE };
    defineAttribute(attribute);
}


//
// int/float already defined by the Attribute:
//
void ArrayBuffer::defineAttribute( const Attribute &_attribute )
{
    // this way attribute definitions don't have to be in order!
    GLsizei attributeWidth = getGLTypeSize(_attribute.type)*_attribute.size; // in bytes
    mStride = std::max( (GLsizei)_attribute.offset + attributeWidth, mStride);
    mAttributes.push_back( _attribute );
}

int32_t ArrayBuffer::getAttributeIndexByName(const std::string& _nameInArray) const
{
    for(AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
        if(mAttributes[i].name == _nameInArray)
            return (int32_t) i;
    return -1;
}

void ArrayBuffer::setGeometryData(SharedGeometryData _geometryData)
{
    removeAttributes();
    for(ArrayBuffer::AttributeVec::const_iterator it = _geometryData->mAttributes.begin();
        it != _geometryData->mAttributes.end();
        ++it)
    {
        defineAttribute(*it);
    }
    setStride(_geometryData->getStrideSize());
    setData(_geometryData->getSize(), _geometryData->getData());
}
