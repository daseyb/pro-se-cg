/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Utils/Memory.hh>


size_t ACGL::Utils::pointerAlignment( const void* _pointer ) {
    size_t address = (size_t) _pointer;

    for (int i = 6; i > 0; --i) {
        size_t alignment = 2<<i;
        if (address % alignment == 0) return alignment;
    }

    return 1;
}
