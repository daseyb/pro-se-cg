/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;
using namespace ACGL::Utils::StringHelpers;

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               generic load/save
///////////////////////////////////////////////////////////////////////////////////////////////////

SharedGeometryData loadGeometryData(const std::string& _filename)
{
    // lower case file ending:
    std::string fileEnding = getFileEnding(_filename);

    if(fileEnding == "obj")
    {
        return loadGeometryDataFromOBJ(_filename);
    }
    else if(fileEnding == "atb")
    {
        return loadGeometryDataFromATB(_filename);
    }
    else if(fileEnding == "vap")
    {
        return loadGeometryDataFromVAP(_filename);
    }
    else
    {
        error() << "geometry file format of " << _filename << " not supported" << std::endl;
    }

    return SharedGeometryData();
}

} // OpenGL
} // ACGL
