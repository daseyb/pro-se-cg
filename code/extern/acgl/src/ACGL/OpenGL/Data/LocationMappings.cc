/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/LocationMappings.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;

GLint LocationMappings::getLocation(const std::string& _name) const
{
    LocationMap::const_iterator location = mMappings.find(_name);
    if (location == mMappings.end())
    {
        return (GLint) -1;
    }
    else
    {
        return (GLint) location->second;
    }
}

void LocationMappings::setLocation(const std::string& _name, GLuint _location)
{
    LocationMap::const_iterator location = mMappings.find(_name);
    if (location != mMappings.end() && (location->second != _location))
    {
        ACGL::Utils::warning() << "LocationMappings: Overwriting location mapping for " << _name;
        ACGL::Utils::warning() << " (previous value: " << mMappings[_name] << ", new value: " << _location<< ")" << std::endl;
    }
    //ACGL::Utils::debug() << "setLocation: " << _name << " value: " << _location<< "" << std::endl;
    mMappings[_name] = _location;
}

void LocationMappings::setLocation(const std::string& _name)
{
    LocationMap::const_iterator end = mMappings.end();
    GLuint nextFreeLocation = 0;

    // most likely not the fastest way ;-)
    LocationMap::const_iterator it = mMappings.begin();
    while (it != end) {
        if ( it->second == nextFreeLocation ) {
            nextFreeLocation++;
            it = mMappings.begin();
        } else {
            ++it;
        }
    }

    setLocation( _name, nextFreeLocation );
}

void LocationMappings::setLocations( const SharedLocationMappings &_other )
{
    LocationMap::const_iterator end = _other->mMappings.end();
    for (LocationMap::const_iterator it = _other->mMappings.begin(); it != end; ++it) {
        setLocation( it->first, it->second );
    }
}

void LocationMappings::addLocation( const std::string& _name, GLuint _location )
{
    LocationMap::const_iterator location = mMappings.find(_name);
    if (location == mMappings.end()) {
        // name did not exist jet
        LocationMap::iterator it = mMappings.begin();
        while (it != mMappings.end())
        {
            if (it->second == _location) {
                // but the number is already taken, find a free one:
                mMappings[_name] = getFreeLocation();
                return;
            }
            ++it;
        }
        // new name, new location, just add it:
        mMappings[_name] = _location;
        return;
    }
    // name is taken, ignore the new location, keep the old one
}


void LocationMappings::addLocations( const SharedLocationMappings &_other )
{
    LocationMap::const_iterator end = _other->mMappings.end();
    for (LocationMap::const_iterator it = _other->mMappings.begin(); it != end; ++it) {
        addLocation( it->first, it->second );
    }
}

GLuint LocationMappings::getFreeLocation()
{
    LocationMap::iterator it = mMappings.begin();
    GLuint unused = 0;
    while (true) {
        while (it != mMappings.end())
        {
            if (it->second == unused) {
                // test next number from the beginning of the map:
                unused++;
                it = mMappings.begin();
            } else {
                ++it;
            }
        }
        // this number was not found in the map:
        return unused;
    }
}

void LocationMappings::printMapping()
{
    LocationMap::const_iterator end = mMappings.end();
    for (LocationMap::const_iterator it = mMappings.begin(); it != end; ++it) {
        ACGL::Utils::debug() << "loc mapping: " << it->first << " - " << it->second << std::endl;
    }
}
