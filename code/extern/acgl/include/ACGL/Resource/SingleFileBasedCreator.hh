/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Base class for all Creators which create an object based on one single file.
 * File based Creators should always be able to update the object if the file
 * has changed.
 *
 * Classes derived from this have to implement a create() and update() method.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <string>

namespace ACGL{
namespace Resource{

template<typename RESOURCE>
class SingleFileBasedCreator
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    SingleFileBasedCreator(const std::string& _filename, const std::string& _basePath = "")
    :   mFilename(_filename),
        mFullFilePath(_basePath + _filename),
        mResourceName(_filename),
        mFileModificationTime()
    {}
    virtual ~SingleFileBasedCreator(){}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    virtual std::string getResourceName(void) const { return mResourceName; }
    const std::string& getFilename(void) const { return mFilename; }
    const std::string& getFullFilePath(void) const { return mFullFilePath; }

    //! this should create and return the resource
    virtual ptr::shared_ptr<RESOURCE> create() = 0;

    //! this should update the resource and return true if successful
    virtual bool update(ptr::shared_ptr<RESOURCE>&) = 0;

protected:
    //! returns true if the file has not changed (based on the modification time)
    inline bool fileIsUpToDate(void) { return Utils::FileHelpers::getFileModificationTime(mFullFilePath) == mFileModificationTime; }
    inline void updateFileModificationTime(void) { mFileModificationTime = Utils::FileHelpers::getFileModificationTime(mFullFilePath); }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    std::string mFilename;
    std::string mFullFilePath;
    std::string mResourceName;

    Utils::FileHelpers::FileModificationTime mFileModificationTime;
};

} // Resource
} // ACGL


