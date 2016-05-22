/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_MATH_CONSTANTS_HH
#define ACGL_MATH_CONSTANTS_HH

/*
 * Some mathmatical constants, for example readable degree to rad conversion.
 *
 * DON'T INCLUDE THIS DIRECTLY! Include <ACGL/Math.hh> instead!
 */
#ifndef ACGL_MATH_HH
#warning "Include Math.hh instead of Constants.hh directly"
#endif

#include <ACGL/ACGL.hh>

#include <cmath>
#include <limits>

#ifndef M_PI
// M_PI is not defined on e.g. VS2010 (isn't part of the standart), so in that case it gets defined here
// outside of the namespace because some code might expect it to be in math.h which is obviously not in our namespace...
#define M_PI 3.14159265358979323846
#endif

namespace ACGL{
namespace Math{
namespace Constants{

//some important constants
const float   INF_FLOAT  = std::numeric_limits<float>::infinity();
const double  INF_DOUBLE = std::numeric_limits<double>::infinity();
const int_t   INF_INT    = std::numeric_limits<int_t>::infinity();
const short_t INF_SHORT  = std::numeric_limits<short_t>::infinity();

template<typename T> inline T INF(void) { return T(); }
template<> inline float   INF<float>   (void) { return INF_FLOAT; }
template<> inline double  INF<double>  (void) { return INF_DOUBLE; }
template<> inline int_t   INF<int_t>   (void) { return INF_INT; }
template<> inline short_t INF<short_t> (void) { return INF_SHORT; }

//constants to change from degree to radians and back
const float  DEG_TO_RAD_FLOAT = (float)(M_PI / 180.0f);
const double DEG_TO_RAD_DOUBLE = M_PI / 180.0 ;
const float  RAD_TO_DEG_FLOAT = (float)(180.0f / M_PI);
const double RAD_TO_DEG_DOUBLE = 180.0  / M_PI;

template<typename T> inline T DEG_TO_RAD(void) { return T(); }
template<> inline float  DEG_TO_RAD<float> (void) { return DEG_TO_RAD_FLOAT; }
template<> inline double DEG_TO_RAD<double>(void) { return RAD_TO_DEG_DOUBLE; }

template<typename T> inline T RED_TO_DEG(void) { return T(); }
template<> inline float  RED_TO_DEG<float> (void) { return DEG_TO_RAD_FLOAT; }
template<> inline double RED_TO_DEG<double>(void) { return RAD_TO_DEG_DOUBLE; }

} // Constants
} // Math
} // ACGL

#endif // ACGL_MATH_CONSTANTS_HH
