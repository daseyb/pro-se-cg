/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_BASE_FRAMEWORKSETTINGS_HH
#define ACGL_BASE_FRAMEWORKSETTINGS_HH

/*
 * This class manages a few runtime settings for the application which can change the
 * behavior of the ACGL library.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Singleton.hh>
#include <string>

namespace ACGL{
namespace Base{

class Settings : public Singleton<Settings>
{
    friend class Singleton<Settings>;
    
    // ==================================================================================================== \/
    // ========================================================================= Constructors / Destructors \/
    // ==================================================================================================== \/
protected:
    //! Constructor is protected => singleton.
    Settings()
    :   mResourcePath (""),
        mTexturePath  (""),
        mGeometryPath (""),
        mShaderPath   ("")
    {
    }
        
public:
    ~Settings(){}
    
    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    inline void setResourcePath (const std::string &_path) { mResourcePath = _path; }
    inline void setTexturePath  (const std::string &_path) { mTexturePath  = _path; }
    inline void setGeometryPath (const std::string &_path) { mGeometryPath = _path; }
    inline void setShaderPath   (const std::string &_path) { mShaderPath   = _path; }

    inline const std::string& getResourcePath() const { return mResourcePath; }
    inline const std::string& getTexturePath()  const { return mTexturePath;  }
    inline const std::string& getGeometryPath() const { return mGeometryPath; }
    inline const std::string& getShaderPath()   const { return mShaderPath;   }

    inline const std::string getFullTexturePath()  const { return mResourcePath + mTexturePath;  }
    inline const std::string getFullGeometryPath() const { return mResourcePath + mGeometryPath; }
    inline const std::string getFullShaderPath()   const { return mResourcePath + mShaderPath;   }
    
    
    // ==================================================================================================== \/
    // ============================================================================================= FIELDS \/
    // ==================================================================================================== \/
private:
    std::string mResourcePath;
    std::string mTexturePath;
    std::string mGeometryPath;
    std::string mShaderPath;
};

} // Base
} // ACGL

#endif // FRAMEWORKSETTINGS_HH
