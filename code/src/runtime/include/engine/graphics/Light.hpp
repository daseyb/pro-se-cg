#include <engine/scene/Entity.hpp>
#include <ACGL/Math/Math.hh>

enum class LightType {
  POINT,
  DIRECTIONAL
};

struct Light : Component<Light> {
  Light(glm::vec4 color, glm::vec3 dir, bool castShadow, LightType type = LightType::POINT, uint32_t renderPassIndex = 0) : color(color), dir(dir), castShadow(castShadow), type(type), renderPassIndex(renderPassIndex) {}

  glm::vec4 color;
  glm::vec3 dir;
  bool castShadow;
  LightType type;
  uint32_t renderPassIndex;
};