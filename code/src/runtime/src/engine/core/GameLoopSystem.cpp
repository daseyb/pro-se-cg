#include <engine/core/GameLoopSystem.hpp>
#include <engine/events/DrawEvent.hpp>
#include <engine/events/PrepareDrawEvent.hpp>
#include <engine/utils/Remotery.h>

#include <engine/events/ControllerEvent.hpp>
#include <engine/events/JoystickEvent.hpp>
#include <engine/events/KeyboardEvent.hpp>
#include <engine/events/MouseEvent.hpp>
#include <engine/events/TouchEvent.hpp>
#include <engine/events/WindowEvent.hpp>
#include <engine/events/ResizeWindowEvent.hpp>
#include <engine/ui/UISystem.hpp>

bool GameLoopSystem::startup() {
  RESOLVE_DEPENDENCY(m_settings);
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_window);
  RESOLVE_DEPENDENCY(m_renderer);
  RESOLVE_DEPENDENCY(m_audio);

  m_targetFrameRate = m_settings->getTargetFps();

  m_events->subscribe<"DrawUI"_sh>([this] {
    static bool opened = true;
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    if (!ImGui::Begin("Frame Statistics", &opened, ImVec2(275, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
      ImGui::End();
      return;
    }

    static int frameTimeIndex = 0;
    double totalFrameTime = m_lastSimulateFrameTime + m_lastPerpareDrawTime + m_lastDrawTime + m_lastSwapTime;

    m_frameTimeHistory[frameTimeIndex] = (float)totalFrameTime;
    frameTimeIndex = (frameTimeIndex + 1) % m_frameTimeHistory.size();
    
    ImGui::Text("FPS:          %d", (int)(1000.0f/(totalFrameTime)));
    ImGui::Separator();
    ImGui::PlotHistogram("", m_frameTimeHistory.data(), m_frameTimeHistory.size(), 0, NULL, 0, 100, glm::vec2(250, 100));
    ImGui::Separator();
    ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    ImGui::End();
  });

  return true;
}

void GameLoopSystem::run() {
  //Main loop flag
  bool quit = false;

  //Event handler
  SDL_Event e;

  //Enable text input
  SDL_StartTextInput();

  Uint32 currentTime = SDL_GetTicks();
  

  Uint32 t = 0;
  Uint32 dt = Uint32(1000.0f / m_targetFrameRate);
	Uint32 accumulator = dt;


  do {
    rmt_ScopedCPUSample(GameLoop, 0);

    Uint32 newTime = SDL_GetTicks();
    Uint32 frameTime = newTime - currentTime;

    if (frameTime > m_maxFrameTime) {
      frameTime = m_maxFrameTime;
    }

    currentTime = newTime;
    accumulator += frameTime;

    m_events->fire<"StartFrame"_sh>();

    rmt_BeginCPUSample(Simulate, 0);
    while (accumulator >= dt) {
      m_events->fire<"StartSimulate"_sh>();
      m_events->fire<SimulateEvent>({float(dt)/1000, float(t)/1000});

      //Handle events on queue
      while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
          quit = true;
        }

        switch (e.type) {
          case SDL_WINDOWEVENT:
            m_events->fire<WindowEvent>({ e });
            if (e.window.type == SDL_WINDOWEVENT_RESIZED) {
              m_events->fire<ResizeWindowEvent>(ResizeWindowEvent{ { e.window.data1, e.window.data2 } });
            }
            break;
          case SDL_KEYDOWN:
          case SDL_KEYUP:
          case SDL_TEXTEDITING:
          case SDL_TEXTINPUT:
            m_events->fire<KeyboardEvent>({ e });
            break;
          case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONUP:
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEWHEEL:
            m_events->fire<MouseEvent>({ e });
            break;
          case SDL_JOYAXISMOTION:
          case SDL_JOYBALLMOTION:
          case SDL_JOYHATMOTION:
          case SDL_JOYBUTTONDOWN:
          case SDL_JOYBUTTONUP:
          case SDL_JOYDEVICEADDED:
          case SDL_JOYDEVICEREMOVED:
            m_events->fire<JoystickEvent>({ e });
            break;
          case SDL_CONTROLLERAXISMOTION:
          case SDL_CONTROLLERBUTTONDOWN:
          case SDL_CONTROLLERBUTTONUP:
          case SDL_CONTROLLERDEVICEADDED:
          case SDL_CONTROLLERDEVICEREMOVED:
          case SDL_CONTROLLERDEVICEREMAPPED:
            m_events->fire<ControllerEvent>({ e });
            break;
          case SDL_FINGERDOWN:
          case SDL_FINGERUP:
          case SDL_FINGERMOTION:
            m_events->fire<TouchEvent>({ e });
            break;
        }
      }
      m_audio->update();
      t += dt;
      accumulator -= dt;
    }
    rmt_EndCPUSample();

    rmt_BeginCPUSample(PrepareDraw, 0);
    const double alpha = double(accumulator) / dt;
    m_events->fire<PrepareDrawEvent>({ alpha, float(t) / 1000 });
    rmt_EndCPUSample();

    rmt_BeginCPUSample(Draw, 0);
    m_events->fire<DrawEvent>({ alpha, float(t) / 1000 });
    rmt_EndCPUSample();

    rmt_BeginCPUSample(Swap, 0);
    m_window->swap();
    rmt_EndCPUSample();

    rmt_BeginCPUSample(EndFrame, 0);
    m_events->fire<"EndFrame"_sh>();
    rmt_EndCPUSample();

  } // Check if the window was closed
  while (!quit);
}

void GameLoopSystem::shutdown() {

}
