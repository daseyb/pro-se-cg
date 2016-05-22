/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#if defined(ACGL_OPENGL_SUPPORTS_VAO)

#include <string>

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               generic load/save
///////////////////////////////////////////////////////////////////////////////////////////////////

//! @brief
//! Loads geometry data from a file and attaches the loaded array buffer data to a VertexArrayObject
//! Tries to guess the file format from the file extension
//! @param _filename The name of the file to load relative to the Geometry load path
//! @param _caching  If true, a disk caching mechanism is used to speed up future reading of the same file.
//!                  When the file is loaded for the first time, a copy of the resulting VAO is stored in
//!                  the binary VAO format next to the original file. Successive loadings of the same file
//!                  will load the cached version if one is present.
SharedVertexArrayObject loadVertexArrayObject(const std::string& _filename, bool _caching = false);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

//! Loads a VertexArrayObject from the ACGL binary VAO format, as specified here:
//! http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/Vao_File_Format
//! A VAO file can contain several named EABs. Only the EAB whose name matches _eabName will be loaded
//! and attached to the VAO. If _eabName is left blank, the first defined EAB will be used.
SharedVertexArrayObject loadVertexArrayObjectFromVAO(const std::string& _filename, const std::string& _eabName = "");

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

//! Stores a VertexArrayObject in a file using the ACGL binary VAO format
//! Each attached ArrayBuffer will be stored entirely in the VAO file but only those attributes which
//! are used by _vao will be defined.
bool saveVertexArrayObjectToVAO(ConstSharedVertexArrayObject _vao, const std::string& _filename);

}
}

#endif // ACGL_OPENGL_SUPPORTS_VAO
