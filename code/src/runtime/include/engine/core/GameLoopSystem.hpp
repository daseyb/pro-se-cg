#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/core/WindowSystem.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/audio/AudioSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/core/SettingsSystem.hpp>
#include <engine/core/SimulateEvent.hpp>

#include <array>

class GameLoopSystem : public System {
private:
  SettingsSystem* m_settings;
  EventSystem* m_events;
  WindowSystem* m_window;
  RendererSystem* m_renderer;
  AudioSystem* m_audio;

  uint32_t m_targetFrameRate;
  uint32_t m_maxFrameTime;

  double m_lastSimulateFrameTime;
  double m_lastPerpareDrawTime;
  double m_lastDrawTime;
  double m_lastSwapTime;

  std::array<float, 50> m_frameTimeHistory;

public:
  CONSTRUCT_SYSTEM(GameLoopSystem, Uint32 targetFrameRate, Uint32 maxFrameTime) {
    m_targetFrameRate = targetFrameRate;
    m_maxFrameTime = maxFrameTime;
    m_lastSimulateFrameTime = 0;

    m_frameTimeHistory = { 0 };
  }

  bool startup() override;
  void shutdown() override;

  void run();
};