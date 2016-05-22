#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/core/WindowSystem.hpp>

#include <engine/utils/Stack.hpp>
#include <engine/graphics/DrawCall.hpp>
#include <engine/graphics/Camera.hpp>
#include <engine/scene/Transform.hpp>

#include <ACGL/OpenGL/Objects.hh>

#include <engine/graphics/Light.hpp>
#include <engine/graphics/PostFX.hpp>
#include <engine/graphics/RenderQueue.hpp>

struct LightData {
  glm::vec4 color;
  bool castShadow;
  glm::vec3 dir;
  LightType type;

  TransformData lastSimulateTransform;
  TransformData thisSimulateTransform;
  glm::mat4 projMatrix;

  ACGL::OpenGL::SharedFrameBufferObject shadowFbo;
  ACGL::OpenGL::ConstSharedTextureBase shadowMap;
};

struct RenderPass {
  Stack<DrawCall> submittedDrawCallsOpaque;
  Stack<DrawCall> submittedDrawCallsTransparent;
  Stack<LightData> submittedLights;
  ACGL::OpenGL::SharedFrameBufferObject compositingTarget;
  ACGL::OpenGL::SharedFrameBufferObject txaaHistory;
  Entity camera;
  bool active;
  bool renderToTextureOnly;
  bool hasSSAO;
};

class RendererSystem : public System {
private:
  const float SHADOW_MAP_SCALE_FACTOR[3] = {0.25f, 0.5f, 1.0f};
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

  const int SHADOW_MAP_COUNT = 2;
  std::vector<ACGL::OpenGL::SharedFrameBufferObject> m_shadowMaps;
  ACGL::OpenGL::SharedTexture2D m_dummyShadowMap;

  std::vector<ScreenSpaceTexture> m_screenSpaceTextures;

  SharedTexture2D m_colorBuffer;
  SharedTexture2D m_emissiveBuffer;
  SharedTexture2D m_normalMotionBuffer;
  SharedTexture2D m_depthBuffer;
  SharedTexture2D m_specularBuffer;

  SharedTexture2D m_ssaoNoise;
  SharedTexture2D m_ssaoKernel;

  ACGL::OpenGL::SharedFrameBufferObject m_ssaoTarget;

  ACGL::OpenGL::SharedFrameBufferObject m_gBufferObject;

  ACGL::OpenGL::SharedFrameBufferObject m_primaryCompositingBuffer;
  ACGL::OpenGL::SharedFrameBufferObject m_secondaryCompositingBuffer;
  ACGL::OpenGL::SharedFrameBufferObject m_postfxTargetBuffer;


  SharedShaderProgram m_deferredCombineProgram;
  SharedShaderProgram m_blitProgram;
  SharedShaderProgram m_passBlitProgram;
  SharedShaderProgram m_shadowMapProg;

  SharedShaderProgram m_ssaoComputeProg;
  SharedShaderProgram m_ssaoBlurProg;

  SharedShaderProgram m_txaaProg;

  glm::mat4 aaProj;

  uint64_t m_frameIndex = 0;

  void render(RenderPass &pass, double interp, double totalTime);

public:
  double m_lastGBufferRenderTime;
  double m_lastGBufferRenderSetupTime;
  double m_lastGBufferRenderSubmitTime;

  double m_lastShadowMapRenderTime;
  double m_lastGBufferResolveTime;
  double m_lastPostprocessingTime;

  ACGL::OpenGL::SharedVertexArrayObject m_transformFeedbackVAO;
  ACGL::OpenGL::SharedArrayBuffer m_transformFeedbackBuffer;

  ACGL::OpenGL::SharedVertexArrayObject m_transformFeedbackVAO2;
  ACGL::OpenGL::SharedArrayBuffer m_transformFeedbackBuffer2;

  CONSTRUCT_SYSTEM(RendererSystem) {}

  bool startup() override;
  void shutdown() override;

  void addRenderPass(Entity cam, StringHash name,
                     ScreenSpaceSize size = ScreenSpaceSize::FULL) {
    cam.component<Camera>()->renderPassIndex = m_passes.size();
    auto target = SharedFrameBufferObject(new FrameBufferObject());
    target->attachColorTexture("oColor",
                               createScreenspaceTexture(size, GL_RGBA32F));
    target->validate();

    auto txaa = SharedFrameBufferObject(new FrameBufferObject());
    txaa->attachColorTexture("oColor",
                             createScreenspaceTexture(size, GL_RGBA32F));
    txaa->validate();

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

  ACGL::OpenGL::ConstSharedTextureBase getRenderPassTarget(StringHash pass) {
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
    return -1;
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

    if (light.castShadow && m_totalLightCount < m_shadowMaps.size()) {
      light.shadowFbo = m_shadowMaps[m_totalLightCount];
      light.shadowMap = light.shadowFbo->getDepthAttachment().texture;
      m_totalLightCount++;
    }

    m_passes[passIndex].submittedLights.push(light);
  }

  inline SharedLocationMappings getGBufferLocations() {
    return m_gBufferObject->getAttachmentLocations();
  }

  inline SharedLocationMappings getTransparentLocations() {
    return m_primaryCompositingBuffer->getAttachmentLocations();
  }

  template <typename T, typename... Args>
  inline void addEffect(Args &&... args) {
    m_effects.push_back(std::make_shared<T>(this, m_events, m_quality,
                                            std::forward<Args>(args)...));

    if (m_IsInitialized) {
      m_effects[m_effects.size() - 1]->startup();
    }
  }

  ACGL::OpenGL::SharedTexture2D
  createScreenspaceTexture(ScreenSpaceSize size, GLenum internalFormat) {
    auto windowSize = m_window->getSize();
    auto tex =
        SharedTexture2D(new Texture2D({windowSize.x * 1.0f / (1 << (int)size),
                                       windowSize.y * 1.0f / (1 << (int)size)},
                                      internalFormat));

    tex->setMinFilter(GL_LINEAR);
    tex->setMagFilter(GL_LINEAR);
    tex->setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_screenSpaceTextures.push_back({size, tex}); // RGBA per default
    return tex;
  }

  glm::mat4 getProjectionMatrix();

  GLfloat getDepthAtPixel(int x, int y);

  void frame(double interp, double totalTime);
};

glm::mat4 interpolate(TransformData a, TransformData b, double t,
                      glm::dvec3 camPos);