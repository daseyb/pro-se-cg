/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/UniformBuffer.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>

using namespace ACGL;
using namespace ACGL::OpenGL;

#if (ACGL_OPENGL_VERSION >= 31)

UniformBuffer::UniformBuffer( const ptr::shared_ptr<const ShaderProgram> &_shaderProgram, const std::string &_uboName ) : Buffer(GL_UNIFORM_BUFFER)
{
    mBlockName = _uboName;
    reserveMemory(     _shaderProgram->getUniformBlockSize(      _uboName ) );
    setUniformOffsets( _shaderProgram->getUniformOffsetsOfBlock( _uboName ) ); // to enable intuitive setUniform functions
    bindBufferBase(    _shaderProgram->getUniformBlockBinding(   _uboName ) ); // bind the UBO to the binding point where the shader expects it
}

#endif // OpenGL 3.1
