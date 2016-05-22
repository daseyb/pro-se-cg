#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/core/WindowSystem.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/audio/AudioSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/core/SimulateEvent.hpp>
#include <engine/ui/imgui.h>

class UISystem : public System {
private:
  EventSystem* m_events;
  WindowSystem* m_window;
  RendererSystem* m_renderer;

  SharedTextureData m_fontTextureData;
  SharedTexture2D m_fontTexture;

  SharedVertexArrayObject m_imguiVao;
  SharedShaderProgram m_imguiProg;
  SharedArrayBuffer m_imguiBuffer;
  SharedElementArrayBuffer m_imguiElements;

  bool m_mousePressed[3];
  float m_mouseWheel;

  void newFrame();
  void renderDrawLists(ImDrawData* drawData);
  void prepareRender();

  void setupGLObjects(ImGuiIO& io);

  bool m_drawUI = false;

public:
  CONSTRUCT_SYSTEM(UISystem) {}

  bool startup() override;
  void shutdown() override;
};