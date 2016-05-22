/***********************************************************************
 * Copyright 2015-2015 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

// Android autodetection:
#ifdef __ANDROID__
#   define ACGL_PLATFORM_ANDROID
#endif

// If we're compiling for an Apple system we need this to distinquish between Mac and iOS:
#ifdef __APPLE__
#   include <TargetConditionals.h>
#endif

#if (defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR))
#   if (TARGET_OS_IPHONE == 1)
#       define ACGL_PLATFORM_IOS
#   endif
#endif
