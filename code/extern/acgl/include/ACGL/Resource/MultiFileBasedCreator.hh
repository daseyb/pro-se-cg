/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Similar to a SingleFileBasedCreator, but this keeps track of multiple files and updates
 * itself if one of them was changed.
 * A special Resourcename can be set as one filename alown can't identify the
 * resource.
 *
 * Classes derived from this have to implement a create() and update() method.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Utils/FileHelpers.hh>

#include <string>
#include <sstream>
#include <algorithm>

namespace ACGL{
namespace Resource{

template<typename RESOURCE>
class MultiFileBasedCreator
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    //! sets the first file and the base path, more files can be added with 'addFile'
    MultiFileBasedCreator(const std::string& _filename, const std::string& _basePath)
    :   mBasePath(_basePath)
    {
        addFile( _filename );
    }
    virtual ~MultiFileBasedCreator(){}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! the default resourcename is the concatenation of the alphabetically sorted filenames
    //! a setter for a user-defined resourcename should be defined in derived classes
    virtual std::string getResourceName(void) const
    {
        if (mResourceName == "") {
            std::vector<std::string> namesAlpha = mFileNames;
            std::sort(namesAlpha.begin(), namesAlpha.end());

            std::stringstream sstream;

            for (unsigned int i = 0; i < namesAlpha.size(); ++i) {
                sstream << namesAlpha[i];
            }

            return sstream.str();
        }
        return mResourceName;
    }

    //! this should create and return the resource
    virtual ptr::shared_ptr<RESOURCE> create() = 0;

    //! this should update the resource and return true if successful
    virtual bool update(ptr::shared_ptr<RESOURCE>&) = 0;

    //! returns true iff _all_ files are up to date
    bool filesAreUpToDate(void) const {
        bool b = true;
        for (unsigned int i = 0; i < mFileNames.size(); ++i) {
            b &= ( Utils::FileHelpers::getFileModificationTime( mBasePath+mFileNames[i] ) == mFileModificationTime[i] );
        }
        return b;
    }

protected:
    //! This constructor does not add any files, use this only on derived types which constructor will add a file itself!
    MultiFileBasedCreator() : mBasePath("") {}

    //! adds another file, this is protected to derived classes can expose specific versions which do more than just remembering one file
    void addFile (const std::string &_fileName) {
        mFileNames.push_back( _fileName );
        mFileModificationTime.push_back( Utils::FileHelpers::FileModificationTime() );
    }

    //! updates _all_ modification times
    void updateFileModificationTimes(void) {
        for (unsigned int i = 0; i < mFileNames.size(); ++i) {
            mFileModificationTime[i] = Utils::FileHelpers::getFileModificationTime( mBasePath+mFileNames[i]);
        }
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    std::vector<std::string> mFileNames;
    std::string mBasePath;
    std::string mResourceName;

    std::vector<Utils::FileHelpers::FileModificationTime> mFileModificationTime;
};

} // Resource
} // ACGL


