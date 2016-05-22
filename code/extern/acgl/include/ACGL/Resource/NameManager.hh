/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_RESOURCE_NAMEMANAGER_HH
#define ACGL_RESOURCE_NAMEMANAGER_HH

/**
 * A NameManager is basically a string-to-resource map implemented as a singleton. This way
 * resources can be referenced via a string and accessed from the whole progam. While this is
 * not very efficient or well organized, it can help the prototyping process.
 *
 * To use one, create a NameManager for the resource:
 *
 *    typedef Resource::NameManager<Foo> FooNameManager;
 *
 * Then add your resources:
 *
 *    SharedFoo foo = ...;
 *    FooNameManager::the()->add( "nameOfFoo", foo );
 *
 * And later query the resource:
 *
 *    SharedFoo foo = FooNameManager::the()->query( "nameOfFoo" );
 *
 *
 * Resources for which a Creator exists (especially such with can update themself)
 * can also be managed by the FileManager / MultiFileManager!
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Singleton.hh>

#include <map>
#include <string>

namespace ACGL{
namespace Resource{

//! NameManager templte, create your own version with a typedef (see above).
template<typename RESOURCE>
class NameManager : public Base::Singleton< NameManager<RESOURCE> >
{
    friend class Base::Singleton< NameManager<RESOURCE> >;

public:
    //! for the resource type, a shared-pointer based version gets defined:
    typedef ptr::shared_ptr<RESOURCE> SharedResource;
private:
    //! better readyble name for the map:
    typedef std::map<std::string, SharedResource> ResourceMap;
public:
    
    virtual ~NameManager() {}

    //! returns a shared pointer to the resource if it's stored in the map, NULL otherwise!
    SharedResource query(const std::string& _key);

    //! test the existance of a resource:
    bool exists(const std::string& _key);

    //! delete one specific resource:
    bool deleteResource(const std::string& key);

    //! delete all resources:
    void clearAllResources();

    //! add one resource, if one is already presend with that name, replace it:
    void add( const std::string& _key, SharedResource _resource );

    typename ResourceMap::const_iterator begin() const { return mResourceMap.begin(); }
    typename ResourceMap::const_iterator end()   const { return mResourceMap.end(); }
    
protected:
    NameManager()
    :   mResourceMap()
    {}
private:
    NameManager(const NameManager&) {}
    
private:
    ResourceMap mResourceMap;
};

template<typename RESOURCE>
typename NameManager<RESOURCE>::SharedResource NameManager<RESOURCE>::query(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return existingResource->second;
    return SharedResource();
}

template<typename RESOURCE>
bool NameManager<RESOURCE>::exists(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return true;
    return false;
}

template<typename RESOURCE>
bool NameManager<RESOURCE>::deleteResource(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
    {
        mResourceMap.erase(existingResource);
        Utils::debug() << "NameManager::Resource deleted: " << _key << std::endl;
        return true;
    }
    Utils::warning() << "NameManager::Resource not found for deletion! " << std::endl;
    return false;
}

template<typename RESOURCE>
void NameManager<RESOURCE>::clearAllResources()
{
    mResourceMap.clear();
}

template<typename RESOURCE>
void NameManager<RESOURCE>::add( const std::string& _key, typename NameManager<RESOURCE>::SharedResource _resource )
{
    mResourceMap[_key] = _resource;
}

} // Resource
} // ACGL

#endif // ACGL_RESOURCE_NAMEMANAGER_HH
