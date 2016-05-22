/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/Utils/MemoryMappedFile.hh>

#include <fstream>
#include <string>
#include <clocale>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace
{
    struct IndexTuple
    {
        IndexTuple() :
        position(-1),
        texCoord(-1),
        normal(-1)
        {
        }

        int position;
        int texCoord;
        int normal;
    };

    // Really naive implementtion of atof but fast
    // if parsing fails std::atof is used the format we expect is a float with a . as decimal point
    float fastAtof(const char * _begin ,const char* _end) {
        const char* p = _begin;
        float r = 0.0;
        bool neg = false;
        if (*p == '-') {
            neg = true;
            ++p;
        }
        while (*p >= '0' && *p <= '9' && p <= _end) {
            r = (r*10.0f) + (*p - '0');
            ++p;
        }
        if (*p == '.'  && p <= _end) {
            float f = 0.0f;
            int n = 0;
            ++p;
            while (*p >= '0' && *p <= '9'  && p <= _end) {
                f = (f*10.0f) + (*p - '0');
                ++p;
                ++n;
            }
            r += f / std::pow(10.0f, n);
        }
        if (neg) {
            r = -r;
        }
        if(p < _end)    //we didnt reach the end something went wrong
            return std::atof(_begin);
        return r;
    }

    void trim(const char*& _position)
    {
        while(*_position == ' ' || *_position == '\t')
            _position ++;
    }

    const char * nextObject(const char* _position, const char* _end)
    {
        while(_position < _end)
        {
            if(*_position == ' ' || *_position == '\t')
                return _position;
            ++_position;
        }
        return _position;
    }

    // Parses a string of space-separated numbers into a packed floating-point vector (_data) with a maximum number of _maxDimension elements
    void parseVector(const char* _it , const char* _end, int _maxDimension, int& _dimension, float* _data)
    {
        const char* found;
        _dimension = 0;
        while (_dimension < _maxDimension && _it < _end)
        {
            trim(_it);
            found = nextObject(_it,_end);
            _data[_dimension++] = fastAtof(_it,found-1);
            _it = found == _end ? _end : found + 1;
        }
    }

    // Turns an index parameter string into a std::vector of IndexTuples, e.g.
    // "1//2 3//4 5//6" --> { IndexTuple(1, -1, 2), IndexTuple(3, -1, 4), IndexTuple(5, -1, 6) }
    std::vector<IndexTuple> parseIndices(const char* _start, const char* _end)
    {
        std::vector<IndexTuple> indices;
        indices.reserve(5);

        const char* it = _start;
        trim(it);
        const char* vsit;
        const char* vsend;
        const char* foundSlash;
        int componentIndex;
        int index;
        while (it < _end)
        {
            vsit = it;
            vsend = nextObject(it, _end);
            componentIndex = 0;
            IndexTuple indexTuple;
            //process the string now meaning we split by /
            while (vsit < vsend)
            {
                trim(vsit);
                foundSlash = std::find(vsit, vsend, '/');
                index = std::atoi(vsit);
                if (componentIndex == 0) indexTuple.position = index - 1;
                if (componentIndex == 1) indexTuple.texCoord = index - 1;
                if (componentIndex == 2) indexTuple.normal = index - 1;
                componentIndex++;
                vsit = foundSlash == vsend ? vsend : foundSlash + 1;
            }
            indices.push_back(indexTuple);
            trim(vsend);
            it = vsend;
        }
        return indices;
    }
}

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

SharedGeometryData loadGeometryDataFromOBJ(const std::string& _filename, bool _computeNormals)
{
    char *currentLocale;
    currentLocale = setlocale( LC_NUMERIC, NULL ); // store current locale
    setlocale( LC_NUMERIC, "C" ); // make sure the decimal point is a '.'

    SharedGeometryData data;
    MemoryMappedFile mmf(_filename.c_str());
    if(mmf.errorCode())
    {
        error() << "could not open file " << _filename << std::endl;
        return data;
    }
    const char* pchBuf = mmf.data();
    const char* pchEnd = pchBuf + mmf.length();

    GLenum primitiveType = GL_INVALID_ENUM;
    bool hasTexCoords = false;
    bool hasNormals = false;
    int positionDimension = 4;
    int texCoordDimension = -1;
    int normalDimension = 3;

    std::vector<glm::vec4> positionData;
    std::vector<glm::vec3> texCoordData;
    std::vector<glm::vec3> normalData;

    std::vector<IndexTuple> indices;

    const char* keyword;
    size_t keywordLength;
    const char* parameters[2];
    while (pchBuf < pchEnd)
    {
        trim(pchBuf);
        // Parse the current line
        const char* pchEOL = std::find(pchBuf, pchEnd, '\n');

        // skip empty lines or lines starting with #
        if (*pchBuf == '#' || pchBuf == pchEOL)
        {
            pchBuf = pchEOL + 1;
            continue;
        }

        // Otherwise, extract the first word and the remainder
        const char* pchKey = nextObject(pchBuf, pchEnd);
        keyword = pchBuf;
        keywordLength = pchKey - pchBuf;
        trim(pchKey);
        parameters[0] = pchKey;
        parameters[1] = pchEOL;

        if(strncmp(keyword,"v",keywordLength) == 0) // vertex position
        {
            glm::vec4 position;
            int dimension;
            parseVector(parameters[0], parameters[1], 4, dimension, (float*)&position);
            if(dimension < 4) position.w = 1.0;

            if(dimension < 3)
            {
                error() << "could not load OBJ: wrong vertex position dimension" << std::endl;
                return data;
            }

            positionData.push_back(position);
        }
        else if (strncmp(keyword, "vt", keywordLength) == 0) // vertex tex coord
        {
            glm::vec3 texCoord;
            int dimension;
            parseVector(parameters[0], parameters[1], 3, dimension, (float*)&texCoord);

            if(texCoordDimension < 0)
                texCoordDimension = dimension;
            else if(texCoordDimension != dimension)
            {
                error() << "could not load OBJ: contains mixed tex coord dimensions" << std::endl;
                return data;
            }

            hasTexCoords = true;

            texCoordData.push_back(texCoord);
        }
        else if (strncmp(keyword, "vn", keywordLength) == 0) // vertex normal
        {
            glm::vec3 normal;
            int dimension;
            parseVector(parameters[0], parameters[1], 3, dimension, (float*)&normal);

            if(dimension < 3)
            {
                error() << "could not load OBJ: wrong vertex normal dimension" << std::endl;
                return data;
            }

            hasNormals = true;

            normalData.push_back(normal);
        }
        else if (strncmp(keyword, "p", keywordLength) == 0) // point
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_POINTS;
            else if(primitiveType != GL_POINTS)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> pointIndices = parseIndices(parameters[0], parameters[1]);
            // points are just added in order
            for(size_t i = 0; i < pointIndices.size(); ++i)
            {
                indices.push_back(pointIndices[i]);
            }
        }
        else if (strncmp(keyword, "l", keywordLength) == 0) // line
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_LINES;
            else if(primitiveType != GL_LINES)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> lineIndices = parseIndices(parameters[0], parameters[1]);
            // add line segments for the line strip defined by the vertices
            for(size_t i = 0; i < lineIndices.size() - 1; ++i)
            {
                indices.push_back(lineIndices[i]);
                indices.push_back(lineIndices[i+1]);
            }
        }
        else if (strncmp(keyword, "f", keywordLength) == 0) // face
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_TRIANGLES;
            else if(primitiveType != GL_TRIANGLES)
            {
                error() << "could not load OBJ: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<IndexTuple> faceIndices = parseIndices(parameters[0], parameters[1]);
            // triangulate the polygon defined by the indices
            for(size_t i = 1; i < faceIndices.size() - 1; ++i)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i+1]);
            }
        }
        else if (strncmp(keyword, "bevel", keywordLength) == 0      || strncmp(keyword, "bmat", keywordLength) == 0
            || strncmp(keyword, "bsp", keywordLength) == 0          || strncmp(keyword, "bzp", keywordLength) == 0
            || strncmp(keyword, "c_interp", keywordLength) == 0     || strncmp(keyword, "cdc", keywordLength) == 0
            || strncmp(keyword, "cdp", keywordLength) == 0          || strncmp(keyword, "con", keywordLength) == 0
            || strncmp(keyword, "cstype", keywordLength) == 0       || strncmp(keyword, "ctech", keywordLength) == 0
            || strncmp(keyword, "curv", keywordLength) == 0         || strncmp(keyword, "curv2", keywordLength) == 0
            || strncmp(keyword, "d_interp", keywordLength) == 0     || strncmp(keyword, "deg", keywordLength) == 0
            || strncmp(keyword, "end", keywordLength) == 0          || strncmp(keyword, "g", keywordLength) == 0
            || strncmp(keyword, "hole", keywordLength) == 0         || strncmp(keyword, "lod", keywordLength) == 0
            || strncmp(keyword, "maplib", keywordLength) == 0       || strncmp(keyword, "mg", keywordLength) == 0
            || strncmp(keyword, "mtllib", keywordLength) == 0       || strncmp(keyword, "o", keywordLength) == 0
            || strncmp(keyword, "parm", keywordLength) == 0         || strncmp(keyword, "res", keywordLength) == 0
            || strncmp(keyword, "s", keywordLength) == 0            || strncmp(keyword, "scrv", keywordLength) == 0
            || strncmp(keyword, "shadow_obj", keywordLength) == 0   || strncmp(keyword, "sp", keywordLength) == 0
            || strncmp(keyword, "stech", keywordLength) == 0        || strncmp(keyword, "step", keywordLength) == 0
            || strncmp(keyword, "surf", keywordLength) == 0         || strncmp(keyword, "trace_obj", keywordLength) == 0
            || strncmp(keyword, "trim", keywordLength) == 0         || strncmp(keyword, "usemap", keywordLength) == 0
            || strncmp(keyword, "usemtl", keywordLength) == 0       || strncmp(keyword, "vp", keywordLength) == 0)
        {
            // part of the OBJ specification (i.e. non-polygonal geometry, object groups, etc.)
            // is not supported and is silently ignored
        }
        else
        {
            warning() << "unknown OBJ keyword ignored: " << keyword << std::endl;
        }
        pchBuf = pchEOL + 1;
    }

    if (!hasNormals && _computeNormals) {
        // perform own per-face normal creation only if the model had no own normals!
        if(primitiveType != GL_TRIANGLES)
        {
            warning() << "computing OBJ normals is only supported for models with faces" << std::endl;
            _computeNormals = false;
        }
        else
        {
            debug() << "model has no normals, computing face normals" << std::endl;
            hasNormals = true;
        }
    } else if (hasNormals) {
        // if the model has normals defined, no face normals have to get computed
        _computeNormals = false;
    }

    // all data are read from the file. construct an ArrayBuffer from the data
    data = SharedGeometryData(new GeometryData());

    size_t abDataElements = (positionDimension + hasTexCoords * texCoordDimension + hasNormals * normalDimension) * indices.size();
    GLfloat* abData = new GLfloat[abDataElements];

    size_t pos = 0;
    for(size_t i = 0; i < indices.size(); ++i)
    {
        const glm::vec4& position = positionData[indices[i].position];
        abData[pos++] = position.x;
        abData[pos++] = position.y;
        abData[pos++] = position.z;
        abData[pos++] = position.w;

        if(hasTexCoords)
        {
            const glm::vec3& texCoord = texCoordData[indices[i].texCoord];
            for(int dim = 0; dim < texCoordDimension; ++dim)
            {
                abData[pos++] = texCoord[dim];
            }
        }

        if(hasNormals)
        {
            if(_computeNormals)
            {
                size_t triangleIndex = i / 3;
                glm::vec3 v0 = (glm::vec3)positionData[indices[3 * triangleIndex + 0].position];
                glm::vec3 v1 = (glm::vec3)positionData[indices[3 * triangleIndex + 1].position];
                glm::vec3 v2 = (glm::vec3)positionData[indices[3 * triangleIndex + 2].position];
                glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
                if (normal != glm::vec3(0))
                    normal = glm::normalize(normal);
                abData[pos++] = normal.x;
                abData[pos++] = normal.y;
                abData[pos++] = normal.z;
            }
            else
            {
                const glm::vec3& normal = normalData[indices[i].normal];
                abData[pos++] = normal.x;
                abData[pos++] = normal.y;
                abData[pos++] = normal.z;
            }
        }
    }

    size_t strideSize = 0;
    ArrayBuffer::Attribute attrPosition = { "aPosition", GL_FLOAT, positionDimension, (GLuint)0, GL_FALSE, 0, GL_FALSE };
    strideSize += positionDimension * sizeof(GLfloat);

    data->mAttributes.push_back(attrPosition);

    if(hasTexCoords)
    {
        ArrayBuffer::Attribute attrTexCoord  = { "aTexCoord", GL_FLOAT, texCoordDimension, (GLuint)strideSize, GL_FALSE, 0, GL_FALSE };
        strideSize += texCoordDimension * sizeof(GLfloat);

        data->mAttributes.push_back(attrTexCoord);
    }

    if(hasNormals)
    {
        ArrayBuffer::Attribute attrNormal = { "aNormal", GL_FLOAT, normalDimension, (GLuint)strideSize, GL_FALSE, 0, GL_FALSE };
        strideSize += normalDimension * sizeof(GLfloat);

        data->mAttributes.push_back(attrNormal);
    }

    data->setStrideSize( (GLsizei) strideSize);
    data->setSize( (GLsizei) abDataElements * sizeof(GLfloat));
    data->setData((GLubyte*)abData);

    setlocale( LC_NUMERIC, currentLocale ); // restore old locale

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

} // OpenGL
} // ACGL
