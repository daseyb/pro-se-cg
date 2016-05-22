/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/VertexArrayObjectLoadStore.hh>
#include <ACGL/OpenGL/Data/ArrayBufferLoadStore.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>

#if defined(ACGL_OPENGL_SUPPORTS_VAO)

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace ACGL{
namespace OpenGL{

SharedVertexArrayObject loadVertexArrayObject(const std::string& _filename, bool _caching)
{
    // Lower case file ending:
    std::string fileEnding = StringHelpers::getFileEnding(_filename);
    std::string loadFileName = _filename;

    // Never cache VAO files
    if(fileEnding == "vao")
        _caching = false;

    bool saveCachedVAO = false;
    if(_caching)
    {
        // See whether a cached version of the file is available
        if(FileHelpers::fileExists(_filename + ".vao")
        && FileHelpers::getFileModificationTime(_filename + ".vao") > FileHelpers::getFileModificationTime(_filename))
        {
            // If so, load the cached file instead
            loadFileName = _filename + ".vao";
            fileEnding = "vao";
        }
        else
        {
            // Otherwise, create a cached copy after loading
            saveCachedVAO = true;
        }
    }

    // Load the VAO
    SharedVertexArrayObject vao;
    if(fileEnding == "vao")
    {
        vao = loadVertexArrayObjectFromVAO(loadFileName);
    }
    else
    {
        // Generic load: Try to load the file as an ArrayBuffer and attach it to a VAO
        SharedArrayBuffer ab = loadArrayBuffer(loadFileName);
        if(ab)
        {
            vao = SharedVertexArrayObject(new VertexArrayObject);
            vao->attachAllAttributes(ab);
        }
    }

    // If necessary, store a cached copy to disk
    if(vao && saveCachedVAO)
    {
        ACGL::Utils::debug() << "Saving a cached copy of " << _filename << " ..." << std::endl;
        saveVertexArrayObjectToVAO(vao, _filename + ".vao");
    }

    vao->setObjectLabel( _filename );

    return vao;
}

}
}

#endif
