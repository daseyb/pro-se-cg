#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/core/SettingsSystem.hpp>
#include <SDL.h>
#include <glm/glm.hpp>

class WindowSystem : public System {
private:
  SettingsSystem* m_settings;

  void setSDLHintsForOpenGLVersion(unsigned int _version);
  bool createWindow();

  glm::uvec2 m_windowSize;
  SDL_Window *m_sdlWindow;
  SDL_GLContext m_glContext;
  bool m_fullscreen;

public:
  CONSTRUCT_SYSTEM(WindowSystem, int windowWidth, int windowHeight, bool fullscreen) {
    m_windowSize.x = windowWidth;
    m_windowSize.y = windowHeight;
    m_fullscreen = fullscreen;
  }

  void setWindowTitle(const char* title);
  glm::uvec2 getSize() { return m_windowSize; }
  void setSize(glm::uvec2 windowSize) { m_windowSize = windowSize; SDL_SetWindowSize(m_sdlWindow, windowSize.x, m_windowSize.y); }

  SDL_Window* getWindowHandle() { return m_sdlWindow; }
  bool startup() override;
  void shutdown() override;

  void swap();
};