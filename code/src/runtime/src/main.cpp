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
#include <engine/audio/MidiControlEvent.hpp>

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

  if (argc != 2) {
    std::cout << "Usage: <config file>" << std::endl;
    return -1;
  }

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

  // renderer.addEffect<BloomPostFX>();

  window.setWindowTitle("Orion");

  midi.setDefaultControlValues(0.5f);

  Entity camera = sceneGraph.create();
  auto camTransform = camera.assign<Transform>();
  auto cam = camera.assign<Camera>(75.0f, 0, 0, 0.01f, 100.0f);
  camTransform->position = glm::vec3(0, 0, 10);
  renderer.addRenderPass(camera, "Main"_sh);

  glow::assimp::Importer().setCalculateTangents(false);
  glow::assimp::Importer().setGenerateSmoothNormal(false);
  glow::assimp::Importer().setGenerateUVCoords(false);

  SharedTexture2D diffuseTex = Texture2D::createFromFile("data/textures/test_diffuse.png");
  SharedTexture2D vciLogoTex = Texture2D::createFromFile("data/textures/vci_logo.png");
  SharedTexture2D emissiveTex = Texture2D::createFromFile("data/textures/test_emissive.png");
  SharedTexture2D normalTex = Texture2D::createFromFile("data/textures/test_normalmap.png");

  Geometry teapotGeom = {glow::assimp::Importer().load("data/geometry/teapot.obj")};
  Geometry testSceneGeom = {glow::assimp::Importer().load("data/geometry/test_scene.obj")};
  Geometry teddyGeom = {glow::assimp::Importer().load("data/geometry/teddy.obj")};
  Geometry coornellBoxGeom = {
      glow::assimp::Importer().load("data/geometry/CornellBox-Original.obj")};
  Geometry icosphereGeom = {glow::assimp::Importer().load("data/geometry/icosphere.obj")};
  Geometry sphereGeom = { glow::assimp::Importer().load("data/geometry/sphere.obj") };

  Material whiteMat = {
      {0.8f, 0.8f, 0.8f},
      1.0f,
      {20.0f, 20.0f, 20.0f},
      0.0f,
      {0.0f, 0.0f, 0.0f},
      1.0,
      nullptr,
      nullptr,
      vciLogoTex,
      nullptr,
  };
  Entity teapotCenter = sceneGraph.create();
  auto boxTrans = teapotCenter.assign<Transform>();
  teapotCenter.assign<Drawable>(testSceneGeom, whiteMat);

  Material emissiveMat = {
      { 1.0f, 1.0f, 1.0f }, 0.0f,{ 15.0f, 1.0f, 1.0f }, 0.0f,{ 0.0f, 0.0f, 0.0f }, 1.0 };

  Material refractiveMat = {
      { 0.0f, 0.0f, 0.0f },
      1.0f,
      { 0.0f, 0.0f, 0.0f },
      1.0f,
      { 1.0f, 1.0f, 1.0f },
      1.5f,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
  };
  
  Entity icosphereSide = sceneGraph.create();
  auto teapotSideTransform = icosphereSide.assign<Transform>();
  teapotSideTransform->position = {-10, 0, 0};
  icosphereSide.assign<Drawable>(sphereGeom, refractiveMat);

  Entity icosphere2 = sceneGraph.create();
  auto icosphere2Transform = icosphere2.assign<Transform>();
  icosphere2Transform->position = { -11, 22, 0 };
  icosphere2.assign<Drawable>(icosphereGeom, emissiveMat);

  Entity light01Ent = sceneGraph.create();
  light01Ent.assign<Transform>()->position = glm::vec3(-11, 7, 8);
  auto light01 = light01Ent.assign<Light>(glm::vec4(0.2f, 1.0f, 0.6f, 40), 0.8);

  Entity light02Ent = sceneGraph.create();
  light02Ent.assign<Transform>()->position = glm::vec3(-11, 7, 1);
  auto light02 = light02Ent.assign<Light>(glm::vec4(1.0, 0.8, 0.6, 40), 0.8);

  Entity light03Ent = sceneGraph.create();
  light03Ent.assign<Transform>()->position = glm::vec3(-11, 22, 8);
  auto light03 = light03Ent.assign<Light>(glm::vec4(0.6, 0.7, 1.0, 40), 0.2);

  std::vector<Transform::Handle> barTransforms;
  std::vector<Entity> barEntities;

  Material cubeMat = {
      { 0.8f, 0.8f, 0.8f },
      1.0f,
      { 0.0f, 0.0f, 0.0f },
      0.0f,
      { 0.0f, 0.0f, 0.0f },
      1.0,
      nullptr,
      nullptr,
      nullptr,
      normalTex,
  };

  Geometry cubeGeom = { glow::assimp::Importer().load("data/geometry/cube.obj") };

  auto cubeEntity = sceneGraph.create();
  auto cubeTransform = cubeEntity.assign<Transform>();
  cubeTransform->position = glm::vec3(-5, -1, -5);
  cubeEntity.assign<Drawable>(cubeGeom, cubeMat);


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
  float teapotPos = 0.0f;

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

    teapotPos += e.dt * midi.controlValue(2);
    teapotSideTransform->position =
        glm::vec3(sinf(teapotPos), 0.0f, cosf(teapotPos)) * 5.0f;
    //teapotSideTransform->scale = glm::vec3(teapotScale + sinf(e.totalTime*10.0f)*midi.controlValue(0)*0.5f);

    light01->color.a = midi.controlValue(4) * 150;
    light02->color.a = midi.controlValue(5) * 150;
    light03->color.a = midi.controlValue(6) * 150;

    float* spectrumData;
    unsigned int length;

    audio.getSpectrum(&spectrumData, &length);

    float bucketsPerCube = float(length) / barTransforms.size();

    if (length == 0) {
        return;
    }

    int currDataPos = 0;
    for (size_t i = 0; i < barTransforms.size(); i++) {
        float percentage = sqrt(1.0f- float(i+1) / (barTransforms.size()+1));
        int bucketCount = (int)(-log(percentage) * bucketsPerCube);
        float subSum = 0; 
        for (size_t j = currDataPos; j < currDataPos + bucketCount; j++) {
            assert(j < length);
            subSum += spectrumData[j];
        }
        subSum /= bucketCount;
        currDataPos += bucketCount;

        barTransforms[i]->scale.y = subSum * 1000 * 1.0f/log(1.0f - percentage) + 0.1f;
    }

  }); 

  events.subscribe<"DrawUI"_sh>([&]() {
    ImGui::Begin("Scene Controls");
    midi.uiFader("Light 01 Color", 4, 0);
    midi.uiFader("Light 02 Color", 5, 0);
    midi.uiFader("Light 03 Color", 6, 0);
    ImGui::Separator();
    midi.uiFader("Camera Focal Distance", 2, 0);
    midi.uiFader("Camera Lens Radius", 3, 0);
    ImGui::End();
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
