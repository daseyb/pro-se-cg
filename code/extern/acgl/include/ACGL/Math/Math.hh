/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_MATH_HH
#define ACGL_MATH_HH

#include <ACGL/ACGL.hh>

/*
 * For our basic vector math we use the GLM library.
 * This library has the advantage that it was designed to mimic the
 * syntax of GLSL for datatypes (vectors, matrices etc.) as well as functions.
 * It also supports swizzling similar to GLSL.
 *
 * Swizzling has to be defined before the glm.hpp gets first included, no not forget
 * this, you should never include glm yourself, but include always our ACGL/Math.hh!
 */

/////////////////////////////////////////////////////////////////////////////////////
// ignore compiler warnings from GLM:
//
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning ( disable : 4201 )
#pragma warning ( disable : 4100 )
#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )
#endif

#if (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4))
#define COMPILER_IS_GCC_4_6_OR_NEWER
#endif

#ifdef __clang__
//   clang/llvm:
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wuninitialized"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
#  ifdef COMPILER_IS_GCC_4_6_OR_NEWER
//    gcc >= 4.6:
#     pragma GCC diagnostic push
#     pragma GCC diagnostic ignored "-Wtype-limits"
#     pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#     pragma GCC diagnostic ignored "-Wstrict-aliasing"
#  endif
// gcc:
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
//
/////////////////////////////////////////////////////////////////////////////////////

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>


/////////////////////////////////////////////////////////////////////////////////////
// reactivate compiler warnings:
//
#ifdef __clang__
// clang/llvm:
#  pragma clang diagnostic pop
#elif defined COMPILER_IS_GCC_4_6_OR_NEWER
// gcc >= 4.6:
#  pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
//
/////////////////////////////////////////////////////////////////////////////////////

#include <ACGL/Math/Constants.hh>
#include <ACGL/Math/Functions.hh>

#endif // ACGL_MATH_HH
