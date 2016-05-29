#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/core/WindowSystem.hpp>

#include <engine/utils/Stack.hpp>
#include <engine/graphics/DrawCall.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/scene/Transform.hpp>

#include <glow/fwd.hh>
#include <glow/gl.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/Framebuffer.hh>

#include <engine/graphics/Light.hpp>
#include <engine/graphics/PostFX.hpp>
#include <engine/graphics/RenderQueue.hpp>

#undef OPAQUE
#undef TRANSPARENT
using namespace glow;

struct LightData {
  glm::vec4 color;
  bool castShadow;
  glm::vec3 dir;
  LightType type;

  TransformData lastSimulateTransform;
  TransformData thisSimulateTransform;
  glm::mat4 projMatrix;
};

struct RenderPass {
  Stack<DrawCall> submittedDrawCallsOpaque;
  Stack<DrawCall> submittedDrawCallsTransparent;
  Stack<LightData> submittedLights;
  glow::SharedFramebuffer compositingTarget;
  glow::SharedFramebuffer txaaHistory;
  Entity camera;
  bool active;
  bool renderToTextureOnly;
  bool hasSSAO;
};

class RendererSystem : public System {
private:
  const ScreenSpaceSize G_BUFFER_SIZE[3] = {
      ScreenSpaceSize::HALF, ScreenSpaceSize::FULL, ScreenSpaceSize::FULL};

  SettingsSystem *m_settings;
  EventSystem *m_events;
  WindowSystem *m_window;

  QualitySetting m_quality;

  std::vector<RenderPass> m_passes;

  std::unordered_map<StringHash, uint32_t> m_passIds;

  uint32_t m_totalLightCount;

  std::vector<std::shared_ptr<PostFX>> m_effects;

  std::vector<ScreenSpaceTexture> m_screenSpaceTextures;

  SharedTexture2D m_colorBuffer;
  SharedTexture2D m_emissiveBuffer;
  SharedTexture2D m_normalMotionBuffer;
  SharedTexture2D m_depthBuffer;
  SharedTexture2D m_specularBuffer;

  glow::SharedFramebuffer m_gBufferObject;

  glow::SharedFramebuffer m_primaryCompositingBuffer;
  glow::SharedFramebuffer m_secondaryCompositingBuffer;
  glow::SharedFramebuffer m_postfxTargetBuffer;

  SharedProgram m_blitProgram;
  SharedProgram m_passBlitProgram;

  SharedProgram m_raycastComputeProgram;

  SharedProgram m_txaaProg;

  SharedShaderStorageBuffer m_camDataBuffer;
  SharedShaderStorageBuffer m_primitiveBuffer;

  uint64_t m_frameIndex = 0;

  void render(RenderPass &pass, double interp, double totalTime);

public:
  CONSTRUCT_SYSTEM(RendererSystem) {}

  bool startup() override;
  void shutdown() override;

  void addRenderPass(Entity cam, StringHash name,
                     ScreenSpaceSize size = ScreenSpaceSize::FULL) {
    cam.component<Camera>()->renderPassIndex = m_passes.size();
    auto target = Framebuffer::create({ { "oColor", createScreenspaceTexture(size, GL_RGBA32F) } });

    auto txaa = Framebuffer::create({ { "oColor", createScreenspaceTexture(size, GL_RGBA32F) } });

    m_passIds[name] = m_passes.size();
    m_passes.push_back({kilobytes(4), kilobytes(4), kilobytes(4), target, txaa,
                        cam, true, false, false});
  }

  void setRenderPassSSAO(StringHash pass, bool active) {
    auto id = getRenderPassId(pass);
    if (id == -1) {
      return;
    }

    m_passes[id].hasSSAO = active;
  }

  void setRenderPassActive(StringHash pass, bool active) {
    auto id = getRenderPassId(pass);
    if (id == -1) {
      return;
    }

    m_passes[id].active = active;
  }

  void setRenderPassOnlyTexture(StringHash pass, bool active) {
    auto id = getRenderPassId(pass);
    if (id == -1) {
      return;
    }

    m_passes[id].renderToTextureOnly = active;
  }

  glow::SharedTexture getRenderPassTarget(StringHash pass) {
    auto id = getRenderPassId(pass);
    if (id == -1) {
      return nullptr;
    }

    return m_passes[id].txaaHistory->getColorAttachments()[0].texture;
  }

  uint32_t getRenderPassId(StringHash sh) {
    if (m_passIds.find(sh) != m_passIds.end()) {
      return m_passIds[sh];
    }
    return (uint32_t)-1;
  }

  inline size_t getNumPasses() { return m_passes.size(); }

  inline void submit(DrawCall drawCall, RenderQueue queue,
                     uint32_t passIndex = 0) {
    if (passIndex >= m_passes.size())
      return;

    if (queue == RenderQueue::OPAQUE) {
      m_passes[passIndex].submittedDrawCallsOpaque.push(drawCall);
    } else if (queue == RenderQueue::TRANSPARENT) {
      m_passes[passIndex].submittedDrawCallsTransparent.push(drawCall);
    }
  }

  inline void submit(LightData light, uint32_t passIndex = 0) {
    if (passIndex >= m_passes.size())
      return;

    m_passes[passIndex].submittedLights.push(light);
  }

  inline SharedLocationMapping getGBufferLocations() {
    return m_gBufferObject->getFragmentMapping();
  }

  inline SharedLocationMapping getTransparentLocations() {
    return m_primaryCompositingBuffer->getFragmentMapping();
  }

  template <typename T, typename... Args>
  inline void addEffect(Args &&... args) {
    m_effects.push_back(std::make_shared<T>(this, m_events, m_quality,
                                            std::forward<Args>(args)...));

    if (m_IsInitialized) {
      m_effects[m_effects.size() - 1]->startup();
    }
  }

  glow::SharedTexture2D
  createScreenspaceTexture(ScreenSpaceSize size, GLenum internalFormat) {
    auto windowSize = m_window->getSize();
    auto tex = Texture2D::create(windowSize.x * 1.0f / (1 << (int)size), windowSize.y * 1.0f / (1 << (int)size), internalFormat);
    
    auto boundTex = tex->bind();
    boundTex.setMinFilter(GL_LINEAR);
    boundTex.setMagFilter(GL_LINEAR);
    boundTex.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    boundTex.makeStorageImmutable(tex->getWidth(), tex->getHeight(), internalFormat);

    m_screenSpaceTextures.push_back({size, tex}); // RGBA per default
    return tex;
  }

  void frame(double interp, double totalTime);
};

glm::mat4 interpolate(TransformData a, TransformData b, double t);