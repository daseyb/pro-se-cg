#pragma once

struct Material {
  glm::vec3 diffuseColor;
  float roughness;
  glm::vec3 emissiveColor;
  float refractiveness;
  glm::vec3 specularColor;
  float eta;
};