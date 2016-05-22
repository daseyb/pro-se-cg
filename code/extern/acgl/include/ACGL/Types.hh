/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_TYPES_HH
#define ACGL_TYPES_HH

#if (_MSC_VER < 1600 && _MSC_VER >= 1500)
    // stdint.h not shipped with VS 2008
    typedef char              int8_t;
    typedef unsigned char    uint8_t;
    typedef short             int16_t;
    typedef unsigned short   uint16_t;
    typedef int               int32_t;
    typedef unsigned int     uint32_t;
    typedef __int64           int64_t;
    typedef unsigned __int64 uint64_t;
#else
#   include <stdint.h>
#endif

namespace ACGL {

/*
 * Our datatypes have fixed bit width like the C99 types from stdint,
 * ours are just nicer to read ;-)
 */
typedef int8_t   byte_t;
typedef uint8_t  ubyte_t;
typedef int16_t  short_t;
typedef uint16_t ushort_t;
typedef int32_t  int_t;
typedef uint32_t uint_t;
typedef int64_t  long_t;
typedef uint64_t ulong_t;

} // ACGL

#endif // ACGL_TYPES_HH
