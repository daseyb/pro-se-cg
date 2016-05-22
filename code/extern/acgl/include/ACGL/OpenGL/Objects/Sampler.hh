/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_OBJECTS_SAMPLER_HH
#define ACGL_OPENGL_OBJECTS_SAMPLER_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/OpenGL/Debug.hh>

#include <ACGL/Math/Math.hh>

#if (ACGL_OPENGL_VERSION >= 33)
namespace ACGL{
namespace OpenGL{

/**
 * OpenGL Sampler Objects (needs OpenGL 3.3)
 *
 * A Sampler holds information about how to sample in textures (bi-linear, anisotrophic, wrap modes etc).
 * Instead of setting these informations for each texture, a sampler object can be used and bound to
 * the texture units, this will overwrite the sampling information in the texture object itself.
 * This way, one texture can be bound to two texture units and two different samplers can be bound to those
 * units to provide different sampling behavior from the same texture in one shaderpass.
 * Similar, different textures can use the same sampler so only one object has to be changed if for example
 * the number of anisotrophic filter-samples should get changed (instead of changing all texture objects).
 *
 * All default parameters of the setters used to change the sampling behavior are the OpenGL defaults as well.
 *
 * If needed, getters could be added that query the settings from GL, so no caching is needed here (that might
 * be something for a derived class with larger memory footprint but faster access to the settings).
 */


class Sampler {
    ACGL_NOT_COPYABLE(Sampler)
public:
    Sampler();
    ~Sampler(void);

    // ===================================================================================================== \/
    // =========================================================================================== KHR_DEBUG \/
    // ===================================================================================================== \/
public:
    // Sets and gets a label visible inside of a OpenGL debugger if KHR_debug is supported at runtime *and*
    // if ACGL_OPENGL_DEBUGGER_SUPPORT was defined during compile time. Does nothing otherwise!
#ifdef ACGL_OPENGL_DEBUGGER_SUPPORT
    void setObjectLabel( const std::string &_label ) { setObjectLabelT<GL_SAMPLER>(getObjectName(),_label); }
    std::string getObjectLabel() { return getObjectLabelT<GL_SAMPLER>(getObjectName()); }
#else
    void setObjectLabel( const std::string & ) {}
    std::string getObjectLabel() { return ""; }
#endif

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline GLuint getObjectName(void) const { return mObjectName; }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
    //! _textureUnit is in the range 0..n (not GL_TEXTURE0..GL_TEXTUREn)
	inline void bind(GLuint _textureUnit) const
    {
        glBindSampler( _textureUnit, mObjectName ); // yes, no adding of GL_TEXTURE0 !
    }

	//! unbinds the texture sampler (if there is one bound) from _textureUnit
	inline static void unBind( GLuint _textureUnit ) { glBindSampler( _textureUnit, 0 ); }

    //! generic parameter setting
    inline void parameter( GLenum _parameterName, GLint   _parameter ) { glSamplerParameteri( mObjectName, _parameterName, _parameter ); }
    inline void parameter( GLenum _parameterName, GLfloat _parameter ) { glSamplerParameterf( mObjectName, _parameterName, _parameter ); }

	inline void setBorderColor( glm::vec4 _color = glm::vec4(0.0f,0.0f,0.0f,0.0f) ) {
		glSamplerParameterfv( mObjectName, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(_color) );
	}
    inline void setMinFilter(GLenum _value = GL_NEAREST_MIPMAP_LINEAR) { glSamplerParameteri( mObjectName, GL_TEXTURE_MIN_FILTER, _value); }
    inline void setMagFilter(GLenum _value = GL_LINEAR)                { glSamplerParameteri( mObjectName, GL_TEXTURE_MAG_FILTER, _value); }

    void setWrap(  GLenum _wrapS );
    void setWrap(  GLenum _wrapS, GLenum _wrapT );
    void setWrap(  GLenum _wrapS, GLenum _wrapT, GLenum _wrapR );
    inline void setWrapS( GLenum _wrapS = GL_REPEAT )                  { glSamplerParameteri( mObjectName, GL_TEXTURE_WRAP_S,     _wrapS); }
	inline void setWrapT( GLenum _wrapT = GL_REPEAT )                  { glSamplerParameteri( mObjectName, GL_TEXTURE_WRAP_T,     _wrapT); }
	inline void setWrapR( GLenum _wrapR = GL_REPEAT )                  { glSamplerParameteri( mObjectName, GL_TEXTURE_WRAP_R,     _wrapR); }

	inline void setMinLOD(  GLint _lod = -1000 )   { glSamplerParameteri( mObjectName, GL_TEXTURE_MIN_LOD, _lod); }
	inline void setMaxLOD(  GLint _lod =  1000 )   { glSamplerParameteri( mObjectName, GL_TEXTURE_MAX_LOD, _lod); }
	inline void setLODBias( GLfloat _bias = 0.0f ) { glSamplerParameterf( mObjectName, GL_TEXTURE_LOD_BIAS, _bias); }
	inline void setCompareMode( GLenum _mode = GL_NONE )   { glSamplerParameteri( mObjectName, GL_TEXTURE_COMPARE_MODE, _mode); }
	inline void setCompareFunc( GLenum _func = GL_LEQUAL ) { glSamplerParameteri( mObjectName, GL_TEXTURE_COMPARE_FUNC, _func); }

    //! _sampleCount = 1.0 to deactivate anisotrop filtering, maximum is often 16. If a value is too high it will get clamped to the maximum
    void setMaxAnisotropy( GLfloat _sampleCount = 1.0f );

    ///////////////////////////////////////////////////////////////////////////////////////
    //                                                                           getter
    ///////////////////////////////////////////////////////////////////////////////////////
    //! generic getter
    GLint   getParameterI( GLenum _parameterName );
    GLfloat getParameterF( GLenum _parameterName );

    GLenum  getMinFilter()   { return (GLenum) getParameterI( GL_TEXTURE_MIN_FILTER ); }
    GLenum  getMagFilter()   { return (GLenum) getParameterI( GL_TEXTURE_MAG_FILTER ); }
    GLenum  getWrapS()       { return (GLenum) getParameterI( GL_TEXTURE_WRAP_S ); }
    GLenum  getWrapT()       { return (GLenum) getParameterI( GL_TEXTURE_WRAP_T ); }
    GLenum  getWrapR()       { return (GLenum) getParameterI( GL_TEXTURE_WRAP_R ); }
    GLfloat getMinLOD()      { return getParameterF( GL_TEXTURE_MIN_LOD      ); }
    GLfloat getMaxLOD()      { return getParameterF( GL_TEXTURE_MAX_LOD      ); }
    GLfloat getLODBias()     { return getParameterF( GL_TEXTURE_LOD_BIAS     ); }
    GLint   getCompareMode() { return getParameterI( GL_TEXTURE_COMPARE_MODE ); }
    GLint   getCompareFunc() { return getParameterI( GL_TEXTURE_COMPARE_FUNC ); }

    glm::vec4 getBorderColor();

private:
    GLuint mObjectName;
};

ACGL_SMARTPOINTER_TYPEDEFS(Sampler)

} // OpenGL
} // ACGL

#endif // OpenGL >= 3.3

#endif // ACGL_OPENGL_OBJECTS_SAMPLER_HH
