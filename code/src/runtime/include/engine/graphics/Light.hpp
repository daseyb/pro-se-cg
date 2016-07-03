#pragma once
#include <engine/scene/Entity.hpp>

enum class LightType {
  POINT,
  DIRECTIONAL
};

struct Light : Component<Light> {
    Light(glm::vec4 color, float size, glm::vec3 dir = glm::vec3(0),
        bool castShadow = true, LightType type = LightType::POINT,
        uint32_t renderPassIndex = 0)
      : color(color), size(size), dir(dir), castShadow(castShadow), type(type),
        renderPassIndex(renderPassIndex) {}

  glm::vec4 color;
  float size;
  glm::vec3 dir;
  bool castShadow;
  LightType type;
  uint32_t renderPassIndex;
};