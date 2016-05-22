/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/OpenGL/Objects/ProgramPipeline.hh>

using namespace ACGL::OpenGL;

#if (ACGL_OPENGL_VERSION >= 41)

ProgramPipeline::ProgramPipeline()
{
    mObjectName = 0;
    glGenProgramPipelines(1, &mObjectName);
}

ProgramPipeline::~ProgramPipeline(void)
{
    // value 0 will get ignored by OpenGL
    glDeleteProgramPipelines(1, &mObjectName);
}

#endif // OpenGL >= 4.1

