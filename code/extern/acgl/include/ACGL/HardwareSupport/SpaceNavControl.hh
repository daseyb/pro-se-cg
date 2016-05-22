/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/GenericCamera.hh>

namespace ACGL{
namespace HardwareSupport{

/// Controls a GenericCamera with a space navigator
class SpaceNavControl
{
public:
    SpaceNavControl(Scene::GenericCamera *_cam, const glm::vec3 &_moveSensitivity = glm::vec3(1.0), const glm::vec3 &_rotateSensitivity = glm::vec3(1.0));
    ~SpaceNavControl();

    /// Updates the camera: call this once a frame, it will poll the SpaceNavigator. The elapsed time is used to scale the movement to
    /// to be framerate independent.
    void update(float _elapsedSeconds);

private:
    /// The referenced camera
    Scene::GenericCamera *mCamera;

    /// Sensitivity for moving the camera
    glm::vec3 mMoveSensitivity;
    /// Sensitivity for rotating the camera
    glm::vec3 mRotateSensitivity;
};

} // HardwareSupport
} // ACGL
