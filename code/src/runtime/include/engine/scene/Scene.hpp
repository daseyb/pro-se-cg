#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/core/SettingsSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/audio/AudioSystem.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/ui/UISystem.hpp>


#define CONSTRUCT_SCENE(scene) scene(Context* context, std::string iniFile) : Scene(context, iniFile, #scene) 


class Scene : public System {
private:
  std::string m_name;

protected:
  SettingsSystem* m_settings;
  EventSystem* m_events;
  RendererSystem* m_renderer;
  SceneGraphSystem* m_sceneGraph;
  AudioSystem* m_audio;

  std::unordered_map<std::string, void*> m_iniValues;

public:


  CONSTRUCT_SYSTEM(Scene, std::string iniFile, std::string name), m_name(name) { }
  std::string getName() { return m_name; }

  bool startup() override {
    RESOLVE_DEPENDENCY(m_settings);
    RESOLVE_DEPENDENCY(m_events);
    RESOLVE_DEPENDENCY(m_renderer);
    RESOLVE_DEPENDENCY(m_sceneGraph);
    RESOLVE_DEPENDENCY(m_audio);
    return true;
  }
};