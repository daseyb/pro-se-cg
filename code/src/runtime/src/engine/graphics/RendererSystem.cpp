#include <engine/graphics/RendererSystem.hpp>
#include <glow/gl.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/VertexArray.hh>

#include <engine/graphics/DrawCall.hpp>
#include <engine/events/DrawEvent.hpp>
#include <engine/events/ResizeWindowEvent.hpp>
#include <glm/ext.hpp>


#include <engine/ui/imgui.h>
#include <engine/utils/Remotery.h>

#undef near
#undef far


using namespace glow;

glm::mat4 interpolate(TransformData a, TransformData b, double t,
                      glm::vec3 camPos) {
  a.pos = a.pos - camPos;
  b.pos = b.pos - camPos;
  glm::vec3 pos = a.pos + (b.pos - a.pos) * t;
  glm::quat rot = glm::slerp(a.rot, b.rot, static_cast<float>(t));
  glm::vec3 s = a.scale + (b.scale - a.scale) * t;

  return glm::translate(pos) * glm::mat4_cast(rot) * glm::scale(s);
}

float random(float start, float end) {
  return start + (float(rand()) / RAND_MAX) * (end - start);
}

struct Primitive {
    glm::vec3 pos;
    float r;
};

struct CameraData {
    glm::vec3 pos;

};


bool RendererSystem::startup() {
  RESOLVE_DEPENDENCY(m_settings);
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_window);

  m_quality = m_settings->getQualitySetting();

  m_events->subscribe<DrawEvent>(
      [this](const DrawEvent &e) { frame(e.interp, e.totalTime); });

  for (auto& fx : m_effects) {
    fx->startup();
  }


  // Set up framebuffer for deferred shading
  auto windowSize = m_window->getSize();
  glViewport(0, 0, windowSize.x, windowSize.y);

  auto currentGBufferSize = G_BUFFER_SIZE[(int)m_quality];

  m_colorBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);
  m_emissiveBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);
  m_normalMotionBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);
  m_specularBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);
  m_depthBuffer = createScreenspaceTexture(currentGBufferSize, GL_DEPTH_COMPONENT24);

  m_gBufferObject =
      Framebuffer::create({{"oColor", m_colorBuffer},
                           {"oEmissive", m_emissiveBuffer},
                           {"oNormalMotion", m_normalMotionBuffer},
                           {"oSpecularSmoothness", m_specularBuffer}},
                          m_depthBuffer);

  m_primaryCompositingBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_secondaryCompositingBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_postfxTargetBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_blitProgram = Program::createFromFile("Blit");
  m_passBlitProgram = Program::createFromFile("PassBlit");
  m_txaaProg = Program::createFromFile("TXAA");
  m_raycastComputeProgram = Program::createFromFile("compute/RaycastCompute.csh");

  m_events->subscribe<ResizeWindowEvent>([this](const ResizeWindowEvent &e) {
    glViewport(0, 0, (int)e.newSize.x, (int)e.newSize.y);
    for (auto tex : m_screenSpaceTextures) {
      glm::vec2 newSize = e.newSize * 1.0f / (1 >> (int)tex.size);
      tex.texture->bind().resize((int)newSize.x, (int)newSize.y);
    }
  });

  m_events->subscribe<"DrawUI"_sh>([this]() {
    ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Image((void*)m_colorBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_emissiveBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_normalMotionBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_specularBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::End();
    
    ImGui::Begin("Render Passes", 0, ImGuiWindowFlags_AlwaysAutoResize);
    for (size_t i = 0; i < m_passes.size(); i++) {
      auto& pass = m_passes[i];
      if (ImGui::Button(("Pass_" + std::to_string(i)).c_str())) {
        pass.active = !pass.active;
      }
    }
    ImGui::End();
  }, 1);

  return true;
}



void RendererSystem::render(RenderPass& pass, double interp, double totalTime) {
  auto camEntity = pass.camera;
  // Make sure we have a camera
  if (!camEntity.valid()) {
    return;
  }

  Camera::Handle cam;
  Transform::Handle trans;

  camEntity.unpack<Camera, Transform>(cam, trans);

  if (!cam.valid() || !trans.valid()) {
    return;
  }

  // Set up gbuffer
  auto gBufferBind = m_gBufferObject->bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  auto gBufferRes = m_gBufferObject->getDim();
  glViewport(0, 0, gBufferRes.x, gBufferRes.y);
  glEnable(GL_CULL_FACE);

  auto origPos = trans->lastGlobalTransform.pos + (trans->thisGlobalTransform.pos - trans->lastGlobalTransform.pos)*interp;

  // Prepare camera coords
  auto camTransform = interpolate(trans->lastGlobalTransform, trans->thisGlobalTransform, interp, trans->thisGlobalTransform.pos);
  auto windowSize = m_window->getSize();

  // Do Camera jitter for TXAA
  // Halton(2,3)
  static const glm::vec3 OFFSETS[8]{
    glm::vec3{ 1.0 / 2.0, 1.0 / 3.0, 0 },
    glm::vec3{ 1.0 / 4.0, 2.0 / 3.0, 0 },
    glm::vec3{ 3.0 / 4.0, 1.0 / 9.0, 0 },
    glm::vec3{ 1.0 / 8.0, 4.0 / 9.0, 0 },

    glm::vec3{ 5.0 / 8.0, 7.0 / 9.0, 0 },
    glm::vec3{ 3.0 / 8.0, 2.0 / 9.0, 0 },
    glm::vec3{ 7.0 / 8.0, 5.0 / 9.0, 0 },
    glm::vec3{ 1.0 / 16.0, 8.0 / 9.0, 0 },
  };

  auto currentOffset = OFFSETS[m_frameIndex % 8] * 2 - 1.0f;

  currentOffset.x /= gBufferRes.x;
  currentOffset.y /= gBufferRes.y;
  currentOffset.z = 0;

  auto aaProj = glm::perspectiveFov<float>(glm::radians(cam->fov), (float)windowSize.x, (float)windowSize.y, cam->near, cam->far);
  glm::mat4 viewMatrix = glm::inverse(camTransform);
  glm::mat4 viewMatrixInverse = camTransform;


  glm::mat4 viewProjectionMatrixNoOffset = aaProj * viewMatrix;
  glm::mat4 viewProjectionMatrix = glm::translate(currentOffset) * viewProjectionMatrixNoOffset;
  glm::mat4 prevViewProjectionMatrix = glm::translate(currentOffset) * aaProj * static_cast<glm::mat4>(glm::inverse(trans->lastRenderTransform));


  auto camPos = glm::vec3(camTransform * glm::vec4{ 0, 0, 0, 1 });
  auto camForward = glm::normalize(glm::vec3(camTransform * glm::vec4{ 0, 0, -1, 0 }));
  auto camRight = glm::normalize(glm::vec3(camTransform * glm::vec4{ 1, 0, 0, 0 }));
  auto camUp = glm::normalize(glm::vec3(camTransform * glm::vec4{ 0, 1, 0, 0 }));

  auto boundRaycastProgram = m_raycastComputeProgram->use();

  for (size_t i = 0; i < pass.submittedDrawCallsOpaque.size(); i++) {
      
  }

  // TXAA
  glDisable(GL_BLEND);

  // attribute-less rendering:
  VertexArray vao;
  auto boundVAO = vao.bind(); // 'empty' VAO -> no attributes are defined

  pass.compositingTarget->bind();
  int width = pass.compositingTarget->getDim().x;
  int height = pass.compositingTarget->getDim().y;
  glViewport(0, 0, width, height);

  auto boundTxaaProg = m_txaaProg->use();
  boundTxaaProg.setTexture(
      "uSamplerColor",
      m_secondaryCompositingBuffer->getColorAttachments()[0].texture);

  boundTxaaProg.setTexture("uSamplerHistory",
                           pass.txaaHistory->getColorAttachments()[0].texture);

  boundTxaaProg.setTexture("uSamplerNormalMotion", m_normalMotionBuffer);
  boundTxaaProg.setTexture("uSamplerDepth", m_depthBuffer);

  auto colorSize = glm::vec2(m_secondaryCompositingBuffer->getDim());
  boundTxaaProg.setUniform("uOneOverColorSize", glm::vec2(1.0) / colorSize);

  auto motionSize = glm::vec2(m_normalMotionBuffer->getDim());
  boundTxaaProg.setUniform("uOneOverMotionSize", glm::vec2(1.0) / motionSize);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes

  if (!pass.renderToTextureOnly) {
    auto compositingSize = m_normalMotionBuffer->getDim();
    glViewport(0, 0, compositingSize.x, compositingSize.y);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    m_primaryCompositingBuffer->bind();
    auto boundPassBlitProgram = m_passBlitProgram->use();
    boundPassBlitProgram.setTexture(
        "uSamplerColor",
        pass.compositingTarget->getColorAttachments()[0].texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes
  }

  // Swap TAA buffers
  auto temp = pass.compositingTarget;
  pass.compositingTarget = pass.txaaHistory;
  pass.txaaHistory = temp;

  trans->lastRenderTransform = static_cast<glm::dmat4>(camTransform);
}

void RendererSystem::frame(double interp, double totalTime) {
  rmt_BeginOpenGLSample(RenderFrame);
  rmt_BeginCPUSample(RenderFrame, 0);

  m_frameIndex++;

  rmt_BeginOpenGLSample(ClearBuffer);
  rmt_BeginCPUSample(ClearBuffer, 0);
  m_primaryCompositingBuffer->bind();
  glClear(GL_COLOR_BUFFER_BIT);
  rmt_EndCPUSample();
  rmt_EndOpenGLSample();

  rmt_BeginOpenGLSample(RenderPasses);
  rmt_BeginCPUSample(RenderPasses, 0);
  for (auto& pass : m_passes) {
    rmt_ScopedCPUSample(DrawPass, 0);
    if (pass.active) {
      render(pass, interp, totalTime);
    }
    pass.submittedDrawCallsOpaque.reset();
    pass.submittedDrawCallsTransparent.reset();
    pass.submittedLights.reset();
  }
  rmt_EndCPUSample();
  rmt_EndOpenGLSample();

  m_totalLightCount = 0;

  // attribute-less rendering:
  VertexArray vao;
  auto boundVAO = vao.bind(); // 'empty' VAO -> no attributes are defined

  rmt_BeginOpenGLSample(PostFX);
  rmt_BeginCPUSample(PostFX, 0);
  for (auto& fx : m_effects) {
    fx->apply(SharedTexture2D((Texture2D*)m_primaryCompositingBuffer->getColorAttachments()[0].texture.get()), m_postfxTargetBuffer);
    
    auto temp = m_primaryCompositingBuffer;
    m_primaryCompositingBuffer = m_postfxTargetBuffer;
    m_postfxTargetBuffer = temp;
  }
  rmt_EndCPUSample();
  rmt_EndOpenGLSample();

  rmt_BeginOpenGLSample(Blit);
  rmt_BeginCPUSample(Blit, 0);
  glViewport(0, 0, m_window->getSize().x, m_window->getSize().y);
  // Blit to backbuffer with tonemapping
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  auto boundBlitProgram = m_blitProgram->use();
  boundBlitProgram.setTexture(
      "uSamplerColor",
      m_primaryCompositingBuffer->getColorAttachments()[0].texture);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes
  rmt_EndCPUSample();
  rmt_EndOpenGLSample();

  rmt_EndCPUSample();
  rmt_EndOpenGLSample();
}

void RendererSystem::shutdown() {
  for (auto& fx : m_effects) {
    fx->shutdown();
  }
}

