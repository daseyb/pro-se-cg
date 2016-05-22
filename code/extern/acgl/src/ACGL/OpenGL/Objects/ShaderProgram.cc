/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>
#include <ACGL/OpenGL/Tools.hh>

using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

bool ShaderProgram::link() const
{
	// Clear uniform cache
	mUniformLocationCache.clear();

    glLinkProgram(mObjectName);

    // check for program link errors:
    GLint programError;
    glGetProgramiv(mObjectName, GL_LINK_STATUS, &programError);

    if (programError != GL_TRUE)
    {
        // yes, errors :-(
        error() << "Program could not get linked:" << std::endl;
    }

    GLsizei length = 0;
    glGetProgramiv(mObjectName, GL_INFO_LOG_LENGTH, &length);
    if (length > 1)
    {
        // error log or warnings:
        GLchar* pInfo = new char[length + 1];
        glGetProgramInfoLog(mObjectName,  length, &length, pInfo);
        warning() << "Linker log: " << std::string(pInfo) << std::endl;
        delete[] pInfo;
        return (programError == GL_TRUE); // if the log contains only warnings we return true
    }
    return true;
}

void ShaderProgram::setAttributeLocations(ConstSharedLocationMappings _locationMappings)
{
    if (!_locationMappings) return;

    bool needsRelink = false;

    // search through all attributes:
    LocationMappings::LocationMap locations = _locationMappings->getLocations();

    for(LocationMappings::LocationMap::iterator it = locations.begin();
        it != locations.end();
        ++it)
    {
        // find out whether a fragment data location with a matching name exists in this shader
        GLint attributeLocation = getAttributeLocation((*it).first);

        if ( attributeLocation  != -1 ) // attributeLocation with that name exists?
        {
            // NOTE: even if the attribute is currently bound correctly already, it might not have
            // been set by bindAttribLocation explititly so the linker might reassign another value
            // during relinking! So set it explititly!
            bindAttributeLocation((*it).first, (*it).second);
            needsRelink = true;
        }
    }

    // re-link the program only if necessary
    if(needsRelink)
    {
        link();
    }
}

#if (ACGL_OPENGL_VERSION >= 30)
void ShaderProgram::setFragmentDataLocations(ConstSharedLocationMappings _locationMappings)
{
    if (!_locationMappings) return;

    bool needsRelink = false;

    // search through all color attachments:
    LocationMappings::LocationMap locations = _locationMappings->getLocations();

    for(LocationMappings::LocationMap::iterator it = locations.begin();
        it != locations.end();
        ++it)
    {
        // find out whether a fragment data location with a matching name exists in this shader
        GLint fragmentDataLocation = getFragmentDataLocation((*it).first);

        if (fragmentDataLocation  != -1) // fragment data location with that name exists?
        {
            bindFragmentDataLocation((*it).first, (*it).second);
            needsRelink = true;
        }
    }

    // re-link the program only if necessary
    if(needsRelink)
    {
        link();
    }
}
#endif // (ACGL_OPENGL_VERSION >= 30)

SharedLocationMappings ShaderProgram::getAttributeLocations() const
{
    SharedLocationMappings locationMap = SharedLocationMappings( new LocationMappings() );

    // query the number of _active_ attributes:
    GLint attributeCount;
    glGetProgramiv( mObjectName, GL_ACTIVE_ATTRIBUTES, &attributeCount );
    if (attributeCount == 0) return locationMap;

    // reserve a string long enought for the longest name:
    GLint longestAttributeName;
    glGetProgramiv( mObjectName, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,  &longestAttributeName );

    if (longestAttributeName == 0) return locationMap; // work around an old nvidia bug.

    char *name = new char[longestAttributeName+1];

    // get the name and location of each active attribute:
    for (int i = 0; i < attributeCount; ++i) {
        GLenum  type;
        GLint   size;
        GLsizei length;
        glGetActiveAttrib( mObjectName, i, longestAttributeName, &length, &size, &type, name );
        name[ length+1 ] = 0; // null terminate string

        GLint attribLocation = glGetAttribLocation( mObjectName, name );

        // if the attribute name starts with "gl_", i.e. is a reserved attribute name, ignore it
        if(length >= 3 && strncmp(name, "gl_", 3) == 0)
            continue;

        locationMap->setLocation( std::string(name), (GLuint) attribLocation );
    }

    delete[] name;
    return locationMap;
}

#if (ACGL_OPENGL_VERSION >= 30)
SharedLocationMappings ShaderProgram::getFragmentDataLocations()
{
    SharedLocationMappings locationMap = SharedLocationMappings( new LocationMappings() );

    ACGL::Utils::error() << " ShaderProgram::getFragmentDataLocations is not implemented -> missing OpenGL API" << std::endl;

    return locationMap;
}
#endif //(ACGL_OPENGL_VERSION >= 30)

#if (ACGL_OPENGL_VERSION >= 31)
SharedLocationMappings ShaderProgram::getUniformOffsetsOfBlock( GLuint _blockIndex ) const
{
    SharedLocationMappings locationMap = SharedLocationMappings( new LocationMappings() );

    if (_blockIndex == GL_INVALID_INDEX) return locationMap; // block does not exist

    // query the number of _active_ uniforms:
    GLint uniformCount;
    glGetProgramiv( mObjectName, GL_ACTIVE_UNIFORMS, &uniformCount );
    if (uniformCount == 0) return locationMap;

    // reserve a string long enought for the longest name:
    GLint longestUniformName;
    glGetProgramiv( mObjectName, GL_ACTIVE_UNIFORM_MAX_LENGTH,  &longestUniformName );
    char *name = new char[longestUniformName+1];

    // get the name and location of each active attribute:
    for (int i = 0; i < uniformCount; ++i) {
        GLsizei length;
        glGetActiveUniformName( mObjectName, i, longestUniformName, &length, name );

        name[ length+1 ] = 0; // null terminate string

        GLuint idx = i;
        GLint uniformBlockIndex;
        glGetActiveUniformsiv( mObjectName, 1, &idx, GL_UNIFORM_BLOCK_INDEX, &uniformBlockIndex );

        if (uniformBlockIndex != -1) {
            if ((GLuint)uniformBlockIndex == _blockIndex) {
                GLint offset;
                glGetActiveUniformsiv( mObjectName, 1, &idx, GL_UNIFORM_OFFSET, &offset );
                //ACGL::Utils::message() << "uniform " << i << " is " << name << " block: " << uniformBlockIndex << " offset: " << offset << std::endl;
                locationMap->setLocation( std::string(name), (GLuint) offset );
            }
        }
    }

    delete[] name;
    return locationMap;
}

GLsizeiptr ShaderProgram::getUniformBlockSize( GLuint _blockIndex ) const
{
    if (_blockIndex == GL_INVALID_INDEX) return 0; // block does not exist

    GLint uniformBlockSize;
    glGetActiveUniformBlockiv( mObjectName, _blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

    return (GLsizeiptr) uniformBlockSize;
}
#endif // OpenGL 3.1
