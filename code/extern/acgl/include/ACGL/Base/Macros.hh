/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_MACROS_HH
#define ACGL_BASE_MACROS_HH

#ifndef ACGL_ACGL_HH
#  error "Don't include Macros.hh directly, include ACGL.hh"
// some defines below will be set by ACGL.hh!
#endif

//Macro to make a class not copyable
#define ACGL_NOT_COPYABLE(Class) \
private:\
    Class(const Class& ){ }\
    void operator=(Class& ){ }


// creates typedefs for a given class for each smartpointer type
# if (ACGL_UNIQUE_POINTER_SUPPORTED == 1)
    // C++11:
#   define ACGL_SMARTPOINTER_TYPEDEFS(Class) \
    typedef ptr::shared_ptr<Class> Shared ## Class; \
    typedef ptr::shared_ptr<const Class> ConstShared ## Class; \
    typedef ptr::weak_ptr<Class> Weak ## Class; \
    typedef ptr::weak_ptr<const Class> ConstWeak ## Class; \
	typedef ptr::unique_ptr<Class> Unique ## Class; \
    typedef ptr::unique_ptr<const Class> ConstUnique ## Class;
#else
    // TR1 does not have unique pointers
#   define ACGL_SMARTPOINTER_TYPEDEFS(Class) \
    typedef ptr::shared_ptr<Class> Shared ## Class; \
    typedef ptr::shared_ptr<const Class> ConstShared ## Class; \
    typedef ptr::weak_ptr<Class> Weak ## Class; \
    typedef ptr::weak_ptr<const Class> ConstWeak ## Class;
#endif

#endif // MACROS_HH
