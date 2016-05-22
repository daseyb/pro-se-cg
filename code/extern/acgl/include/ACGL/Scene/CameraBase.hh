/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/
#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>

namespace ACGL{
namespace Scene{

/**
 * @brief Common interface for cameras
 *
 * This interface only contains getter on purpose.
 * All logic that wants to modify a camera should know the actual structure of the camera and therefore use the
 * specific subclass.
 */
class CameraBase
{
protected:
    CameraBase();
public:
    virtual ~CameraBase();

    /**
     * @brief gets the Position of the camera
     * @return a 3-dimensional position in the global coordinate system
     */
    virtual glm::vec3 getPosition() const = 0;
    /**
     * @brief gets the ViewMatrix of the camera
     * @return a 4x4 matrix containing projection independent camera transforms
     */
    virtual glm::mat4 getViewMatrix() const = 0;
    /**
     * @brief gets the ProjectionMatrix of the camera
     * @return a 4x4 matrix containing the projection into normalized device coordinates
     */
    virtual glm::mat4 getProjectionMatrix() const = 0;
    /**
     * @brief gets the ViewportSize of the current viewport of this camera
     * @return the 2-dimensional size of the viewport
     */
    virtual glm::uvec2 getViewportSize() const = 0;
};

}
}
