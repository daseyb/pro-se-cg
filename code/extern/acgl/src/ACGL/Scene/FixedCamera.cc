/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/
#include <ACGL/Scene/FixedCamera.hh>


namespace ACGL{
namespace Scene{

FixedCamera::FixedCamera()
{
}

FixedCamera::FixedCamera(const glm::vec3 &_pos, const glm::mat4 &_view, const glm::mat4 &_proj, const glm::uvec2 &_viewport) :
    mPosition(_pos),
    mViewMatrix(_view),
    mProjectionMatrix(_proj),
    mViewportSize(_viewport)
{
}

}
}
