/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>

#include <string>

namespace ACGL{
namespace OpenGL{

//! Loads an ArrayBuffer from the given file (e.g. a *.obj file).
//! Often not an ArrayBuffer but a complete VAO is needed, in that case,
//! see VertexArrayObjectLoadStore.hh !
SharedArrayBuffer loadArrayBuffer(const std::string& _filename);

//! Loads an ArrayBuffer from the given ATB (attribute) file.
//! An attribute name can be specified, otherwise it is guessed from the filename.
SharedArrayBuffer loadArrayBufferFromATB(const std::string& _filename, const std::string& _attributeName = "");

}
}
