/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_DATA_GEOMETRYDATA_HH
#define ACGL_OPENGL_DATA_GEOMETRYDATA_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Objects/ArrayBuffer.hh> // get Attribute definition

namespace ACGL{
namespace OpenGL{

class GeometryData
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    GeometryData(void)
    :   mpData(NULL),
        mSize(0),
        mStrideSize(0)
    {}

    virtual ~GeometryData(void)
    {
        delete[] mpData;
    }

    // ========================================================================================================= \/
    // ================================================================================================= GETTERS \/
    // ========================================================================================================= \/
public:
    GLubyte *getData       (void) const { return mpData;   }
    GLsizei  getSize       (void) const { return mSize;   }
    GLsizei  getStrideSize (void) const { return mStrideSize;  }

    // ========================================================================================================= \/
    // ================================================================================================= SETTERS \/
    // ========================================================================================================= \/
public:
    void setData      (GLubyte *_pData)      { mpData   = _pData;  }
    void setSize      (GLsizei  _size)       { mSize    = _size;  }
    void setStrideSize(GLsizei  _strideSize) { mStrideSize = _strideSize; }

    // ========================================================================================================= \/
    // ================================================================================================== FIELDS \/
    // ========================================================================================================= \/
private:
    GLubyte *mpData;      // raw data, just cast the pointer as needed
    GLsizei  mSize;       // size in bytes
    GLsizei  mStrideSize; // size in bytes of a stride

public:
    ArrayBuffer::AttributeVec mAttributes;
};

ACGL_SMARTPOINTER_TYPEDEFS(GeometryData)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_DATA_GEOMETRYDATA_HH
