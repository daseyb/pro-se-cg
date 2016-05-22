/**
 *
 * Includes the Oculus Rift LibOVR but tries to suppress as much compiler warnings
 * as possible.
 *
 */

#ifdef ACGL_USE_OCULUS_RIFT

/////////////////////////////////////////////////////////////////////////////////////
// ignore compiler warnings from LibOVR:
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
#     pragma GCC diagnostic ignored "-Wstrict-aliasing"
#     pragma GCC diagnostic ignored "-Wattributes"
#     pragma GCC diagnostic ignored "-Wreorder"
#  endif
// gcc:
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
//
/////////////////////////////////////////////////////////////////////////////////////

#if ACGL_RIFT_SDK_VERSION < 40
// used for SDK 0.2.5
#include <OVR.h>
#include <OVRVersion.h>
#else
#include <OVR_Version.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#endif

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


#endif // ACGL_USE_OCULUS_RIFT
