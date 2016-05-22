/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_SINGLETON_HH
#define ACGL_BASE_SINGLETON_HH

/*
 * A very simple Singleton as a template.
 * Not thread save etc.
 */

#include <ACGL/ACGL.hh>

namespace ACGL{
namespace Base{

template<typename CLASS>
class Singleton
{
public:
    virtual ~Singleton(void) {}
    
    static ptr::shared_ptr<CLASS> the(void)
    {
        if(!spInstance)
            spInstance.reset(new CLASS());
        return(spInstance);
    }
    
protected:
    Singleton(void){}
private:
    Singleton(const Singleton&){}
    
private:
    static ptr::shared_ptr<CLASS> spInstance;
    
};

template<typename CLASS>
ptr::shared_ptr<CLASS> Singleton<CLASS>::spInstance = ptr::shared_ptr<CLASS>();

#define ACGL_SINGLETON(Class) \
friend class Base::Singleton< Class >; \
private:\
    Class(const Class& ){ }\
    void operator=(Class& ){ }

} // Base
} // ACGL

#endif
