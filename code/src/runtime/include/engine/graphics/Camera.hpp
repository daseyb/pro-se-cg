#pragma once
#include <engine/scene/Entity.hpp>

#undef near
#undef far
#undef up

enum class ProjectionMode {
  Perspective,
  Orthographic
};

struct Camera : Component<Camera> {
  explicit Camera() : fov(75), near(0.1f), far(100), worldUp(glm::dvec3(0, 1, 0)), isMain(false), renderPassIndex(std::numeric_limits<uint32_t>::max()) {}
  explicit Camera(float fov, float near, float far, glm::dvec3 up = glm::dvec3(0, 1, 0), bool isMain = false, uint32_t passIndex = std::numeric_limits<uint32_t>::max())
    : fov(fov), near(near), far(far), worldUp(up), isMain(isMain), renderPassIndex(passIndex) { }

  float fov;
  float near;
  float far;
  glm::dvec3 worldUp;
  bool isMain;
  uint32_t renderPassIndex;
  ProjectionMode mode = ProjectionMode::Perspective;
};

