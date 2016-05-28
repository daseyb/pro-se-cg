#include <engine/core/WindowSystem.hpp>
#include <glow/glow.hh>
#include <glow/gl.hh>
#include <iostream>
#include <glow/common/log.hh>
#include <SDL_opengl.h>

using namespace std;

void WindowSystem::setSDLHintsForOpenGLVersion(unsigned int _version) {
#ifdef __APPLE__
#if (ACGL_OPENGL_VERSION >= 30)
  // request OpenGL 3.2, will return a 4.1 context on Mavericks
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
#else
  // non-apple
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _version / 10);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _version % 10);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef ACGL_OPENGL_PROFILE_CORE
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
#endif
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam) {
  _CRT_UNUSED(id);
  _CRT_UNUSED(length);
  _CRT_UNUSED(severity);
  _CRT_UNUSED(userParam);
  if (type == GL_DEBUG_TYPE_OTHER) return;

  cout << "Note: ";
  if (source == GL_DEBUG_SOURCE_API_ARB)
    cout << "OpenGL";
  else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
    cout << "your OS";
  else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
    cout << "the Shader Compiler";
  else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
    cout << "a third party component";
  else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
    cout << "your application";
  else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
    cout << "someone";

  cout << " reported a problem - it's a";
  if (type == GL_DEBUG_TYPE_ERROR_ARB)
    cout << "n error";
  else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
    cout << " deprecated behavior";
  else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
    cout << "n undefined behavior";
  else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
    cout << " portability issue";
  else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
    cout << " performance issue";
  else if (type == GL_DEBUG_TYPE_OTHER_ARB)
    cout << " something";

  cout << "\n";
  cout << "The message was: " << message << "\n" << "\n";
}

/**********************************************************************************************************************
* Returns true if a window with the desired context could get created.
* Requested OpenGL version gets set by ACGL defines.
*/
bool WindowSystem::createWindow() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    glow::error() << "SDL_Init Error: " << SDL_GetError() << "\n";
    return false;
  }

  setSDLHintsForOpenGLVersion(GLOW_OPENGL_VERSION);

  // request an OpenGL debug context:
  m_sdlWindow = SDL_CreateWindow(
      "New Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      m_windowSize.x, m_windowSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (m_fullscreen & SDL_WINDOW_FULLSCREEN));

  if (!m_sdlWindow) {
      glow::error() << "Failed to open a SDL window - requested OpenGL: "
            << GLOW_OPENGL_VERSION << "\n";
    return false;
  }

#ifdef _DEBUG
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#endif

  m_glContext = SDL_GL_CreateContext(m_sdlWindow);

  if (m_glContext == NULL) {
    printf("OpenGL context could not be created! SDL Error: %s\n",
           SDL_GetError());
    std::getchar();
    return false;
  }

  if (!glow::initGLOW()) {
      glow::error() << "Failed to init GLOW!" << "\n";
    return false;
  }

  // Use Vsync
  if (SDL_GL_SetSwapInterval(m_settings->vsyncEnabled() ? 1 : 0) < 0) {
    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
  }

  return true;
}

bool WindowSystem::startup() {
  RESOLVE_DEPENDENCY(m_settings);
  m_fullscreen = m_settings->getFullscreen();
  m_windowSize = m_settings->getResolution();

  /////////////////////////////////////////////////////////////////////////////////////
  // Create OpenGL capable window:
  if (!createWindow()) {
    return false;
  }

  /////////////////////////////////////////////////////////////////////////////////////
  // Set window title
  SDL_SetWindowTitle(m_sdlWindow, "Edge of Space");
  return true;
}

void WindowSystem::setWindowTitle(const char *title) {
  SDL_SetWindowTitle(m_sdlWindow, title);
}

void WindowSystem::swap() {
  // MacOS X will not swap correctly is another FBO is bound:
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  SDL_GL_SwapWindow(m_sdlWindow);
}

void WindowSystem::shutdown() {
  //This fucks things up in release mode, which is really weird...
  /*if (m_glContext) {
    SDL_GL_DeleteContext(m_glContext);
  }

  SDL_Quit();*/
}