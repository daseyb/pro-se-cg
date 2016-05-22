/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_RESOURCE_FILEMANAGER_HH
#define ACGL_RESOURCE_FILEMANAGER_HH

/**
 * The FileManager and MultiFileManager are like the NameManager Singleton-
 * maps to access resources from everywhere by calling them by name.
 * In addition to the NameManager, these containers also feature the
 * functionality to reload all contained resources. For this it is important
 * that in addition to the resource and the name a Creator is also provided.
 *
 * Note that changes to the objects returned by these managers might get
 * destroyed in case the resource gets reloaded. The update() function
 * of the Creator should as far as possible transfer the state from the
 * old object to the new one, but this might not always work.
 * Note that declaring the object as const isn't always helpful as well
 * as for OpenGL objects the state often gets altered (e.g. uniforms are being set)
 * and disalowing the change of OpenGL states is not useful.
 *
 * To use one, create a FileManager for the resource:
 *
 *    typedef Resource::FileManager<Foo> FooFileManager;
 *
 * Then add your resources:
 *
 *    SharedFoo foo = FooNameManager::the()->get( FooCreator("filename").option1().option2() );
 *
 * And later query the resource:
 *
 *    SharedFoo foo = FooNameManager::the()->query( "filename" );
 *
 * If get() gets called more than once for the same resource, only one copy gets created.
 *
 * Per default the resource name is the filename (FileManager) or the concatenated list of
 * alphabetically ordered filenames (MultiFileManager).
 * The resource name can get explicitly set by (Multi)FileBasedCreator.setResourceName().
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Singleton.hh>
#include <ACGL/Base/Macros.hh>
#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/Resource/MultiFileBasedCreator.hh>

#include <map>
#include <string>

namespace ACGL{
namespace Resource{

template<typename RESOURCE>
class FileManager : public Base::Singleton< FileManager<RESOURCE> >
{
    ACGL_SINGLETON(FileManager<RESOURCE>)
    
public:
    typedef ptr::shared_ptr< SingleFileBasedCreator<RESOURCE> > SharedController;
    ACGL_SMARTPOINTER_TYPEDEFS(RESOURCE)
    struct Resource
    {
        SharedController controller;
        SharedRESOURCE resource;
    };

    typedef std::map<std::string, Resource> ResourceMap;
    
    virtual ~FileManager() {}

    //! returns the resource and creates it if it isn't created yet.
    template<typename CONTROLLER>
    SharedRESOURCE get(const CONTROLLER& _controller);

    //! returns the resource by name (or NULL if it isn't stored)
    SharedRESOURCE query(const std::string &_filename);

    //! test the existance of a resource
    bool exists(const std::string &_key);
    bool erase(const std::string &_key);
    void eraseAll();

    //! update a specific resource
    bool update(const std::string& key);

    //! update all resources - will only update the resources if the file modification time has changed
    void updateAll(void);

    typename ResourceMap::const_iterator begin(void) const { return mResourceMap.begin(); }
    typename ResourceMap::const_iterator end(void) const { return mResourceMap.end(); }
    
protected:
    FileManager(void)
    :   mResourceMap()
    {}
    
private:
    ResourceMap mResourceMap;
};



template<typename RESOURCE> template<typename CONTROLLER>
typename FileManager<RESOURCE>::SharedRESOURCE FileManager<RESOURCE>::get(const CONTROLLER &_controller)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_controller.getResourceName());
    if(existingResource != mResourceMap.end())
        return existingResource->second.resource;

    SharedController pController(new CONTROLLER(_controller));
    SharedRESOURCE pResource = pController->create();
    if(pResource)
    {
        Resource resource = { pController, pResource };
        mResourceMap[_controller.getResourceName()] = resource;
        //Utils::debug() << "FileManager::getResource: Resource loaded: " << _controller.getResourceName() << std::endl;
        return pResource;
    }
    else
    {
        Utils::error() << "FileManager::getResource: Resource could not be loaded: " << _controller.getResourceName() << std::endl;
    }
    return SharedRESOURCE();
}

template<typename RESOURCE>
typename FileManager<RESOURCE>::SharedRESOURCE FileManager<RESOURCE>::query(const std::string &_key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return existingResource->second.resource;
    return ConstSharedRESOURCE();
}

template<typename RESOURCE>
bool FileManager<RESOURCE>::exists(const std::string &_key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return true;
    return false;
}

template<typename RESOURCE>
bool FileManager<RESOURCE>::erase(const std::string &_key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
    {
        mResourceMap.erase(existingResource);
        Utils::debug() << "FileManager::Resource deleted: " << _key << std::endl;
        return true;
    }
    Utils::warning() << "FileManager::Resource not found for deletion! " << std::endl;
    return false;
}

template<typename RESOURCE>
void FileManager<RESOURCE>::eraseAll()
{
    mResourceMap.clear();
}

template<typename RESOURCE>
bool FileManager<RESOURCE>::update(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return existingResource->second.controller->update(existingResource->second.resource);
    return false;
}

template<typename RESOURCE>
void FileManager<RESOURCE>::updateAll(void)
{
    for(typename ResourceMap::iterator i = mResourceMap.begin();
        i != mResourceMap.end();
        ++i)
        i->second.controller->update(i->second.resource);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: merge FileManager & MultiFileManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RESOURCE>
class MultiFileManager : public Base::Singleton< MultiFileManager<RESOURCE> >
{
    ACGL_SINGLETON(MultiFileManager<RESOURCE>)

public:
    typedef ptr::shared_ptr< MultiFileBasedCreator<RESOURCE> > SharedController;
    ACGL_SMARTPOINTER_TYPEDEFS(RESOURCE)
    struct Resource
    {
        SharedController controller;
        SharedRESOURCE resource;
    };

    typedef std::map<std::string, Resource> ResourceMap;

    virtual ~MultiFileManager(void) {}

    template<typename CONTROLLER>
    SharedRESOURCE get(const CONTROLLER& _controller);
    SharedRESOURCE query(const std::string& _filename);
    bool exists(const std::string& _key);
    bool erase(const std::string& key);
    void eraseAll(void);
    bool update(const std::string& key);
    void updateAll(void);

    typename ResourceMap::const_iterator begin(void) const { return mResourceMap.begin(); }
    typename ResourceMap::const_iterator end(void) const { return mResourceMap.end(); }

protected:
    MultiFileManager(void)
    :   mResourceMap()
    {}

private:
    ResourceMap mResourceMap;
};

template<typename RESOURCE> template<typename CONTROLLER>
typename MultiFileManager<RESOURCE>::SharedRESOURCE MultiFileManager<RESOURCE>::get(const CONTROLLER& _controller)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_controller.getResourceName());
    if(existingResource != mResourceMap.end())
        return existingResource->second.resource;

    SharedController pController(new CONTROLLER(_controller));
    SharedRESOURCE pResource = pController->create();
    if(pResource)
    {
        Resource resource = { pController, pResource };
        mResourceMap[_controller.getResourceName()] = resource;
        //Utils::debug() << "FileManager::getResource: Resource loaded: " << _controller.getResourceName() << std::endl;
        return pResource;
    }
    else
    {
        Utils::error() << "FileManager::getResource: Resource could not be loaded: " << _controller.getResourceName() << std::endl;
    }
    return SharedRESOURCE();
}

template<typename RESOURCE>
typename MultiFileManager<RESOURCE>::SharedRESOURCE MultiFileManager<RESOURCE>::query(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return existingResource->second.resource;
    return ConstSharedRESOURCE();
}

template<typename RESOURCE>
bool MultiFileManager<RESOURCE>::exists(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return true;
    return false;
}

template<typename RESOURCE>
bool MultiFileManager<RESOURCE>::erase(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
    {
        mResourceMap.erase(existingResource);
        Utils::debug() << "FileManager::Resource deleted: " << _key << std::endl;
        return true;
    }
    Utils::warning() << "FileManager::Resource not found for deletion! " << std::endl;
    return false;
}

template<typename RESOURCE>
void MultiFileManager<RESOURCE>::eraseAll(void)
{
    mResourceMap.clear();
}

template<typename RESOURCE>
bool MultiFileManager<RESOURCE>::update(const std::string& _key)
{
    typename ResourceMap::iterator existingResource = mResourceMap.find(_key);
    if(existingResource != mResourceMap.end())
        return existingResource->second.controller->update(existingResource->second.resource);
    return false;
}

template<typename RESOURCE>
void MultiFileManager<RESOURCE>::updateAll(void)
{
    for(typename ResourceMap::iterator i = mResourceMap.begin();
        i != mResourceMap.end();
        ++i)
        i->second.controller->update(i->second.resource);
}

} // Resource
} // ACGL

#endif // ACGL_RESOURCE_NAMEMANAGER_HH
