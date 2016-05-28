#pragma once
#include <engine/graphics/Geometry.hpp>
#include <engine/graphics/Material.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformData {
  glm::vec3 pos;
  glm::quat rot;
  glm::vec3 scale;
};

struct DrawCall {
  Material material;
  Geometry geometry;
  glm::mat4 lastRenderTransform;
  glm::mat4 thisRenderTransform;
};