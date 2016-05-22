#pragma once
#include <engine/scene/Entity.hpp>
#include <ACGL/Math/Math.hh>
#include <engine/graphics/DrawCall.hpp>

struct Transform : Component<Transform> {
  Transform::Handle parent;
  
  glm::vec3 lastPosition;
  glm::mat4 lastRotation;
  glm::vec3 lastScale;

  glm::vec3 position;
  glm::mat4 rotation;
  glm::vec3 scale;


  glm::mat4 lastRenderTransform;
  TransformData lastGlobalTransform;
  TransformData thisGlobalTransform;

  Transform() : position(glm::vec3(0, 0, 0)), lastPosition(glm::vec3(0, 0, 0)), lastScale(glm::vec3(1.0, 1.0, 1.0)), scale(glm::vec3(1.0, 1.0, 1.0)) {};
};

