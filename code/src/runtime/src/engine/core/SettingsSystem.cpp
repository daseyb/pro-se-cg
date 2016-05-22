#include <engine/core/SettingsSystem.hpp>
#include <ACGL/ACGL.hh>
#include <ACGL/Base/Settings.hh>

#define PICOJSON_USE_INT64
#include <engine/utils/picojson.h>

#include <fstream>


using namespace ACGL::Base;

std::string getDirectory(std::string filename) {
  size_t found;
  found = filename.find_last_of("/\\");
  return filename.substr(0, found);
}

bool SettingsSystem::startup() {
  Settings::the()->setResourcePath(m_resourcePath);
  Settings::the()->setTexturePath(m_texturePath);
  Settings::the()->setGeometryPath(m_geometryPath);
  Settings::the()->setShaderPath(m_shaderPath);

  
  std::ifstream configFile(m_resourcePath + m_configFilePath);

  if (!configFile.is_open()) {
    std::cerr << "Could not open config file " << m_configFilePath << "!" << std::endl;
    return false;
  }

  picojson::value config;
  configFile >> config;

  if (!config.is<picojson::object>()) {
    std::cerr << "Could not read config file " << m_configFilePath << "!" << std::endl;
    return false;
  }

  const picojson::object& o = config.get<picojson::object>();

  m_availablePlanets = { };
  m_resolution = { 1280, 720 };
  m_qualitySetting = QualitySetting::High;
  m_fullscreen = false;
  m_vsyncEnabled = true;
  m_targetFps = 60;
  m_defaultScene = "AtmosphereTest";
  m_defaultPlanetType = "earth";

#define VALIDATE_TYPE(p, type) if(!p.second.is<type>()) { std::cout << "Warning: Setting \"" << i.first << "\": " << i.second << " is not of expected type " #type "." << std::endl; continue; }

  for (auto& i : o) {
    if (i.first == "available_planet_prefabs") {
      VALIDATE_TYPE(i, picojson::array);
      const picojson::array& planets = i.second.get<picojson::array>();
      m_availablePlanets.clear();
      for (auto& planet : planets) {

        auto planetName = planet.get<std::string>();
        std::string fullPath = m_resourcePath + getDirectory(m_configFilePath) + "/planets/" + planetName + ".json";
        std::ifstream planetFile(fullPath);

        if (!planetFile.is_open()) {
          std::cerr << "Could not open planet file " << fullPath << "!" << std::endl;
          continue;
        }

        picojson::value planetVal;
        planetFile >> planetVal;

        if (!planetVal.is<picojson::object>()) {
          std::cerr << "Could not read planet file " << fullPath << "!" << std::endl;
          return false;
        }

        const picojson::object& planetObj = planetVal.get<picojson::object>();


        PlanetMetaData planetData;
        planetData.name = planetName;
        for (auto& prop : planetObj) {
          if (prop.first == "shaders") {
            VALIDATE_TYPE(prop, std::string);
            planetData.shaders = prop.second.get<std::string>();
          } else if (prop.first == "atmosphere") {
            VALIDATE_TYPE(prop, bool);
            planetData.hasAtmosphere = prop.second.get<bool>();
          } else if (prop.first == "water") {
            VALIDATE_TYPE(prop, bool);
            planetData.hasWater = prop.second.get<bool>();
          } else if (prop.first == "moons") {
            VALIDATE_TYPE(prop, int64_t);
            planetData.moonCount = prop.second.get<int64_t>();
          }
        }

        m_availablePlanets.push_back(planetData);
      }
    } else if(i.first == "resolution") {
      VALIDATE_TYPE(i, picojson::array);
      const picojson::array& res = i.second.get<picojson::array>();
      m_resolution.x = res[0].get<int64_t>();
      m_resolution.y = res[1].get<int64_t>();
    } else if (i.first == "fullscreen") {
      VALIDATE_TYPE(i, bool);
      m_fullscreen = i.second.get<bool>();
    } else if (i.first == "ssao") {
      VALIDATE_TYPE(i, bool);
      m_ssaoEnabled = i.second.get<bool>();
    } else if (i.first == "vsync") {
      VALIDATE_TYPE(i, bool);
      m_vsyncEnabled = i.second.get<bool>();
    } else if (i.first == "target_fps") {
      VALIDATE_TYPE(i, int64_t);
      m_targetFps = i.second.get<int64_t>();
    } else if (i.first == "quality") {
      VALIDATE_TYPE(i, std::string);
      auto setting = i.second.get<std::string>();

      if (setting == "low") m_qualitySetting = QualitySetting::Low;
      else if (setting == "medium") m_qualitySetting = QualitySetting::Medium;
      else if (setting == "high") m_qualitySetting = QualitySetting::High;

    } else if (i.first == "scene") {
      VALIDATE_TYPE(i, std::string);
      m_defaultScene = i.second.get<std::string>();
    } else if (i.first == "default_planet_type") {
      VALIDATE_TYPE(i, std::string);
      m_defaultPlanetType = i.second.get<std::string>();
    } else {
      std::cout << "Warning: Unknown setting \"" << i.first << "\"." << std::endl;
    }
  }
#undef VALIDATE_TYPE

  return true;
}

void SettingsSystem::shutdown() {

}
