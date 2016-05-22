#pragma once
#include <engine/scene/Entity.hpp>
#include <ACGL/Math/Math.hh>
#include <engine/graphics/DrawCall.hpp>

struct Transform : Component<Transform> {
  Transform::Handle parent;
  
  glm::dvec3 lastPosition;
  glm::dmat4 lastRotation;
  glm::dvec3 lastScale;

  glm::dvec3 position;
  glm::dmat4 rotation;
  glm::dvec3 scale;


  glm::dmat4 lastRenderTransform;
  TransformData lastGlobalTransform;
  TransformData thisGlobalTransform;

  Transform() : position(glm::dvec3(0, 0, 0)), lastPosition(glm::dvec3(0, 0, 0)), lastScale(glm::dvec3(1.0, 1.0, 1.0)), scale(glm::dvec3(1.0, 1.0, 1.0)) {};
};

