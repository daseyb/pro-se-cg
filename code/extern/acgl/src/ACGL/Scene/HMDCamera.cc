/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Scene/HMDCamera.hh>

namespace ACGL{
namespace Scene{

HMDCamera::HMDCamera()
{
    setStereoMode( GenericCamera::PARALLEL_SHIFT );
    mNeckToEyeVerticalDistance   = 0.0f;
    mNeckToEyeHorizontalDistance = 0.0f;
    mEyeHeight = 1.68f; // average eye height in m for a 178cm person

    mHMDRotation = glm::mat4(1.0f);
}

void HMDCamera::setProjectionCenterOffset( const glm::vec2 &_projectionCenterOffset )
{
    mProjectionCenterOffset = _projectionCenterOffset;
}

void HMDCamera::setNeckToEyeVerticalDistance( float _f )
{
    mNeckToEyeVerticalDistance = _f;
}

void HMDCamera::setNeckToEyeHorizontalDistance( float _f )
{
    mNeckToEyeHorizontalDistance = _f;
}

void HMDCamera::setHMDRotation( const glm::mat3 &_rotation )
{
    //mHMDRotation = glm::mat4( _rotation );
    GenericCamera::setRotationMatrix( _rotation );
}


glm::mat4 HMDCamera::getViewMatrix() const
{
    glm::mat4 viewMatrix = GenericCamera::getViewMatrix();

    glm::vec3 up      = -mNeckToEyeVerticalDistance   * glm::normalize( getUpDirection() );
    glm::vec3 forward = -mNeckToEyeHorizontalDistance * glm::normalize( getForwardDirection() );

    glm::mat4 neck = glm::translate( up ) * glm::translate( forward );

    return neck * mHMDRotation * viewMatrix;
}


glm::mat4 HMDCamera::getProjectionMatrix() const
{
    glm::mat4 projectionOffset;
    if (getEye() == GenericCamera::EYE_RIGHT) {
        projectionOffset = glm::translate( glm::vec3(-mProjectionCenterOffset.x, mProjectionCenterOffset.y, 0.0f) );
    } else {
        projectionOffset = glm::translate( glm::vec3( mProjectionCenterOffset.x, mProjectionCenterOffset.y, 0.0f) );
    }
    return projectionOffset * GenericCamera::getProjectionMatrix(); // yes, from the left side!
}


}
}
