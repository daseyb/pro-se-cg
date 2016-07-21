#pragma once
#include <glow/fwd.hh>

struct Material {
  glm::vec3 diffuseColor;
  float roughness;
  glm::vec3 emissiveColor;
  float refractiveness;
  glm::vec3 specularColor;
  float eta;
  glow::SharedTexture2D diffuseTexture;
  glow::SharedTexture2D specularTexture;
  glow::SharedTexture2D emissiveTexture;
};