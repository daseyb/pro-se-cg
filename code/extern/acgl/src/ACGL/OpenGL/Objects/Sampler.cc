/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/Sampler.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh> // for anisotrophic filtering

#if (ACGL_OPENGL_VERSION >= 33)

using namespace ACGL::OpenGL;

Sampler::Sampler()
{
    mObjectName = 0;
    glGenSamplers(1, &mObjectName);
}

Sampler::~Sampler(void)
{
    // sampler 0 will get ignored by OpenGL
    glDeleteSamplers(1, &mObjectName);
}

void Sampler::setMaxAnisotropy( GLfloat _sampleCount )
{
    if ( ACGL_EXT_texture_filter_anisotrophic() ) {
		// anisotrophic filtering is supported:
        _sampleCount = std::max( _sampleCount, 1.0f );
        _sampleCount = std::min( _sampleCount, ACGL_MAX_TEXTURE_MAX_ANISOTROPY );
        glSamplerParameterf( mObjectName, GL_TEXTURE_MAX_ANISOTROPY_EXT, _sampleCount );
    } else
    {
		if (_sampleCount != 1.0f ) {
			// anisotropic filtering will just increase the image quality, so this is just
			// a warning and no error, 1 sample is equal to no anisotrophic filtering, so ignore
			// that case entirely!
			ACGL::Utils::warning() << "Anisotropic filtering is not supported, ignored" << std::endl;
		}
    }
}

void Sampler::setWrap(GLenum _wrapS)
{
    setWrapS(_wrapS);
}

void Sampler::setWrap(GLenum _wrapS, GLenum _wrapT)
{
    setWrapS(_wrapS);
    setWrapT(_wrapT);
}

void Sampler::setWrap(GLenum _wrapS, GLenum _wrapT, GLenum _wrapR)
{
    setWrapS(_wrapS);
    setWrapT(_wrapT);
    setWrapR(_wrapR);
}

GLint   Sampler::getParameterI( GLenum _parameterName )
{
    GLint value;
    glGetSamplerParameteriv( mObjectName, _parameterName, &value );
    return value;
}

GLfloat Sampler::getParameterF( GLenum _parameterName )
{
    GLfloat value;
    glGetSamplerParameterfv( mObjectName, _parameterName, &value );
    return value;
}

glm::vec4 Sampler::getBorderColor()
{
    glm::vec4 color;
    glGetSamplerParameterfv( mObjectName, GL_TEXTURE_BORDER_COLOR, &(color[0]) );
    return color;
}

#endif // OpenGL >= 3.3
