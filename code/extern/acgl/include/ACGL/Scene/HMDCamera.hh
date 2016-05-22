/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/
#pragma once

/**
 * This camera class extends the GenericCamera to better support a
 * HeadMountedDisplay.
 *
 * The usual GenericCamera API controls the position and orientation
 * of the *neck* of the camera while the added API add a head rotation
 * and an offset of the neck to the eyes.
 * These additional values should be set by the HMD hardware.
 *
 * The projection matrix can also handle an offset applied from the *left*
 * to the projection matrix from the GenericCamera. This is needed for
 * some HMDs.
 *
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/GenericCamera.hh>

namespace ACGL{
namespace Scene{

class HMDCamera : public GenericCamera
{
public:

    HMDCamera();
    virtual ~HMDCamera() {}

    //! View plus the neck transformation
    virtual glm::mat4 getViewMatrix() const;

    //! projection of the current eye translated by the projection center offset
    virtual glm::mat4 getProjectionMatrix() const;

    //! projection offset for the left eye, the right eye will mult the X component with -1
    void setProjectionCenterOffset( const glm::vec2 &_projectionCenterOffset );

    /*
     * o = eye
     * x = neck == point of rotation
     *
     *   |---| <- mNeckToEyeHorizontalDistance
     *
     *  /----\
     *  |o   | ---
     *  |    |  |  <- mNeckToEyeVerticalDistance
     *  \__  |  |
     *     |x| ---
     */
    void setNeckToEyeVerticalDistance( float _f );
    void setNeckToEyeHorizontalDistance( float _f );

    void setHMDRotation( const glm::mat3 &_rotation );

    void  setEyeHeight( float _eyeHeight) { if (_eyeHeight>0.0f) mEyeHeight = _eyeHeight; }
    float getEyeHeight() { return mEyeHeight; }


private:
    //! stored as a translation vector
    glm::vec2 mProjectionCenterOffset;

    //! height of the eyes above the neck, just height difference in meters
    float mNeckToEyeVerticalDistance;

    //! distance eyes to neck without the height in meters
    float mNeckToEyeHorizontalDistance;

    //! rotation of the HMD
    glm::mat4 mHMDRotation;

    //! distance floor to eyes in meters (player height is a few cm more)
    float mEyeHeight;
};
ACGL_SMARTPOINTER_TYPEDEFS(HMDCamera)

}
}
