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
#include <engine/audio/OscSystem.hpp>
#include <engine/audio/OscToMidiSystem.hpp>

#include <engine/scene/Drawable.hpp>
#include <engine/events/MouseEvent.hpp>
#include <engine/events/KeyboardEvent.hpp>
#include <engine/core/SimulateEvent.hpp>
#include <engine/audio/MidiNoteEvent.hpp>

#include <engine/graphics/Light.hpp>

#include <glow-extras/assimp/Importer.hh>
#include <glow/objects/VertexArray.hh>

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
  OscSystem osc(&context, 2346);
  OscToMidiSystem oscToMidi(&context, 1);
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

  Entity camera = sceneGraph.create();
  auto camTransform = camera.assign<Transform>();
  camera.assign<Camera>(75.0f, 0.01f, 100.0f);
  camTransform->position = glm::vec3(0, 0, 10);
  renderer.addRenderPass(camera, "Main"_sh);

  auto &importer = glow::assimp::Importer();
  importer.setCalculateTangents(false);
  importer.setGenerateSmoothNormal(false);
  importer.setGenerateUVCoords(false);

  Geometry teapotGeom = {importer.load("data/geometry/teapot.obj")};
  Geometry testSceneGeom = {importer.load("data/geometry/test_scene.obj")};
  Geometry teddyGeom = {importer.load("data/geometry/teddy.obj")};
  Geometry coornellBoxGeom = {
      importer.load("data/geometry/CornellBox-Original.obj")};
  Geometry icosphereGeom = {importer.load("data/geometry/icosphere.obj")};

  Material whiteMat = {
      {0.8f, 0.8f, 0.8f}, 0.0f, {0.0f, 0.0f, 0.0f}, 0.0f, { 0.0f, 0.0f, 0.0f }, 0.0 };
  Entity teapotCenter = sceneGraph.create();
  auto boxTrans = teapotCenter.assign<Transform>();
  teapotCenter.assign<Drawable>(testSceneGeom, whiteMat);

  Material emissiveMat = {
      { 1.0f, 1.0f, 1.0f }, 0.0f,{ 5.0f, 1.0f, 1.0f }, 0.0f,{ 0.0f, 0.0f, 0.0f }, 0.0 };
  Entity icosphereSide = sceneGraph.create();
  auto teapotSideTransform = icosphereSide.assign<Transform>();
  teapotSideTransform->position = {-10, 0, 0};
  icosphereSide.assign<Drawable>(icosphereGeom, emissiveMat);

  Entity icosphere2 = sceneGraph.create();
  auto icosphere2Transform = icosphere2.assign<Transform>();
  icosphere2Transform->position = { -11, 22, 0 };
  icosphere2.assign<Drawable>(icosphereGeom, emissiveMat);

  /*Entity icosphereLight = sceneGraph.create();
  auto icoSphereLightTransform = icosphereLight.assign<Transform>();
  icoSphereLightTransform->position = glm::vec3(0, 3, 0);
  icoSphereLightTransform->parent = teapotSideTransform;
  icosphereLight.assign<Light>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1);*/


  Entity light01 = sceneGraph.create();
  light01.assign<Transform>()->position = glm::vec3(-11, 7, 8);
  light01.assign<Light>(glm::vec4(0.2f, 1.0f, 0.6f, 1.0f), 0.8);

  Entity light02 = sceneGraph.create();
  light02.assign<Transform>()->position = glm::vec3(-11, 7, 1);
  light02.assign<Light>(glm::vec4(1.0, 0.8, 0.6, 1.0f), 0.8);

  Entity light03 = sceneGraph.create();
  light03.assign<Transform>()->position = glm::vec3(-11, 20, 8);
  light03.assign<Light>(glm::vec4(0.6, 0.7, 1.0, 1.0f), 0.2);

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
            camTransform->rotation, mouseMove.y * 0.001f, glm::vec3(1, 0, 0));
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

    if (e.originalEvent.key.keysym.scancode < SDL_NUM_SCANCODES &&
        e.originalEvent.key.keysym.scancode >= 0) {
      keyState[e.originalEvent.key.keysym.scancode] =
          e.originalEvent.key.type == SDL_KEYDOWN;
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

  float teapotScale = 1.0f;
  float boxScale = 1.0f;

  events.subscribe<SimulateEvent>([&](const SimulateEvent &e) {
    glm::vec3 moveDir{0, 0, 0};
    if (keyState[SDL_SCANCODE_W])
      moveDir -= glm::vec3(0, 0, 1);
    if (keyState[SDL_SCANCODE_S])
      moveDir += glm::vec3(0, 0, 1);
    if (keyState[SDL_SCANCODE_A])
      moveDir -= glm::vec3(1, 0, 0);
    if (keyState[SDL_SCANCODE_D])
      moveDir += glm::vec3(1, 0, 0);

    float speed = 10.0;
    if (keyState[SDL_SCANCODE_LSHIFT])
      speed = 100.0f;

    if (moveDir.x != 0 || moveDir.y != 0 || moveDir.z != 0) {
      moveDir = rotate(glm::normalize(moveDir), camTransform->rotation);
      camTransform->position += moveDir * e.dt * speed;
    }

    if (teapotScale > 1.0f) {
      teapotScale -= (teapotScale - 1.0f) * e.dt * 10.0f;
    }

    if (boxScale > 1.0f) {
        boxScale -= (boxScale - 1.0f) * e.dt * 10.0f;
    }

    teapotSideTransform->position =
        glm::vec3(sinf(e.totalTime), 0.0f, cosf(e.totalTime)) * 5.0f;
    teapotSideTransform->scale = glm::vec3(teapotScale + sinf(e.totalTime*10.0f)*midi.controlValue(0, 0)*0.5f);

    boxTrans->scale = glm::vec3(boxScale + midi.controlValue(1)*2.5f);

  }); 

  events.subscribe<MidiNoteEvent>([&](const MidiNoteEvent &e) {
    if (!e.on) {
      return;
    }

    switch (e.noteIndex) {
    case 60:
      teapotScale = 1.0f + 4.0 * e.velocity;
      break;
    case 61:
      boxScale = 1.0f + 0.5 * e.velocity;
      break;
    default:
      break;
    }
  });

  // Kickoff the gameloop
  // This is what actually runs the game
  gameLoop.run();

  // Shutdown the registered systems in reverse order
  context.shutdown();
  return 0;
}
