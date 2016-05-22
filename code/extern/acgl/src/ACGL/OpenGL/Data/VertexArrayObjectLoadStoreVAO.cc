/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/VertexArrayObjectLoadStore.hh>
#include <ACGL/OpenGL/GL.hh>

#if defined(ACGL_OPENGL_SUPPORTS_VAO)

#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>
#include <ACGL/OpenGL/Objects/ElementArrayBuffer.hh>

#include <fstream>
#include <set>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace
{
    // Reads _n bytes from _stream and returns them as a std::string. If the string
    // in the stream is null terminated, only _n-1 characters are read and the
    // trailing '\0' is ignored
    std::string readString(std::ifstream& _stream, std::streamsize _n, bool _nullTerminated = true)
    {
        std::string str;
        if(_nullTerminated)
            --_n;
        str.resize(_n);
        _stream.read((char*)str.data(), _n);
        if(_nullTerminated)
            _stream.ignore(1);
        return str;
    }

    // Writes _string to _stream. By default, appends a trailing '\0'
    void writeString(std::ofstream& _stream, const std::string& _string, bool _nullTerminated = true)
    {
        _stream.write((char*)_string.data(), _string.size());
        if(_nullTerminated)
            _stream.put('\0');
    }
}

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

SharedVertexArrayObject loadVertexArrayObjectFromVAO(const std::string& _filename, const std::string& _eabName)
{
    SharedVertexArrayObject vao(new VertexArrayObject());

    std::ifstream file(_filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if(!file.good())
    {
        error() << "Could not open file " << _filename << std::endl;
        return SharedVertexArrayObject();
    }

    // Header Section
    std::string magic = readString(file, 4, false);
    if(magic != "VAO1")
    {
        error() << _filename << " is not an ACGL VAO file" << std::endl;
        return SharedVertexArrayObject();
    }

    uint32_t numABs;
    uint32_t numEABs;
    GLenum   defaultMode;

    file.read((char*)&numABs, 4);
    file.read((char*)&numEABs, 4);
    file.read((char*)&defaultMode, 4);

    // Warn if no AB was specified
    if(numABs == 0)
    {
        warning() << _filename << " contains no ArrayBuffers" << std::endl;
    }

    vao->setMode(defaultMode);

    // ArrayBuffer Sections
    for(uint32_t abindex = 0; abindex < numABs; ++abindex)
    {
        // ArrayBuffer Section
        SharedArrayBuffer ab = SharedArrayBuffer(new ArrayBuffer());

        uint32_t    abNameLength;
        std::string abName;
        GLsizei     abStride;
        uint32_t    abNumAttributes;

        file.read((char*)&abNameLength,    4);
        abName = readString(file, abNameLength);
        file.read((char*)&abStride,        4);
        file.read((char*)&abNumAttributes, 4);

        for(uint32_t attrindex = 0; attrindex < abNumAttributes; ++attrindex)
        {
            // ArrayBuffer Attribute Section
            ArrayBuffer::Attribute attr;
            uint32_t               attrNameLength;

            file.read((char*)&attrNameLength,          4);
            attr.name = readString(file, attrNameLength);
            file.read((char*)&attr.type,               4);
            file.read((char*)&attr.size,               4);
            file.read((char*)&attr.offset,             4);
            file.read((char*)&attr.normalized,         1);
            file.read((char*)&attr.isIntegerInShader,  1);
            file.read((char*)&attr.divisor,            4);

            ab->defineAttribute(attr);
        }

        // ArrayBuffer Section contd.
        uint64_t abDataLength;
        char*    abData;

        file.read((char*)&abDataLength, 8);
        abData = new char[abDataLength];
        file.read(abData, abDataLength);

        ab->setStride(abStride);
        ab->setData(abDataLength, (GLvoid*)abData);

        delete[] abData;

        vao->attachAllAttributes(ab);
    }

    // ElementArrayBuffer Sections
    bool eabMatched = false; // Only match the first EAB fitting _eabName
    for(uint32_t i = 0; i < numEABs; ++i)
    {
        // ElementArrayBuffer Section
        uint32_t    eabNameLength;
        std::string eabName;
        GLenum      eabMode;
        GLenum      eabType;
        bool        eabPR;
        GLuint      eabPRIndex;
        uint64_t    eabDataLength;
        char*       eabData;

        file.read((char*)&eabNameLength, 4);
        eabName = readString(file, eabNameLength);
        file.read((char*)&eabMode,       4);
        file.read((char*)&eabType,       4);
        file.read((char*)&eabPR,         1);
        file.read((char*)&eabPRIndex,    4);
        file.read((char*)&eabDataLength, 8);

        if(!eabMatched && (_eabName == "" || _eabName == eabName))
        {
            // This is the EAB we are looking for
            eabMatched = true;
            SharedElementArrayBuffer eab(new ElementArrayBuffer());
            eab->setType(eabType);
            if(eabPR)
            {
                // TODO: Implement Primitive Restart
                warning() << "Primitive Restart for EABs is not supported yet" << std::endl;
            }

            eabData = new char[eabDataLength];
            file.read(eabData, eabDataLength);
            eab->setData(eabDataLength, (GLvoid*)eabData);
            delete[] eabData;

            vao->attachElementArrayBuffer(eab);
            vao->setMode(eabMode);
        }
        else
        {
            // Ignore this EAB
            file.ignore(eabDataLength);
        }

        // Warn if an EAB name was specified but none matched
        if(!_eabName.empty() && !eabMatched)
        {
            warning() << "No EAB with name " << _eabName << " found in " << _filename << std::endl;
        }
    }

    return vao;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

bool saveVertexArrayObjectToVAO(ConstSharedVertexArrayObject _vao, const std::string& _filename)
{
#if (ACGL_OPENGLES_VERSION >= 20)
    // not on GL ES so far
    error() << "saveVertexArrayObjectToVAO is not implemented for OpenGL ES yet!" << std::endl;
    return false;
#else
    
    // First, find the set of ArrayBuffers used by _vao
    // Each ArrayBuffer will be stored entirely in the VAO file but only those attributes which
    // are used by _vao will be defined.
    std::set<ConstSharedArrayBuffer> usedArrayBuffers;
    for(VertexArrayObject::AttributeVec::const_iterator it = _vao->getAttributes().begin();
        it != _vao->getAttributes().end();
        ++it)
    {
        const VertexArrayObject::Attribute& attr = *it;
        if(attr.arrayBuffer)
            usedArrayBuffers.insert(attr.arrayBuffer);
    }

    std::ofstream file(_filename.c_str(), std::ios_base::out | std::ios_base::binary);

    // Header Section
	uint32_t numABs = (uint32_t) usedArrayBuffers.size();
    uint32_t numEABs     = _vao->getElementArrayBuffer() ? 1 : 0;
    GLenum   defaultMode = _vao->getMode();

    writeString(file, "VAO1", false);
    file.write((char*)&numABs,      4);
    file.write((char*)&numEABs,     4);
    file.write((char*)&defaultMode, 4);

    for(std::set<ConstSharedArrayBuffer>::const_iterator abIt = usedArrayBuffers.begin();
        abIt != usedArrayBuffers.end();
        ++abIt)
    {
        // Array Buffer Section
        const ConstSharedArrayBuffer& ab = *abIt;

        // Find all attributes in _vao that use this AB
        std::vector<int32_t> attributeIDs;
        for(VertexArrayObject::AttributeVec::const_iterator attrIt = _vao->getAttributes().begin();
            attrIt != _vao->getAttributes().end();
            ++attrIt)
        {
            const VertexArrayObject::Attribute& attr = *attrIt;
            if(attr.arrayBuffer == ab)
                attributeIDs.push_back(attr.attributeID);
        }

        std::string abName          = "";
		uint32_t    abNameLength    = (uint32_t) abName.size() + 1; // Counting the trailing '\0'
        GLsizei     abStride        = ab->getStride();
		uint32_t    abNumAttributes = (uint32_t) attributeIDs.size();

        file.write((char*)&abNameLength,    4);
        writeString(file, abName);
        file.write((char*)&abStride,        4);
        file.write((char*)&abNumAttributes, 4);

        for(std::vector<int32_t>::iterator attrIDIt = attributeIDs.begin();
            attrIDIt != attributeIDs.end();
            ++attrIDIt)
        {
            // ArrayBuffer Attribute Section
            int32_t attrID = *attrIDIt;

            std::string attrName       = ab->getAttribute(attrID).name;
			uint32_t    attrNameLength = (uint32_t) attrName.size() + 1; // Counting the trailing '\0'
            GLenum      attrType       = ab->getAttribute(attrID).type;
            GLint       attrSize       = ab->getAttribute(attrID).size;
            GLuint      attrOffset     = ab->getAttribute(attrID).offset;
            GLboolean   attrNormalized = ab->getAttribute(attrID).normalized;
            GLboolean   attrIsInteger  = ab->getAttribute(attrID).isIntegerInShader;
            GLuint      attrDivisor    = ab->getAttribute(attrID).divisor;

            file.write((char*)&attrNameLength, 4);
            writeString(file, attrName);
            file.write((char*)&attrType,       4);
            file.write((char*)&attrSize,       4);
            file.write((char*)&attrOffset,     4);
            file.write((char*)&attrNormalized, 1);
            file.write((char*)&attrIsInteger,  1);
            file.write((char*)&attrDivisor,    4);
        }

        // Array Buffer Section contd.
        uint64_t abDataLength = ab->getSize();
        file.write((char*)&abDataLength, 8);

        ArrayBuffer* abMutable = const_cast<ArrayBuffer*>(ab.get());
        char* abData = (char*)abMutable->map(GL_READ_ONLY);
        file.write(abData, abDataLength);
        abMutable->unmap();
    }

    if(_vao->getElementArrayBuffer())
    {
        // ElementArrayBuffer section
        const ConstSharedElementArrayBuffer& eab = _vao->getElementArrayBuffer();

        std::string eabName       = "";
		uint32_t    eabNameLength = (uint32_t) eabName.size() + 1; // Counting the trailing '\0'
        GLenum      eabMode       = _vao->getMode();
        GLenum      eabType       = eab->getType();
        bool        eabPR         = false;              // TODO: implement Primitive Reset
        GLuint      eabPRIndex    = 0;                  // TODO: implement Primitive Reset
        uint64_t    eabDataLength = eab->getSize();

        file.write((char*)&eabNameLength, 4);
        writeString(file, eabName);
        file.write((char*)&eabMode,       4);
        file.write((char*)&eabType,       4);
        file.write((char*)&eabPR,         1);
        file.write((char*)&eabPRIndex,    4);
        file.write((char*)&eabDataLength, 8);

        ElementArrayBuffer* eabMutable = const_cast<ElementArrayBuffer*>(eab.get());
        char* eabData = (char*)eabMutable->map(GL_READ_ONLY);
        file.write(eabData, eabDataLength);
        eabMutable->unmap();
    }

    return true;
#endif
}

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_SUPPORTS_VAO

