/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_LOCATIONMAPPINGS_HH
#define ACGL_OPENGL_OBJECTS_LOCATIONMAPPINGS_HH

/**
 * LocationMappings is a map from strings to GLuints that stores the mappings from
 *
 *    attribute names  to  attribute locations
 *                     or
 *    fragment outputs to  fragdata locations
 *
 * (as long as there are no name clashes one map can be used for both)
 *
 * Another use is to query
 *    uniform names    to  uniform buffer offsets
 *
 * A mapping like this can be used to init all mappings of multiple ShaderPrograms
 * in the same way to they can be used with the same VAOs or FBOs. Similar, these
 * mapping objects can be used to configute VAOs and FBOs!
 *
 * To fully automate the mappings in a program the creation of these mappings can
 * be done automatically by parsing shader sources, querying locations and names from
 * ShaderPrograms or any other way that best suits the use case of the application.
 */

#include <map>
#include <string>

#include <ACGL/ACGL.hh>
#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

class LocationMappings
{
    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::map< std::string, GLuint > LocationMap;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    LocationMappings() {}
    ~LocationMappings() {}

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    //! Returns the stored location for a given name or -1 if the name could not get found (similar to how GL behaves)
    GLint getLocation(const std::string& _name) const;

    //! Returns the raw location map:
    const LocationMap& getLocations() const { return mMappings; }

    inline size_t getSize() { return mMappings.size(); }

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/
public:

    //! Adds one location, if the name already exists, the location number gets changed:
    void setLocation(const std::string& _name, GLuint _location);

    //! Adds one location, uses the next free integer as the location
    //! this way the locations can get the number of the order they were added (if only this function gets used)
    void setLocation(const std::string& _name);

    //! Adds all given locations via setLocation:
    void setLocations( const ptr::shared_ptr<LocationMappings> &_other );

    //! Adds one location, if the name is new! (otherwise keep the old one)
    //! If the name is new and the location is free, use that, otherwise use a free location
    //! Useful for merging -> uses as much info from the new location WITHOUT destroying and old information!
    void addLocation( const std::string& _name, GLuint _location );

    //! Adds all given locations via addLocation:
    void addLocations( const ptr::shared_ptr<LocationMappings> &_other );

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! Tells whether a mapping for a given name exists
    inline bool exists(const std::string& _name) const { return (getLocation(_name) != -1); }

    void printMapping();

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    //! returns the first unused location number
    GLuint getFreeLocation();
    LocationMap mMappings;
};
ACGL_SMARTPOINTER_TYPEDEFS(LocationMappings)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_LOCATIONMAPPINGS_HH
