/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/


#include <ACGL/OpenGL/Creator/ArrayBufferCreator.hh>

using namespace ACGL::OpenGL;

SharedArrayBuffer ArrayBufferCreator::create()
{
    SharedArrayBuffer arrayBuffer( new ArrayBuffer() );

    for(AttributeDefineVec::size_type i = 0; i < mAttributeDefines.size(); i++)
    {
        if(mAttributeDefines[i].isInteger)
        {
            arrayBuffer->defineIntegerAttribute(mAttributeDefines[i].name,
                                                mAttributeDefines[i].type,
                                                mAttributeDefines[i].dimension);
        }
        else
        {
            arrayBuffer->defineAttribute(mAttributeDefines[i].name,
                                         mAttributeDefines[i].type,
                                         mAttributeDefines[i].dimension,
                                         mAttributeDefines[i].normalized);
        }

    }
    if (mpData != NULL)
    {
        arrayBuffer->bind();
        arrayBuffer->setDataElements(mElements, mpData, mUsage);
    }
    return arrayBuffer;
}

