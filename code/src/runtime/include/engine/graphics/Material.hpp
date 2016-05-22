#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <engine/graphics/RenderQueue.hpp>

using namespace ACGL::OpenGL;

struct Material {
  glm::vec4 tintColor;
  glm::vec4 emissiveColor;
  ConstSharedTextureBase mainTexture;
  ConstSharedTextureBase normalTexture;
  ConstSharedTextureBase specularSmoothnessTexture;
  ConstSharedTextureBase emissiveTexture;
  SharedShaderProgram prog;
  bool castShadow;
  RenderQueue queue;
  GLenum cullSide;
};