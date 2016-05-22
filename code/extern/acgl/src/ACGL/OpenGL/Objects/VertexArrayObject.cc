/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Debug.hh>
#if defined (ACGL_OPENGL_SUPPORTS_VAO)

using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

VertexArrayObject::VertexArrayObject( GLenum _mode ) :
                        mpElementArrayBuffer(),
                        mAttributes(),
                        mObjectName(0),
                        mMode(_mode)
{
    glGetError();
    glGenVertexArrays(1, &mObjectName);
    GLint maxAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

    mAttributes.resize( maxAttributes ); // reserve probably 16 slots, the size() can now be used to query the MAX_VERTEX_ATTRIBS
}

#if (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGL_VERSION >= 32))
void VertexArrayObject::setObjectLabel( const std::string &_label )
{
    setObjectLabelT<GL_VERTEX_ARRAY>(getObjectName(),_label);
}

std::string VertexArrayObject::getObjectLabel()
{
    return getObjectLabelT<GL_VERTEX_ARRAY>(getObjectName());
}
#elif (defined(ACGL_OPENGL_DEBUGGER_SUPPORT) && (ACGL_OPENGLES_VERSION >= 10))
void VertexArrayObject::setObjectLabel( const std::string &_label )
{
    setObjectLabelT<GL_VERTEX_ARRAY_OBJECT_EXT>(getObjectName(),_label);
}

std::string VertexArrayObject::getObjectLabel()
{
    return getObjectLabelT<GL_VERTEX_ARRAY_OBJECT_EXT>(getObjectName());
}
#else
void VertexArrayObject::setObjectLabel( const std::string & ) {}
std::string VertexArrayObject::getObjectLabel() { return ""; }
#endif

void VertexArrayObject::bind() const
{
    glBindVertexArray( mObjectName );

#ifdef ACGL_DEBUG
    // check if the EAB actually bound is the same as the one this ACGL object has stored:
    GLint myEAB = 0;
    GLint boundEAB = 0;
    if (mpElementArrayBuffer) {
        myEAB = mpElementArrayBuffer->getObjectName();
    }
    glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEAB );

    if (myEAB != boundEAB) {
        error() << "wrong EAB bound to VAO " << mObjectName << " check EAB header for possible reasons!" << std::endl
                << "trying to correct this..." << std::endl;
        // don't just call attachElementArrayBuffer( mpElementArrayBuffer ); as it will call bind()
        // an we end up in an infinite recursion...
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEAB);
    }
#endif
}

void VertexArrayObject::attachElementArrayBuffer( const ConstSharedElementArrayBuffer& _elementArrayBuffer )
{
    storeOldVAOandBind();

    mpElementArrayBuffer = _elementArrayBuffer;

    if (mpElementArrayBuffer) // could be set to NULL!
    {
        mpElementArrayBuffer->bind();
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    restoreOldVAO();
}

void VertexArrayObject::attachAttribute( const Attribute &_attribute, GLint _location )
{
    if(_attribute.attributeID == -1) {
        ACGL::Utils::error() << "can't attach attribute: invalid attribute ID" << std::endl;
        return;
    }
    else if ( _location >= (GLint)mAttributes.size()) {
        ACGL::Utils::error() << "can't attach attribute " << _attribute.arrayBuffer->getAttributes()[_attribute.attributeID].name
                             << " - maximum number of attributes: " << mAttributes.size() << std::endl;
        return;
    } else if (_location < 0) {
        // find a free location:
        _location = getFreeAttributeLocation();
        if (_location < 0) {
            // no free location found
            ACGL::Utils::error() << "can't attach attribute " << _attribute.arrayBuffer->getAttributes()[_attribute.attributeID].name
                                 << " - maximum number of attributes reached ( " << mAttributes.size() << " )" << std::endl;
            return;
        }
    }

    mAttributes[_location] = _attribute;
    storeOldVAOandBind();
    setAttributePointer( _location );
    restoreOldVAO();
}

GLint VertexArrayObject::getFreeAttributeLocation()
{
    GLint maxAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttributes);

    for (ArrayBuffer::AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
    {
        if ( !(mAttributes[i].arrayBuffer) ) {
            // no shared pointer set, no attribute set!
            return (GLint) i;
        }
    }

    return -1;
}

void VertexArrayObject::attachAttribute( const ConstSharedArrayBuffer& _arrayBuffer,
                             const std::string& _arrayBufferAttributeName,
                             GLint   _attributeLocation )
{
    int32_t arrayBufferAttribute = _arrayBuffer->getAttributeIndexByName(_arrayBufferAttributeName);

    if(arrayBufferAttribute == -1) {
        ACGL::Utils::error() << "can't attach attribute: attribute " << _arrayBufferAttributeName << " not present in array buffer" << std::endl;
        return;
    }

    attachAttribute(_arrayBuffer,
                    arrayBufferAttribute,
                    _attributeLocation);
}

void VertexArrayObject::attachAllAttributes( const ConstSharedArrayBuffer& _arrayBuffer )
{
    ArrayBuffer::AttributeVec attributes = _arrayBuffer->getAttributes();
    for(ArrayBuffer::AttributeVec::size_type i = 0; i < attributes.size(); ++i)
    {
        attachAttribute(_arrayBuffer, (GLint) i, getFreeAttributeLocation() );
    }
}

void VertexArrayObject::detachAttribute( GLuint _location )
{
    if (_location >= mAttributes.size()) {
        ACGL::Utils::error() << "can't detach attribute with location " << _location << " - no such Attribute" << std::endl;
        return;
    }

    storeOldVAOandBind();
    mAttributes[_location].arrayBuffer = SharedArrayBuffer();
    glDisableVertexAttribArray( _location );
    restoreOldVAO();
}

/**
 * Will detach the first found Attribute with the given name.
 */
void VertexArrayObject::detachAttribute( const std::string &_name )
{
    for (AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
    {
        if (mAttributes[i].arrayBuffer->getAttributes()[ mAttributes[i].attributeID ].name == _name)
        {
            detachAttribute( (GLuint) i );
            // the other pointer data is still set, but that isn't relevant if the attribute itself is deactivated
            //glDisableVertexArrayAttribEXT( mObjectName, mAttributes[i].location );
            //mAttributes.erase( mAttributes.begin()+i );
            return;
        }
    }
    // if we got here, no Attribute of the given name exists
    ACGL::Utils::warning() << "can't detach attribute " << _name << " - no such Attribute" << std::endl;
}

void VertexArrayObject::detachAllAttributes()
{
    for(AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
    {
		detachAttribute( (GLuint) i );
    }
}

void VertexArrayObject::setAttributeLocations( ConstSharedLocationMappings _locationMappings )
{
    AttributeVec oldAttributes = mAttributes;
    for (AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
    {
        mAttributes[i].arrayBuffer = SharedArrayBuffer(); // set to NULL
    }
    AttributeVec unmatchedAttributes;

    // if there is an old attribute, look up the name in the _locationMap, use the new location if the
    // name is present (stored in the map), remove the attribute from the old map.
    // after that, run over the unmatchedAttributes to store the remaining attributes on free slots in the map
    for (AttributeVec::size_type i = 0; i < oldAttributes.size(); ++i)
    {
        if (oldAttributes[i].arrayBuffer) {
            GLint location = _locationMappings->getLocation( oldAttributes[i].arrayBuffer->getAttribute( (int) oldAttributes[i].attributeID ).name );
            if ((location >= 0) && ((GLuint)location < oldAttributes.size())) {
                mAttributes[location] = oldAttributes[i];
                //debug() << "moved  loc " << oldAttributes[i].arrayBuffer->getAttribute( (int) oldAttributes[i].attributeID ).name
                //        << " from " << i << " to " << location << std::endl;
            } else {
                unmatchedAttributes.push_back( oldAttributes[i] );
                //debug() << "unmatched " << oldAttributes[i].arrayBuffer->getAttribute( (int) oldAttributes[i].attributeID ).name
                //        << " at " << i << std::endl;
            }

            oldAttributes[i].arrayBuffer = SharedArrayBuffer(); // set to NULL
        }
    }
    for (AttributeVec::size_type i = 0; i < unmatchedAttributes.size(); ++i)
    {
        GLint location = getFreeAttributeLocation();
        mAttributes[location] = unmatchedAttributes[i]; // the total attribute number hasn't changed so location must be valid
        //debug() << "moved uloc " << unmatchedAttributes[i].arrayBuffer->getAttribute( (int) unmatchedAttributes[i].attributeID ).name
        //        << " to " << location << std::endl;
    }

    // query the currently bound VAO and restore that later to be side-effect free:
    storeOldVAOandBind();
    // disable all attributes & set the new ones
    for (GLuint i = 0; i < mAttributes.size(); ++i) {
        glDisableVertexAttribArray( i );

        if (mAttributes[i].arrayBuffer) setAttributePointer( i );
    }
    restoreOldVAO();
}


SharedLocationMappings VertexArrayObject::getAttributeLocations() const
{
    SharedLocationMappings locationMap = SharedLocationMappings( new LocationMappings() );

    for (AttributeVec::size_type i = 0; i < mAttributes.size(); ++i)
    {
        if ( mAttributes[i].arrayBuffer ) {
            //debug() << "aloc: " << mAttributes[i].arrayBuffer->getAttribute( (int) mAttributes[i].attributeID ).name << " " << i << std::endl;
			locationMap->setLocation(mAttributes[i].arrayBuffer->getAttribute((int)mAttributes[i].attributeID).name, (GLuint) i);
        }
    }

    return locationMap;
}


void VertexArrayObject::setAttributePointer( GLuint _index )
{
    mAttributes[_index].arrayBuffer->bind();

    ArrayBuffer::Attribute arrayBufferAttribute = mAttributes[_index].arrayBuffer->getAttribute(mAttributes[_index].attributeID);

    if (arrayBufferAttribute.isIntegerInShader == GL_TRUE) {
        // read out as a int, ivecN
        glVertexAttribIPointer(_index,
                               arrayBufferAttribute.size,
                               arrayBufferAttribute.type,
                               mAttributes[_index].arrayBuffer->getStride(),
                               reinterpret_cast<GLvoid*>(arrayBufferAttribute.offset)
                               );
    } else {
        // read out as a float, vecN
        glVertexAttribPointer(_index,
                              arrayBufferAttribute.size,
                              arrayBufferAttribute.type,
                              arrayBufferAttribute.normalized,
                              mAttributes[_index].arrayBuffer->getStride(),
                              reinterpret_cast<GLvoid*>(arrayBufferAttribute.offset)
                              );
    }
    #if ((ACGL_OPENGL_VERSION >= 33) || (ACGL_OPENGLES_VERSION >= 30))
        glVertexAttribDivisor( _index, arrayBufferAttribute.divisor );
    #else
        if(arrayBufferAttribute.divisor > 0)
            warning() << "Attribute divisors are not supported in OpenGL < 3.3 or OpenGL ES < 3.0" << std::endl;
    #endif
    glEnableVertexAttribArray(_index);
}



#endif // ACGL_OPENGL_SUPPORTS_VAO
