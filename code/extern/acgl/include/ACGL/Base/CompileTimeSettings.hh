/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_COMPILETIMESETTINGS_HH
#define ACGL_BASE_COMPILETIMESETTINGS_HH

/*
 * OpenGL error checking defines
 *
 * By default, only critical errors will get checked in release build
 * and critical&common in debug build.
 *
 * This can get overwritten with a compile flag define:
 *  -DACGL_ERROR_LEVEL_EC0
 *    No error checking at all
 *  -DACGL_ERROR_LEVEL_EC1
 *    Places will get checked where errors like out of memory can occur.
 *    This will detect runtime errors which can happen even if the program
 *    is bug free.
 *    Switching this on shouldn't be a performance hit.
 *  -DACGL_ERROR_LEVEL_EC2
 *    Problems that happen more often while developing like typos in uniform
 *    names. All placed where strings are used to call OpenGL objects etc.
 *  -DACGL_ERROR_LEVEL_EC3
 *    Most likely bugs in ACGL itself or wrong usage. If there are strange errors
 *    switch this on to find the reason more quickly. Even normal development
 *    should work without this. Results in a lot of glGetError!
 *
 * Note that all error levels also include the higher ones (COMMON also adds CRITICAL).
 */

// try to detect a debug build:
#if defined( DEBUG ) || defined (_DEBUG)
#define ACGL_DEBUG
#endif

/*
 * Map CMake generated error-level defines to internally used, more readable defines
 */
#ifdef ACGL_ERROR_LEVEL_EC0
#   define ACGL_CHECK_NO_GL_ERRORS
#endif
#ifdef ACGL_ERROR_LEVEL_EC1
#   define ACGL_CHECK_CRITICAL_GL_ERRORS
#endif
#ifdef ACGL_ERROR_LEVEL_EC2
#   define ACGL_CHECK_COMMON_GL_ERRORS
#endif
#ifdef ACGL_ERROR_LEVEL_EC3
#   define ACGL_CHECK_RARE_GL_ERRORS
#endif


#ifndef ACGL_CHECK_NO_GL_ERRORS
# ifndef ACGL_CHECK_CRITICAL_GL_ERRORS
#  ifndef ACGL_CHECK_COMMON_GL_ERRORS
#   ifndef ACGL_CHECK_RARE_GL_ERRORS
//   if nothing is defined, use defaults:
#    ifdef ACGL_DEBUG
#     define ACGL_CHECK_RARE_GL_ERRORS
#    else
#     define ACGL_CHECK_CRITICAL_GL_ERRORS
#    endif
#   endif
#  endif
# endif
#endif


// if rare is set, set also common:
#ifdef ACGL_CHECK_RARE_GL_ERRORS
# ifndef ACGL_CHECK_COMMON_GL_ERRORS
#  define ACGL_CHECK_COMMON_GL_ERRORS
# endif
#endif

// if common is set, set also critical:
#ifdef ACGL_CHECK_COMMON_GL_ERRORS
# ifndef ACGL_CHECK_CRITICAL_GL_ERRORS
#  define ACGL_CHECK_CRITICAL_GL_ERRORS
# endif
#endif

// if critical is set, NO must not be set:
#ifdef ACGL_CHECK_CRITICAL_GL_ERRORS
# ifdef ACGL_CHECK_NO_GL_ERRORS
#  undef ACGL_CHECK_NO_GL_ERRORS
# endif
#endif



#endif // ACGL_BASE_COMPILETIMESETTINGS_HH

