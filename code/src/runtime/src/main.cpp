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

int main(int argc, char *argv[]) {
#if _DEBUG
  if (argc != 2) {
    std::cout << "Usage: <config file>" << std::endl;
    return -1;
  }
#endif
  std::string configFile = argv[1];

  // Set up the systems we want to use.
  // They depends on each other so we need to add them to the context in the right order
  // This also makes sure that dependencies are initialized before things that depend on them
  // If you try to initialize a system before it's dependencies you'll get an error message in the console
  Context context;
  SettingsSystem settings(&context, "data/", "textures/", "geometry/", "shader/", "sound/", configFile);
  WindowSystem window(&context, 1280, 720, false);
  ProfilerSystem profiler(&context);
  EventSystem events(&context);
  RendererSystem renderer(&context);
  SceneGraphSystem sceneGraph(&context);
  AudioSystem audio(&context);
  MidiSystem midi(&context);
  UISystem ui(&context);
  GameLoopSystem gameLoop(&context, 60, 100);


  // Call the startup functions of the systems above in the order they are listed.
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
  camera.assign<Camera>(90.0f, 0.01f, 100.0f);
  renderer.addRenderPass(camera, "Main"_sh);

  Geometry sphereGeom = { 2.0f };
  Material sphereMat = { {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f, 0.0f }, RenderQueue::OPAQUE };
  Entity sphere = sceneGraph.create();
  sphere.assign<Transform>();
  sphere.assign<Drawable>( sphereGeom, sphereMat );

  // Kickoff the gameloop
  // This is what actually runs the game
  gameLoop.run();


  // Shutdown the registered systems in reverse order
  context.shutdown();
  return 0;
}

