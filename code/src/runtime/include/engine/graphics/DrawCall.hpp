#pragma once
#include <engine/graphics/Geometry.hpp>
#include <engine/graphics/Material.hpp>


struct TransformData {
  glm::dvec3 pos;
  glm::quat rot;
  glm::dvec3 scale;
};

struct DrawCall {
  Material material;
  Geometry geometry;
  glm::dmat4 lastRenderTransform;
  glm::dmat4 thisRenderTransform;
	int recursionDepth;
};