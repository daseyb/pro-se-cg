#include <iostream>
#include <memory>

#include <engine/events/EventSystem.hpp>
#include <engine/core/SettingsSystem.hpp>
#include <engine/core/ProfilerSystem.hpp>
#include <engine/core/WindowSystem.hpp>
#include <engine/audio/AudioSystem.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/core/GameLoopSystem.hpp>
#include <engine/ui/UISystem.hpp>
#include <engine/scene/SceneGraphSystem.hpp>
#include <engine/audio/MidiSystem.hpp>

#include <engine/scene/Drawable.hpp>
#include <engine/events/MouseEvent.hpp>
#include <engine/events/KeyboardEvent.hpp>
#include <engine/core/SimulateEvent.hpp>

#include <engine/graphics/BloomPostFX.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

glm::vec3 rotate(glm::vec3 vec, glm::mat4 mat) {
  auto temp = mat * glm::vec4(vec.x, vec.y, vec.z, 0);
  return glm::vec3(temp.x, temp.y, temp.z);
}

int main(int argc, char *argv[]) {
#if _DEBUG
  if (argc != 2) {
    std::cout << "Usage: <config file>" << std::endl;
    return -1;
  }
#endif
  std::string configFile = argv[1];

  // Set up the systems we want to use.
  // They depends on each other so we need to add them to the context in the
  // right order
  // This also makes sure that dependencies are initialized before things that
  // depend on them
  // If you try to initialize a system before it's dependencies you'll get an
  // error message in the console
  Context context;
  SettingsSystem settings(&context, "data/", "textures/", "geometry/",
                          "shader/", "sound/", configFile);
  WindowSystem window(&context, 1280, 720, false);
  ProfilerSystem profiler(&context);
  EventSystem events(&context);
  RendererSystem renderer(&context);
  SceneGraphSystem sceneGraph(&context);
  AudioSystem audio(&context);
  MidiSystem midi(&context);
  UISystem ui(&context);
  GameLoopSystem gameLoop(&context, 60, 100);

  // Call the startup functions of the systems above in the order they are
  // listed.
  // This means it's safe to access other systems in a startup function as long
  // as you depend on them
  if (!context.startup()) {
    glow::error() << "Some systems failed to start up. Exiting!"
                  << "\n";
    return -1;
  }

  window.setWindowTitle("Orion");

  renderer.addEffect<BloomPostFX>();

  Entity camera = sceneGraph.create();
  auto camTransform = camera.assign<Transform>();
  camera.assign<Camera>(75.0f, 0.01f, 100.0f);
  camTransform->position = glm::vec3(0, 0, 30);
  renderer.addRenderPass(camera, "Main"_sh);

  Geometry sphereGeom = {2.0f};
  Material sphereMat = {
      {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, RenderQueue::OPAQUE};
  Entity sphere = sceneGraph.create();
  sphere.assign<Transform>();
  sphere.assign<Drawable>(sphereGeom, sphereMat);

  Geometry sphereGeom2 = { 5.0f };
  Entity sphere2 = sceneGraph.create();
  sphere2.assign<Transform>()->position = { 10, 0, 0 };
  sphere2.assign<Drawable>(sphereGeom2, sphereMat);

  bool keyState[SDL_NUM_SCANCODES] = {};

  events.subscribe<MouseEvent>([&](const MouseEvent &e) {
    switch (e.originalEvent.type) {
    // Capture the mouse when we click on the window
    case SDL_MOUSEBUTTONDOWN:
      // As long as we are not over any UI (because we might want to click on
      // that)
      if (!ImGui::GetIO().WantCaptureMouse) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
      }
      break;
    case SDL_MOUSEMOTION: {
      // If the mouse is captured
      if (SDL_GetRelativeMouseMode()) {
        // Rotate the camera based on the mouse movement
        auto mouseMove = glm::vec2(-(float)e.originalEvent.motion.xrel,
                                   -(float)e.originalEvent.motion.yrel);

        auto up =
            rotate(glm::vec3(0, 1, 0), glm::inverse(camTransform->rotation));
        camTransform->rotation =
            glm::rotate(camTransform->rotation, mouseMove.x * 0.001f, up);
        camTransform->rotation = glm::rotate(
            camTransform->rotation, mouseMove.y * 0.001f, glm::vec3(-1, 0, 0));
      }
      break;
    }
    default:
      break;
    }
  });

  events.subscribe<KeyboardEvent>([&](const KeyboardEvent &e) {
    if (e.originalEvent.key.repeat != 0) {
        return;
    }

    if (e.originalEvent.key.keysym.scancode < SDL_NUM_SCANCODES && e.originalEvent.key.keysym.scancode >= 0) {
        keyState[e.originalEvent.key.keysym.scancode] = e.originalEvent.key.type == SDL_KEYDOWN;
    }

    switch (e.originalEvent.key.keysym.scancode) {
    case SDL_SCANCODE_ESCAPE:
      // Make the mouse freely movable on escape
      if (e.originalEvent.key.type == SDL_KEYDOWN) {
        if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
          SDL_SetRelativeMouseMode(SDL_FALSE);
        } else
          SDL_SetRelativeMouseMode(SDL_TRUE);
      }
      break;
    default:
      break;
    }
  });

  events.subscribe<SimulateEvent>([&](const SimulateEvent& e) {
      glm::vec3 moveDir{ 0, 0, 0 };
      if (keyState[SDL_SCANCODE_W]) moveDir -= glm::vec3(0, 0, 1);
      if (keyState[SDL_SCANCODE_S]) moveDir += glm::vec3(0, 0, 1);
      if (keyState[SDL_SCANCODE_A]) moveDir -= glm::vec3(1, 0, 0);
      if (keyState[SDL_SCANCODE_D]) moveDir += glm::vec3(1, 0, 0);

      if (moveDir.x != 0 || moveDir.y != 0 || moveDir.z != 0) {
          moveDir = rotate(glm::normalize(moveDir), camTransform->rotation);
          camTransform->position += moveDir * e.dt * 10.0;
      }
  });

  // Kickoff the gameloop
  // This is what actually runs the game
  gameLoop.run();

  // Shutdown the registered systems in reverse order
  context.shutdown();
  return 0;
}
