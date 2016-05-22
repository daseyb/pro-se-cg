#pragma once
#include <ACGL/OpenGL/Objects.hh>
#include <engine/core/SettingsSystem.hpp>

enum class ScreenSpaceSize {
  FULL,
  HALF,
  QUARTER,
  EIGHTH
};

struct ScreenSpaceTexture {
  ScreenSpaceSize size;
  ACGL::OpenGL::SharedTexture2D texture;
};

class RendererSystem;
class EventSystem;

class PostFX {
protected:
  RendererSystem* m_renderer;
  EventSystem* m_events;

  QualitySetting m_quality;

public:
  bool enabled;

  PostFX(RendererSystem* renderer, EventSystem* events, QualitySetting quality) : m_renderer(renderer), m_events(events), m_quality(quality) {};
  virtual void startup() = 0;
  virtual void apply(ACGL::OpenGL::ConstSharedTextureBase inputBuffer, ACGL::OpenGL::SharedFrameBufferObject outputBuffer) = 0;
  virtual void shutdown() = 0;
};