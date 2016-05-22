/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Data/VertexArrayObjectLoadStore.hh>

#if defined( ACGL_OPENGL_SUPPORTS_VAO )

using namespace ACGL;
using namespace ACGL::OpenGL;

VertexArrayObjectCreator::VertexArrayObjectCreator(const std::string &_filename)
:   Resource::SingleFileBasedCreator<VertexArrayObject>(_filename, Base::Settings::the()->getFullGeometryPath()),
    mCaching(false)
{
}

VertexArrayObjectCreator::VertexArrayObjectCreator(const char *_filename)
:   Resource::SingleFileBasedCreator<VertexArrayObject>(std::string(_filename), Base::Settings::the()->getFullGeometryPath()),
    mCaching(false)
{
}

bool VertexArrayObjectCreator::load(SharedVertexArrayObject &_vao)
{
    _vao = loadVertexArrayObject(getFullFilePath(), mCaching);
    return _vao != nullptr;
}

SharedVertexArrayObject VertexArrayObjectCreator::create()
{
    updateFileModificationTime();

    SharedVertexArrayObject vao(new VertexArrayObject);

    if(!load(vao))
        return SharedVertexArrayObject();

    return vao;
}

bool VertexArrayObjectCreator::update(SharedVertexArrayObject &vao)
{
    if(fileIsUpToDate())
        return false;

    if(!load(vao))
        return false;

    updateFileModificationTime();
    return true;
}

#endif // #ifdef ACGL_OPENGL_SUPPORTS_VAO

