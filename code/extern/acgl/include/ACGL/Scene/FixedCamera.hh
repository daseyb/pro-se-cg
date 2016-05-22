#pragma once

#include "CameraBase.hh"

namespace ACGL{
namespace Scene{

/**
 * @brief A fixed camera
 */
class FixedCamera : public CameraBase
{
private:

    glm::vec3 mPosition;
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
    glm::uvec2 mViewportSize;

public:
    /// CAUTION: default ctor with zero values
    FixedCamera();
    FixedCamera(const glm::vec3 &_pos, const glm::mat4 &_view, const glm::mat4 &_proj, const glm::uvec2 &_viewport);

    // Getter, Setter for Camera Position
    virtual glm::vec3 getPosition() const { return mPosition; }
    virtual void setPosition(glm::vec3 const& _val) { mPosition = _val; }

    // Getter, Setter for Camera ViewMatrix
    virtual glm::mat4 getViewMatrix() const { return mViewMatrix; }
    virtual void setViewMatrix(glm::mat4 const& _val) { mViewMatrix = _val; }

    // Getter, Setter for Camera ProjectionMatrix
    virtual glm::mat4 getProjectionMatrix() const { return mProjectionMatrix; }
    virtual void setProjectionMatrix(glm::mat4 const& _val) { mProjectionMatrix = _val; }

    // Getter, Setter for Camera ViewportSize
    virtual glm::uvec2 getViewportSize() const { return mViewportSize; }
    virtual void setViewportSize(glm::uvec2 const& _val) { mViewportSize = _val; }
};

}
}
