/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once


#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

class ArrayBufferCreator
{
    // ==================================================================================================== \/
    // ============================================================================================ STRUCTS \/
    // ==================================================================================================== \/
public:
    struct AttributeDefine
    {
        std::string name;
        GLenum      type;
        GLint       dimension;
        GLboolean   normalized;
        GLboolean   isInteger;
    };

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< AttributeDefine > AttributeDefineVec;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    ArrayBufferCreator(void)
    :   mUsage(GL_STATIC_DRAW),
        mElements(0),
        mpData(NULL),
        mAttributeDefines()
    {}
    virtual ~ArrayBufferCreator() {}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    inline ArrayBufferCreator& usage (GLenum _usage) { mUsage = _usage; return *this; }

    inline ArrayBufferCreator& data (const GLvoid* _pData, GLsizei _elements)
    {
        mpData = _pData;
        mElements = _elements;
        return *this;
    }

    inline ArrayBufferCreator& attribute (const std::string& _name, GLenum _type, GLint _dimension, GLboolean _normalized = GL_FALSE)
    {
        AttributeDefine a = {_name, _type, _dimension, _normalized, GL_FALSE};
        mAttributeDefines.push_back(a);
        return *this;
    }

    inline ArrayBufferCreator& integerAttribute (const std::string& _name, GLenum _type, GLint _dimension)
    {
        AttributeDefine a = {_name, _type, _dimension, GL_FALSE, GL_TRUE};
        mAttributeDefines.push_back(a);
        return *this;
    }

    // ===================================================================================================== \/
    // ============================================================================================ OVERRIDE \/
    // ===================================================================================================== \/
public:
    SharedArrayBuffer create();

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    GLenum  mUsage;
    GLsizei mElements;
    const GLvoid* mpData;
    AttributeDefineVec mAttributeDefines;
};

} // OpenGL
} // ACGL

