/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>

namespace ACGL{
namespace Utils{

//! returns the alignment of the pointer, e.g. 4 if the pointer is 4-byte aligned
//! maximal alignment returned is 64 byte (might get increased in the future), minimum 1
size_t pointerAlignment( const void* _pointer );

}
}
