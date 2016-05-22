/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/ArrayBufferLoadStore.hh>
#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace ACGL{
namespace OpenGL{

SharedArrayBuffer loadArrayBuffer(const std::string &_filename)
{
    ACGL::OpenGL::SharedGeometryData data = loadGeometryData(_filename);
    if(data)
    {
        SharedArrayBuffer ab(new ArrayBuffer);
        ab->setGeometryData(data);
        return ab;
    }
    else
    {
        return SharedArrayBuffer();
    }
}

SharedArrayBuffer loadArrayBufferFromATB(const std::string& _filename, const std::string& _attributeName)
{
    ACGL::OpenGL::SharedGeometryData data = loadGeometryDataFromATB(_filename, _attributeName);
    if(data)
    {
        SharedArrayBuffer ab(new ArrayBuffer);
        ab->setGeometryData(data);
        return ab;
    }
    else
    {
        return SharedArrayBuffer();
    }
}

}
}
