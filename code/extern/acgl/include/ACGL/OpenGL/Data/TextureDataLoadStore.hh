/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_DATA_TEXTUREDATALOADSTORE_HH
#define ACGL_OPENGL_DATA_TEXTUREDATALOADSTORE_HH

/**
 * Helper function for writing the contents of a TextureData object into a file
 * and loading them from a file.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Data/ColorSpace.hh>
#include <ACGL/OpenGL/Data/TextureData.hh>

#include <string>
#include <map>
#include <vector>

namespace ACGL{
namespace OpenGL{

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               generic load/save
///////////////////////////////////////////////////////////////////////////////////////////////////

//! functionpointer which can be used for generic Texture loading
typedef SharedTextureData (*TextureLoadFuncPtr)(const std::string&, ColorSpace);

//! registers a functionpointer to be used for loading specified file types (E.G. provide a load function which loads compressed)
void registerTextureLoadFunction(std::vector<std::string> _endings, TextureLoadFuncPtr _function);

//! remove a registered functionpointer (if you want to use the stock ones again
void unregisterTextureLoadFunction(TextureLoadFuncPtr _function);

//! generic load function that will use one of the loading functions below based on the file ending
SharedTextureData loadTextureData(const std::string &_filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

//! generic save function that will use one of the saving functions below based on the file ending
bool saveTextureData(const SharedTextureData &_textureData, const std::string &_filename);

//! saves the viewport visible part of the framebuffer 0 to a file named _fileName. The file ending determines the file type
bool saveScreenshot( const std::string& _fileName );

//! saves the viewport visible part of the framebuffer 0 to a file named _prefix_DATE-TIME._fileEnding
bool saveScreenshotWithDate( const std::string& _prefix, const std::string& _fileEnding);

//! saves the viewport visible part of the framebuffer 0 to a file named screenshot_DATE-TIME._fileEnding
inline bool saveScreenshotWithDate( const std::string& _fileEnding = "png" ) {
    return saveScreenshotWithDate( "screenshot", _fileEnding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific load
///////////////////////////////////////////////////////////////////////////////////////////////////

//! loads from a PNG using the simple lodepng library
SharedTextureData loadTextureDataFromLodepng(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

#ifdef ACGL_COMPILE_WITH_QT
//! loads various formats from the QT library
SharedTextureData loadTextureDataFromQT(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);
#endif

//! loads RGBE aka Radiance files
SharedTextureData loadTextureDataFromRGBE(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

//! loads EXR / OpenEXR files iff the library is present AT RUNTIME (linux only)
SharedTextureData loadTextureDataFromEXR(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

//! loads PNM / PPM files:
SharedTextureData loadTextureDataFromPNM(const std::string& _filename, ColorSpace _colorSpace = ColorSpace::AUTO_DETECT);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                           library specific save
///////////////////////////////////////////////////////////////////////////////////////////////////

//! saves to a PPM file
bool saveTextureDataToPPM(const SharedTextureData &_textureData, const std::string &_filename);

//! save to a PNG file with lodepng
bool saveTextureDataToLodepng( const SharedTextureData &_data, const std::string &_filename );

//! save the imagedata raw to a file
bool saveTextureDataToRAW( const SharedTextureData &_data, const std::string &_filename );

#ifdef ACGL_COMPILE_WITH_QT
bool saveTextureDataToQT( const SharedTextureData &_data, const std::string &_filename );
#endif


// helper for saveTextureDataToLodepng
template< typename T>
unsigned char *preProcess( const SharedTextureData &_data)
{
    unsigned int channelCount = _data->getNumberOfChannels();

    unsigned int pixelCount = _data->getWidth() * _data->getHeight();
    T *processedrawdata = new T[pixelCount * channelCount];

    // copy & flip the image:
    T *originalrawdata  = (T*) _data->getData();

    for (int i = 0; i < _data->getHeight(); ++i) {
        size_t srcOffset = _data->getWidth() * i * channelCount;
        size_t dstOffset = _data->getWidth() * (_data->getHeight()-i-1) * channelCount;

        memcpy( processedrawdata + dstOffset, originalrawdata + srcOffset, _data->getWidth()*channelCount * sizeof(T) );
    }

    return (unsigned char *) processedrawdata;
}

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_DATA_TEXTUREDATALOADSTORE_HH
