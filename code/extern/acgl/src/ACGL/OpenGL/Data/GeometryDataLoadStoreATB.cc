/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/GeometryDataLoadStore.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include <fstream>
#include <string>

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;

namespace
{
    void skipLine(std::fstream& _stream)
    {
        _stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Parses a string of space-separated numbers into a packed floating-point vector (_data) with a maximum number of _maxDimension elements
    void parseVector(const std::string& _string, int _maxDimension, int& _dimension, float* _data)
    {
        std::stringstream stream(_string);
        float temp;
        _dimension = 0;
        while(stream >> temp && _dimension < _maxDimension)
        {
            _data[_dimension] = temp;
            _dimension++;
        }
    }

    // Turns an index parameter string into a std::vector of indices
    std::vector<int> parseIndices(const std::string& _string)
    {
        std::vector<int> indices;

        std::stringstream stream(_string);
        int index;
        while(stream >> index)
        {
            indices.push_back(index - 1);
        }

        return indices;
    }
}

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

SharedGeometryData loadGeometryDataFromATB(const std::string& _filename, const std::string& _attributeName)
{
    SharedGeometryData data;

    std::fstream file(_filename.c_str(), std::ios_base::in);
    if(!file.good())
    {
        error() << "could not open file " << _filename << std::endl;
        return data;
    }

    GLenum primitiveType = GL_INVALID_ENUM;
    int attributeDimension = -1;

    std::vector<glm::vec4> attributeData;
    std::vector<int> indices;

    std::string keyword;
    std::string parameters;
    while(file.good())
    {
        // Parse the current line

        // If the line starts with a #, it is a comment
        if(file.peek() == '#')
        {
            skipLine(file);
            continue;
        }

        // Otherwise, extract the first word and the remainder
        file >> keyword;
        std::getline(file, parameters);

        if(keyword == "a") // attribute data
        {
            glm::vec4 attribute;
            int dimension;
            parseVector(parameters, 4, dimension, (float*)&attribute);

            if(attributeDimension < 0)
                attributeDimension = dimension;
            else if(dimension != attributeDimension)
            {
                error() << "could not load ATB: contains attributes of mixed size" << std::endl;
                return data;
            }

            attributeData.push_back(attribute);
        }
        else if(keyword == "p") // point
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_POINTS;
            else if(primitiveType != GL_POINTS)
            {
                error() << "could not load ATB: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<int> pointIndices = parseIndices(parameters);
            // points are just added in order
            for(size_t i = 0; i < pointIndices.size(); ++i)
            {
                indices.push_back(pointIndices[i]);
            }
        }
        else if(keyword == "l") // line
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_LINES;
            else if(primitiveType != GL_LINES)
            {
                error() << "could not load ATB: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<int> lineIndices = parseIndices(parameters);
            // add line segments for the line strip defined by the vertices
            for(size_t i = 0; i < lineIndices.size() - 1; ++i)
            {
                indices.push_back(lineIndices[i]);
                indices.push_back(lineIndices[i+1]);
            }
        }
        else if(keyword == "f") // face
        {
            if(primitiveType == GL_INVALID_ENUM)
                primitiveType = GL_TRIANGLES;
            else if(primitiveType != GL_TRIANGLES)
            {
                error() << "could not load ATB: contains mixed primitive types" << std::endl;
                return data;
            }

            std::vector<int> faceIndices = parseIndices(parameters);
            // triangulate the polygon defined by the indices
            for(size_t i = 1; i < faceIndices.size() - 1; ++i)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i+1]);
            }
        }
        else
        {
            warning() << "unknown ATB keyword ignored: " << keyword << std::endl;
        }
    }

    // Flatten the indexed attribute data
    // Also the attribute dimension may be lower than 4 so we can throw away unused elements
    size_t abDataElements = indices.size();
    size_t abDataSize = abDataElements * attributeDimension;
    GLfloat* abData = new GLfloat[abDataSize];

    size_t abDataIndex = 0;
    for(size_t i = 0; i < indices.size(); ++i)
    {
        const glm::vec4& attribute = attributeData[indices[i]];
        for(int dim = 0; dim < attributeDimension; ++dim)
        {
            abData[abDataIndex++] = attribute[dim];
        }
    }

    // If no attribute name was given, try to guess a name from the filename
    // e.g. /foo/bar/VertexColor.atb --> aVertexColor
    std::string attributeName = _attributeName;
    if(attributeName.empty())
    {
        std::string _;
        std::string filenameWithoutPath;
        std::string filenameWithoutPathAndExtension;
        if(ACGL::Utils::StringHelpers::splitLastFileOrFolder(_filename, _, filenameWithoutPath) &&
           ACGL::Utils::StringHelpers::splitFileExtension(filenameWithoutPath, filenameWithoutPathAndExtension, _))
        {
            attributeName = "a" + filenameWithoutPathAndExtension;
        }
        else
        {
            // Fallback if attribute name could not be guessed from the filename
            attributeName = "aAttribute";
        }
    }

    // All data are read from the file. Construct an ArrayBuffer from the data
    data = SharedGeometryData(new GeometryData());
    ArrayBuffer::Attribute attributeDescription = { attributeName, GL_FLOAT, attributeDimension, (GLuint)0, GL_FALSE, 0, GL_FALSE };
    data->mAttributes.push_back(attributeDescription);

    data->setStrideSize(attributeDimension * sizeof(GLfloat));
    data->setSize( (GLsizei) (abDataSize * sizeof(GLfloat)) );
    data->setData((GLubyte*)abData);

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

} // OpenGL
} // ACGL
