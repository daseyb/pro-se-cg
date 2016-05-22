/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Data/TextureDataLoadStore.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/Utils/Memory.hh>

// for the screenshot function:
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>
#include <cstdio>
#include <ctime>

#include <stdexcept>
#include <fstream>
#include "lodepng/lodepng.h"
#include "rgbe/rgbe.hh"

#ifdef ACGL_COMPILE_WITH_QT
#include <QImage>
#include <QGLWidget>
#include <QByteArray>
#include <QImageWriter>
#elif ACGL_COMPILE_WITH_QT5
#include <QImage>
#include <QtOpenGL/QGLWidget>
#include <QByteArray>
#include <QImageWriter>
#endif

//
// OpenEXR:
// Only compile with OpenEXR support if explicitly wanted so.
// Note that the application has to be linked to libIlmImf and libIex and the include path has to be set.
// (in your CMakeLists.txt add:
//   ADD_DEFINITIONS(-DACGL_BUILD_WITH_EXR)
//   ADD_DEFINITIONS(-I"/usr/include/OpenEXR/")
//   SET(LIBRARIES ${LIBRARIES} "-lIlmImf -lIex")
//
#ifdef ACGL_BUILD_WITH_EXR
//#include <ImfInputFile.h>
#include <ImfRgbaFile.h>
using namespace Imf;
using namespace Imath;
#endif

using namespace ACGL;
using namespace ACGL::OpenGL;
using namespace ACGL::Utils;
using namespace ACGL::Utils::StringHelpers;

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               generic load/save
///////////////////////////////////////////////////////////////////////////////////////////////////

std::map<std::string,TextureLoadFuncPtr> textureLoadFunctions = std::map<std::string,TextureLoadFuncPtr>();

void registerTextureLoadFunction(std::vector<std::string> _endings, TextureLoadFuncPtr _function)
{
    if(_function != NULL)
    {
        std::vector<std::string>::iterator n = _endings.end();
        for(std::vector<std::string>::iterator i = _endings.begin() ; i != n ; i++ )
        {
            textureLoadFunctions.insert( std::pair<std::string,TextureLoadFuncPtr>(*i ,_function) );
        }
    }
}

void unregisterTextureLoadFunction(TextureLoadFuncPtr _function)
{
    std::map<std::string,TextureLoadFuncPtr>::iterator n = textureLoadFunctions.end();
    for(std::map<std::string,TextureLoadFuncPtr>::iterator i = textureLoadFunctions.begin() ; i != n ; i++)
    {

        if(i->second == _function)
            textureLoadFunctions.erase(i);
    }
}

SharedTextureData loadTextureData(const std::string &_filename, ColorSpace _colorSpace)
{
    // lower case file ending:
    std::string fileEnding = getFileEnding( _filename );

    SharedTextureData textureData;

    if(textureLoadFunctions.find(fileEnding) != textureLoadFunctions.end()) {
        textureData = textureLoadFunctions[fileEnding](_filename, _colorSpace);
    } else if (fileEnding == "png") {
        textureData = loadTextureDataFromLodepng( _filename, _colorSpace );
    } else if (fileEnding == "hdr") {
        textureData = loadTextureDataFromRGBE( _filename, _colorSpace );
    } else if (fileEnding == "exr") {
        textureData = loadTextureDataFromEXR( _filename, _colorSpace );
    } else if (fileEnding == "ppm") {
        textureData = loadTextureDataFromPNM( _filename, _colorSpace );
    }
#ifdef ACGL_COMPILE_WITH_QT
    else if (   fileEnding == "bmp" || fileEnding == "jpg" || fileEnding == "jpeg"
             || fileEnding == "png" || fileEnding == "pbm" || fileEnding == "ppm"
             || fileEnding == "tif" || fileEnding == "tiff"|| fileEnding == "xbm"
             || fileEnding == "xpm" || fileEnding == "pgm") {
        textureData = loadTextureDataFromQT( _filename, _colorSpace );
    }
#endif
    else {
        error() << "texture file format of " << _filename << " not supported" << std::endl;
    }

    return textureData;
}

bool saveTextureData(const SharedTextureData &_textureData, const std::string &_filename)
{
    // lower case file ending:
    std::string fileEnding = getFileEnding( _filename );

    if (fileEnding == "png") {
        return saveTextureDataToLodepng( _textureData, _filename );
    }
#ifdef ACGL_COMPILE_WITH_QT
    else if (   fileEnding == "bmp" || fileEnding == "jpg" || fileEnding == "jpeg"
             || fileEnding == "png" || fileEnding == "ppm"
             || fileEnding == "tif" || fileEnding == "tiff"|| fileEnding == "xbm"
             || fileEnding == "xpm") {
        return saveTextureDataToQT( _textureData, _filename );
    }
#endif
    else if (fileEnding == "ppm") {
        return saveTextureDataToPPM( _textureData, _filename );
    } else if (fileEnding == "raw") {
        return saveTextureDataToRAW( _textureData, _filename );
    } else {
        error() << "texture file format of " << _filename << " not supported" << std::endl;
    }

    return false;
}

bool saveScreenshot(const std::string& _fileName)
{
    ACGL::OpenGL::SharedTextureData screenShot = FrameBufferObject::getImageData();
    bool success = ACGL::OpenGL::saveTextureData( screenShot, _fileName );
    return success;
}


bool saveScreenshotWithDate(const std::string& _prefix, const std::string& _fileEnding )
{
    time_t rawtime;
    time( &rawtime );

    char timestring[30];
    strftime( timestring, 30, "_%Y-%m-%d_%H-%M-%S", localtime( &rawtime ) );

    static int i = 0;
    i++;

    std::string filename = _prefix + timestring + "_" + ACGL::Utils::StringHelpers::toString(i) + "." + _fileEnding;

    return saveScreenshot(filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

struct LodepngFile
{
public:
    LodepngFile(const std::string& _filename)
    {
        unsigned int errorCode = lodepng_load_file(&mData, &mSize, _filename.c_str());
        if(errorCode)
        {
            std::stringstream errorMsg;
            errorMsg << "LodePNG error while loading file " << _filename << " - " << errorCode << ": " << lodepng_error_text(errorCode);
            mData = NULL;
            throw std::runtime_error(errorMsg.str());
        }
    }

    ~LodepngFile()
    {
        free(mData);
    }

    unsigned char* mData;
    size_t mSize;
};

SharedTextureData loadTextureDataFromLodepng(const std::string &_filename, ColorSpace _colorSpace)
{
    SharedTextureData data;

    unsigned int errorCode;
    unsigned int width, height;
    LodePNGState state;

    // Load the PNG file from disk
    lodepng_state_init(&state);

    try
    {
        LodepngFile lodepngFile(_filename);

        // Extract metadata (bit depth, color type)
        errorCode = lodepng_inspect(&width, &height, &state, lodepngFile.mData, lodepngFile.mSize);
        if(errorCode)
        {
            std::stringstream errorMsg;
            errorMsg << "LodePNG error " << errorCode << ": " << lodepng_error_text(errorCode);
            throw std::runtime_error(errorMsg.str());
        }

        unsigned int bitdepth = state.info_png.color.bitdepth;
        LodePNGColorType colorType = state.info_png.color.colortype;

        unsigned int channels = 0;
        GLenum glFormat = 0;

#if ((ACGL_OPENGLES_VERSION >= 30) || (ACGL_OPENGL_VERSION >= 30))
        if(colorType == LCT_GREY)       { channels = 1; glFormat = GL_RED;  }
        if(colorType == LCT_GREY_ALPHA) { channels = 2; glFormat = GL_RG;   }
#endif
        if(colorType == LCT_RGB)        { channels = 3; glFormat = GL_RGB;  }
        if(colorType == LCT_RGBA)       { channels = 4; glFormat = GL_RGBA; }
        if(colorType == LCT_PALETTE)    { channels = 4; glFormat = GL_RGBA; colorType = LCT_RGBA; } // force LodePNG to convert paletted data to RGBA

        GLenum glType = 0;
        if(bitdepth ==  8) glType = GL_UNSIGNED_BYTE;
        if(bitdepth == 16) glType = GL_UNSIGNED_SHORT;

        if(channels == 0 || glFormat == 0 || glType == 0)
        {
            std::stringstream errorMsg;
            errorMsg << "Could not load " << _filename << ": " << "unsupported bit depth or format";
            throw std::runtime_error(errorMsg.str());
        }

        // Decode the image
        unsigned char* lodepngImage;
        errorCode = lodepng_decode_memory(&lodepngImage, &width, &height, lodepngFile.mData, lodepngFile.mSize, colorType, bitdepth);
        if(errorCode)
        {
            std::stringstream errorMsg;
            errorMsg << "LodePNG error while decoding file " << _filename << " - " << errorCode << ": " << lodepng_error_text(errorCode);
            throw std::runtime_error(errorMsg.str());
        }
        // HOTFIX: TextureData destroys its pointer using delete[], but lodepng uses malloc().
        // As a hotfix, copy lodepngImage data to a memory location allocated using new[].
        unsigned int dataSize = width * height * channels * bitdepth / 8;
        unsigned char* image = new unsigned char[dataSize];
        memcpy(image, lodepngImage, dataSize);
        free(lodepngImage);

        // LodePNG decodes 16 bit PNGs in big endian format ==> Convert
#ifndef ACGL_BIG_ENDIAN
        if(bitdepth == 16)
            for(unsigned int i = 0; i < (width * height * channels * 2); i += 2)
                std::swap(image[i], image[i+1]);
#endif

        // Wrap the data in a TextureData object
        data = SharedTextureData(new TextureData());
        data->setData((GLubyte*)image);
        data->setWidth(width);
        data->setHeight(height);
        data->setType(glType);
        data->setFormat(glFormat);
        data->setPadding(0); // LodePNG data has no padding
        data->setColorSpace(_colorSpace); // no auto-detection

        // Flip
        data->flipVertically();
    }
    catch(std::runtime_error& error)
    {
        ACGL::Utils::error() << error.what() << std::endl;
    }

    return data;
}


#ifdef ACGL_COMPILE_WITH_QT
//! loads from the QT library
SharedTextureData loadTextureDataFromQT(const std::string& _filename, ColorSpace _colorSpace)
{
    SharedTextureData data = SharedTextureData( new TextureData() );

    QImage image = QImage( QString(_filename.c_str()) );

    if (image.isNull())
    {
        ACGL::Utils::error() << "Loading image " << _filename << " has failed!" << std::endl;
        return data;
    }

    image = QGLWidget::convertToGLFormat(image);
    GLubyte *pImageData = new GLubyte[image.byteCount()];
    memcpy(pImageData, image.bits(), image.byteCount());
    data->setData(   pImageData     );
    data->setWidth(  image.width()  );
    data->setHeight( image.height() );
    data->setColorSpace(_colorSpace); // no auto-detection

    return data;
}
#endif

SharedTextureData loadTextureDataFromRGBE(const std::string& _filename, ColorSpace _colorSpace)
{
    SharedTextureData data = SharedTextureData( new TextureData() );

    FILE* f = fopen(_filename.c_str(), "rb");

    int texWidth, texHeight;
    RGBE_ReadHeader(f, &texWidth, &texHeight, NULL);
    GLubyte* tempdata = new GLubyte[ sizeof(GL_FLOAT) * 3 * texWidth * texHeight];
    RGBE_ReadPixels_RLE(f, (float*)tempdata, texWidth, texHeight);
    fclose(f);

    data->setData(   tempdata  );
    data->setHeight( texHeight );
    data->setWidth(  texWidth  );
    data->setFormat( GL_RGB );
    data->setType(   GL_FLOAT  );
    data->setColorSpace(_colorSpace); // no auto-detection

    return data;
}

SharedTextureData loadTextureDataFromEXR(const std::string& _filename, ColorSpace _colorSpace)
{
#ifndef ACGL_BUILD_WITH_EXR
    error() << "can't load EXR file " << _filename << " ACGL was not build with EXR support" << std::endl;
    return SharedTextureData();
#else
    SharedTextureData data = SharedTextureData( new TextureData() );
    try {
        // see GPU gems 1:
        Imf::RgbaInputFile in( _filename.c_str() );

        Imath::Box2i window = in.dataWindow();

        Imath::V2i dimension( window.max.x - window.min.x +1,
                              window.max.y - window.min.y +1);

        unsigned char *buffer = (unsigned char *) new Imf::Rgba[ dimension.x*dimension.y ];

        int dx = window.min.x;
        int dy = window.min.y;

        in.setFrameBuffer( (Imf::Rgba*)buffer-dx-dy*dimension.x, 1, dimension.x );
        in.readPixels( window.min.y, window.max.y );

        data->setData(   buffer  );
        data->setHeight( dimension.y );
        data->setWidth(  dimension.x  );
        data->setFormat( GL_RGBA );
        data->setType(   GL_HALF_FLOAT  );
        data->setColorSpace(_colorSpace); // no auto-detection
    } catch(Iex::BaseExc &e) {
        error() << "EXR loading failed: " << e.what() << std::endl;
        return SharedTextureData();
    }

    return data;
#endif
}

enum loadTextureDataFromPNM_InputDataFormat
{
    PLAIN_TEXT_BITMAP,  // used by P1
    PLAIN_TEXT_DECIMAL, // used by P2 and P3
    BINARY_PACKED,      // used by P4
    BINARY_BYTES,       // used by P5 and P6
    BINARY_WORDS        // used by P5 and P6
};

void loadTextureDataFromPNM_skipWhitespace(std::istream& _in, uint_t _max = std::numeric_limits<uint_t>::max())
{
    uint_t skipped = 0;
    while(isspace(_in.peek()) && skipped < _max)
    {
        _in.ignore(1);
        skipped++;
    }
}

std::istream& loadTextureDataFromPNM_filterPNMComments(std::istream& _in)
{
    loadTextureDataFromPNM_skipWhitespace(_in);

    // When the stream starts with a #, throw away all following characters until the next newline
    // TODO: this does not entirely conform to the PNM specs, where # characters may appear anywhere, not just at the beginning of fields
    while(_in.peek() == '#')
    {
        _in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        loadTextureDataFromPNM_skipWhitespace(_in);
    }
    return _in;
}

SharedTextureData loadTextureDataFromPNM(const std::string& _filename, ColorSpace _colorSpace)
{
    std::ifstream fileStream( _filename.c_str() );

    if (!fileStream.good()) {
        error() << "could not open file " << _filename << std::endl;
        return SharedTextureData();
    }

    SharedTextureData textureData = SharedTextureData( new TextureData() );

    // Read the PNM header
    std::string header;
    loadTextureDataFromPNM_filterPNMComments(fileStream) >> header;

    // The header version determines the format of the data
    loadTextureDataFromPNM_InputDataFormat inputDataFormat = BINARY_BYTES; // set it to something to prevent 'may be uninitialized' warnings (which can't occur here)
    uint_t components;
    if     (header == "P1") { components = 1; inputDataFormat = PLAIN_TEXT_BITMAP; }
    else if(header == "P2") { components = 1; inputDataFormat = PLAIN_TEXT_DECIMAL; }
    else if(header == "P3") { components = 3; inputDataFormat = PLAIN_TEXT_DECIMAL; }
    else if(header == "P4") { components = 1; inputDataFormat = BINARY_PACKED; }
    else if(header == "P5") { components = 1; /* the data format will be determined later */ }
    else if(header == "P6") { components = 3; /* the data format will be determined later */ }
    else
    {
        error() << "could not load " << _filename << ": invalid header" << std::endl;
        fileStream.close();
        return SharedTextureData();
    }

    // Read the width and height of the image
    uint_t width, height;
    loadTextureDataFromPNM_filterPNMComments(fileStream) >> width;
    loadTextureDataFromPNM_filterPNMComments(fileStream) >> height;
    if(!fileStream.good() || width == 0 || height == 0)
    {
        error() << "could not load " << _filename << ": invalid image size" << std::endl;
        fileStream.close();
        return SharedTextureData();
    }

    // The max value is only present in the P2, P3, P5, P6 versions of the PNM format
    uint_t maxValue = 1;
    if(header == "P2" || header == "P3" || header == "P5" || header == "P6")
    {
        loadTextureDataFromPNM_filterPNMComments(fileStream) >> maxValue;

        if(!fileStream.good() || maxValue == 0)
        {
            error() << "could not load " << _filename << ": invalid value range" << std::endl;
            fileStream.close();
            return SharedTextureData();
        }
    }

    // The binary formats P5 and P6 use either bytes or words of data for each element, depending on the maxValue
    if(header == "P5" || header == "P6")
    {
        if     (maxValue <   256) { inputDataFormat = BINARY_BYTES; }
        else if(maxValue < 65536) { inputDataFormat = BINARY_WORDS; }
        else
        {
            error() << "could not load " << _filename << ": max value out of bounds" << std::endl;
            fileStream.close();
            return SharedTextureData();
        }
    }

    // If the image data has maxValue of 255 or 1, we return a GL_UNSIGNED_BYTE texture,
    // otherwise the values are normalized and returned as a GL_FLOAT texture
    GLenum outputDataType = GL_FLOAT;
    if(maxValue == 255 || maxValue == 1)
        outputDataType = GL_UNSIGNED_BYTE;

    // The data section is separated by a single whitespace
    loadTextureDataFromPNM_skipWhitespace(fileStream, 1);

    // Now, read in the data
    // No comments (#) are expected during this section
    unsigned char* data = new unsigned char[width * height * components * getGLTypeSize(outputDataType)];
    uint_t elementsRead = 0;
    while (fileStream.good() && elementsRead < (width * height * components))
    {
        // Read in one element
        // The exact procedure is based on the input data format

        uint_t elem;
        if(inputDataFormat == PLAIN_TEXT_DECIMAL)
        {
            fileStream >> elem;
        }
        else if(inputDataFormat == BINARY_BYTES)
        {
            uint8_t byte;
            fileStream.read((char*)&byte, 1);
            elem = byte;
        }
        else if(inputDataFormat == BINARY_WORDS)
        {
            uint8_t word;
            fileStream.read((char*)&word, 2);
            elem = word;
        }
        else if(inputDataFormat == PLAIN_TEXT_BITMAP)
        {
            loadTextureDataFromPNM_skipWhitespace(fileStream);
            char c;
            fileStream.read(&c, 1);
            if     (c == '0') elem = 0;
            else if(c == '1') elem = 1;
            else              break;
        }
        else if(inputDataFormat == BINARY_PACKED)
        {
            uint_t offset = elementsRead % 8;
            uint8_t byte = fileStream.peek();

            if(byte & 1<<(7-offset)) elem = 1;
            else                     elem = 0;

            if(offset == 7)
                fileStream.ignore(1);
        }
		else elem = 0;

        if(!fileStream.good()) break;

        // Store the element in the data array
        if(outputDataType == GL_UNSIGNED_BYTE)
        {
            if(maxValue == 1) elem *= 255; // Bitmap values are normalized to 0..255
            data[elementsRead * getGLTypeSize(outputDataType)] = (GLubyte)elem;
        }
        else if(outputDataType == GL_FLOAT)
        {
            // Float values are normalized to 0..1
            GLfloat normalized = (GLfloat)elem / (GLfloat)maxValue;
            GLfloat* position = (GLfloat*)&data[elementsRead * getGLTypeSize(outputDataType)];
            *position = normalized;
        }

        elementsRead++;
    }
    fileStream.close();

    if(elementsRead != width * height * components)
    {
        error() << "could not load " << _filename << ": unexpected EOF" << std::endl;
        return SharedTextureData();
    }

    textureData->setData(data); // data will get deleted by the TextureData destructor!
    textureData->setDepth(1);   // 2D so, depth is 1
    textureData->setHeight(height);
    textureData->setWidth(width);
#ifdef ACGL_OPENGL_ES
    textureData->setFormat(GL_RGB);
#else
    textureData->setFormat(components == 3 ? GL_RGB : GL_RED);
#endif
    textureData->setType(outputDataType);
    textureData->setColorSpace(_colorSpace); // no auto-detection

    return textureData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

bool saveTextureDataToRAW( const SharedTextureData &_data, const std::string &_filename )
{
    std::ofstream outFileStream(_filename.c_str(), std::ios_base::out | std::ios_base::binary);

    if(!outFileStream.good())
    {
        error() << "saveTextureDataToRAW: Could not open file " << _filename << std::endl;
        return false;
    }

    outFileStream.write( (char*)_data->getData(), _data->getSizeInBytes() );
    return true;
}

bool saveTextureDataToPPM(const SharedTextureData& _textureData, const std::string &_filename)
{
    std::ofstream outFileStream(_filename.c_str(), std::ios_base::out | std::ios_base::binary);

    if(!outFileStream.good())
    {
        error() << "saveTextureDataToPPM: Could not open file " << _filename << std::endl;
        return false;
    }

    // Use the binary-encoded RGB texture format
    outFileStream << "P6" << std::endl;

    // Width and height
    outFileStream << _textureData->getWidth() << " " << _textureData->getHeight() << std::endl;

    // Maximum value
    uint_t maxValue;
    if(_textureData->getType() == GL_UNSIGNED_BYTE)
    {
        maxValue = 255;
    }
    else
    {
        error() << "saveTextureDataToPPM: Unsupported texture type, must be GL_UNSIGNED_BYTE" << std::endl;
        return false;
    }
    outFileStream << maxValue;

    // Single whitespace as a delimiter
    outFileStream << "\n";

    GLsizei channelCount = _textureData->getNumberOfChannels();

    // Now, write the image data in binary format
    for(GLsizei y = 0; y < _textureData->getHeight(); ++y)
    {
        for(GLsizei x = 0; x < _textureData->getWidth(); ++x)
        {
            GLsizei row = _textureData->getHeight() - y - 1; // from bottom to the top to mirror the image
            GLsizei i = channelCount * (row * _textureData->getWidth() + x) + (row * _textureData->getPadding());

            unsigned char r =                     _textureData->getData()[(i + 0)];
            unsigned char g = (channelCount > 1)? _textureData->getData()[(i + 1)] : 0;
            unsigned char b = (channelCount > 2)? _textureData->getData()[(i + 2)] : 0;

            outFileStream.put( r );
            outFileStream.put( g );
            outFileStream.put( b );
            // ignore alpha if there is an alpha
        }
    }

    outFileStream.close();
    return true;
}

bool saveTextureDataToLodepng( const SharedTextureData &_data, const std::string &_filename )
{
    if (_data->getPadding() != 0) {
        error() << "png saving via lodepng does not support padding per row" << std::endl;
        return false;
    }
    GLenum channelDataType    = _data->getType();
    unsigned int channelCount = _data->getNumberOfChannels();

    int channelBitCount = 0;
    if (channelDataType == GL_BYTE  || channelDataType == GL_UNSIGNED_BYTE)  channelBitCount = 8;
    if (channelDataType == GL_SHORT || channelDataType == GL_UNSIGNED_SHORT) channelBitCount = 16;
    if (channelDataType == GL_INT   || channelDataType == GL_UNSIGNED_INT)   channelBitCount = 32;

    unsigned char *processedData;
    if ( channelBitCount == 8 ) {
       processedData = preProcess<unsigned char>( _data );
    } else if ( channelBitCount == 16 ) {
        processedData = preProcess<unsigned short>( _data );
    } else if ( channelBitCount == 32 ) {
        processedData = preProcess<unsigned int>( _data );

        // no 32 bit save support -> reduce to 16 bit
        uint32_t *original = (uint32_t*) processedData;
        uint16_t *processedData16 = new uint16_t[ channelCount*_data->getWidth() * _data->getHeight() ];

        for (unsigned int i = 0; i < channelCount*_data->getWidth() * _data->getHeight(); ++i) {
            processedData16[i] = (uint16_t) original[i];
        }
        channelBitCount = 16;
        processedData = (unsigned char*) processedData16;

        delete[] original;

    } else {
        error() << "savePNG does not support this data type (yet), could not write " << _filename << std::endl;
        return false;
    }

    // LodePNG expects 16 bit image data in big endian format ==> convert
#ifndef ACGL_BIG_ENDIAN
        if(channelBitCount == 16)
            for(int i = 0; i < _data->getWidth() * _data->getHeight() * _data->getNumberOfChannels() * 2; i += 2)
                std::swap(processedData[i], processedData[i+1]);
#endif

    LodePNGColorType colorType;
    if (channelCount == 1) colorType = LCT_GREY;
    else if (channelCount == 2) colorType = LCT_GREY_ALPHA;
	else if (channelCount == 3) colorType = LCT_RGB;
	else if (channelCount == 4) colorType = LCT_RGBA;
	else colorType = LCT_RGBA;

    unsigned int errorCode = lodepng_encode_file(_filename.c_str(),
                                    processedData, _data->getWidth(), _data->getHeight(),
                                    colorType, channelBitCount );

    delete[] processedData;

    //if there's an error, display it
    if (errorCode) {
        error() << "encoder error " << errorCode << ": "<< lodepng_error_text(errorCode) << std::endl;
        return false;
    }

    return true;
}

#ifdef ACGL_COMPILE_WITH_QT
bool saveTextureDataToQT( const SharedTextureData &_data, const std::string &_filename )
{
    QImage::Format format = QImage::Format_ARGB32;

    if ( pointerAlignment( (void*)_data->getData() ) < 4 ) {
        error() << "image data is not 32bit aligned, QT will not be able to handle that" << std::endl;
        return false;
    }

    GLenum glFormat = _data->getFormat();
    if (glFormat == GL_RGB) {
        format = QImage::Format_RGB888;
        if (_data->getPackAlignment() < 4 ) {
            error() << "scan lines of image data are not 32bit aligned, QT will not be able to handle that" << std::endl;
            return false;
        }
    } else if (glFormat == GL_RGBA || glFormat == GL_BGRA) {
        format = QImage::Format_ARGB32;
    } else {
        error() << "unsupported OpenGL format" << std::endl;
    }

    QImage image = QImage( _data->getData(), _data->getWidth(), _data->getHeight(), format );
    return image.save( QString( _filename.c_str() ) );
}
#endif

} // OpenGL
} // ACGL
