/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/**
 * Creates a VAO from a file which contains a mesh, e.g. an *.obj file.
 *
 *    SharedVertexArrayObject vaoBunny = VertexArrayObjectCreator("Bunny.obj").create();
 *
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Resource/SingleFileBasedCreator.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Data/LocationMappings.hh>
#include <ACGL/Base/Settings.hh>

#if defined( ACGL_OPENGL_SUPPORTS_VAO )

namespace ACGL{
namespace OpenGL{

class VertexArrayObjectCreator : public Resource::SingleFileBasedCreator<VertexArrayObject>
{
    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    VertexArrayObjectCreator(const std::string& _filename);
    VertexArrayObjectCreator(const char* _filename);

    VertexArrayObjectCreator& setResourceName(const std::string &_resourceName) { mResourceName = _resourceName; return *this; }

    template<typename CONTROLLER>
    VertexArrayObjectCreator(const CONTROLLER& _fileController)
    :   Resource::SingleFileBasedCreator<VertexArrayObject>(_fileController.getFilename(), Base::Settings::the()->getFullGeometryPath()),
        mAttributeLocations()
    {}
    virtual ~VertexArrayObjectCreator() {}

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! links to an external LocationMappings object
    inline VertexArrayObjectCreator& externAttributeLocations(const SharedLocationMappings& _attributeLocations) { mAttributeLocations = _attributeLocations; return *this; }

    //! sets the caching mode for this creator 
    inline VertexArrayObjectCreator& cached(bool _cacheMode) { mCaching = _cacheMode; return *this; }

private:
    bool load(SharedVertexArrayObject& vao);

    // ===================================================================================================== \/
    // ============================================================================================ OVERRIDE \/
    // ===================================================================================================== \/
public:
    virtual SharedVertexArrayObject create();
    virtual bool update(SharedVertexArrayObject& _vao);

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    SharedLocationMappings        mAttributeLocations;
    bool                          mCaching;
};

} // OpenGL
} // ACGL

#endif // #ifdef ACGL_OPENGL_SUPPORTS_VAO
