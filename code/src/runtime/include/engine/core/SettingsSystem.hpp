#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <ACGL/Math/Math.hh>

enum class QualitySetting {
  Low = 0,
  Medium,
  High
};

struct PlanetMetaData {
  std::string name;
  std::string shaders;
  bool hasAtmosphere;
  bool hasWater;
  uint64_t moonCount;
};


class SettingsSystem : public System {
private:
  std::string m_resourcePath;
  std::string m_texturePath;
  std::string m_geometryPath;
  std::string m_shaderPath;
  std::string m_soundPath;

  std::string m_configFilePath;

  std::vector<PlanetMetaData> m_availablePlanets;

  glm::ivec2 m_resolution;
  QualitySetting m_qualitySetting;
  bool m_fullscreen;
  bool m_ssaoEnabled;
  bool m_vsyncEnabled;
  uint64_t m_targetFps;
  std::string m_defaultScene;
  std::string m_defaultPlanetType;

public:
  CONSTRUCT_SYSTEM(SettingsSystem, std::string resourcePath,
                   std::string texturePath, std::string geometryPath,
                   std::string shaderPath, std::string soundPath, std::string configFilePath)
    , m_resourcePath(resourcePath), m_texturePath(texturePath),
      m_geometryPath(geometryPath), m_shaderPath(shaderPath),
      m_soundPath(soundPath) {

    m_configFilePath = configFilePath;
  }

  inline std::string getResourcePath() const { return m_resourcePath; }
  inline std::string getFullTexturePath() const { return m_resourcePath + m_texturePath; }
  inline std::string getFullGeometryPath() const { return m_resourcePath + m_geometryPath; }
  inline std::string getFullShaderPath() const { return m_resourcePath + m_shaderPath; }
  inline std::string getFullSoundPath() const { return m_resourcePath + m_soundPath; }
  inline glm::ivec2 getResolution() const { return m_resolution; }
  inline bool getFullscreen() const { return m_fullscreen; }
  inline bool ssaoEnabled() const { return m_ssaoEnabled; }
  inline bool vsyncEnabled() const { return m_vsyncEnabled; }
  inline uint64_t getTargetFps() const { return m_targetFps; }
  inline QualitySetting getQualitySetting() const { return m_qualitySetting; }
  inline std::string getDefaultScene() const { return m_defaultScene; }
  inline std::string getDefaultPlanetType() const { return m_defaultPlanetType; }

  inline std::vector<PlanetMetaData> getAvailablePlanets() const { return m_availablePlanets; }
  inline PlanetMetaData getPlanet(std::string name) const { 
    for (auto& planet : m_availablePlanets) {
      if (planet.name == name) {
        return planet;
      }
    }
    return{};
  }

  bool startup() override;
  void shutdown() override;
};