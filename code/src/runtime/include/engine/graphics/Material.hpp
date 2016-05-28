#pragma once
#include <engine/graphics/RenderQueue.hpp>

struct Material {
  glm::vec4 tintColor;
  glm::vec4 emissiveColor;
  RenderQueue queue;
};