/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_DATA_GEOMETRYDATALOADSTORE_HH
#define ACGL_OPENGL_DATA_GEOMETRYDATALOADSTORE_HH

/**
 * Helper function for writing the contents of a TextureData object into a file
 * and loading them from a file.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Data/GeometryData.hh>

#include <string>
#include <algorithm>

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               generic load/save
///////////////////////////////////////////////////////////////////////////////////////////////////

//! Generic load function that will use one of the loading functions below based on the file ending
SharedGeometryData loadGeometryData(const std::string& _filename);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

//! Loads from a Wavefront OBJ file. If _computeNormals and the mesh had no normals stored,
//! face normals are computed from the geometry
SharedGeometryData loadGeometryDataFromOBJ(const std::string& _filename, bool _computeNormals = true);

//! Loads data from an attribute ATB file. If no _attributeName is specified, it will be guessed
//! from the filename, e.g. /foo/bar/VertexColor.atb --> aVertexColor
SharedGeometryData loadGeometryDataFromATB(const std::string& _filename, const std::string &_attributeName = "");

//! Loads from a VirtualAachen project (VAP) file. If _computeNormals and the mesh had no normals stored,
//! face normals are computed from the geometry
SharedGeometryData loadGeometryDataFromVAP(const std::string& _filename, bool _computeNormals = true);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_DATA_GEOMETRYDATALOADSTORE_HH
