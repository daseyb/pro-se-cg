#include <engine/graphics/RendererSystem.hpp>
#include <glow/gl.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/VertexArray.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/UniformBuffer.hh>
#include <glow/objects/ShaderStorageBuffer.hh>

#include <engine/graphics/DrawCall.hpp>
#include <engine/events/DrawEvent.hpp>
#include <engine/events/ResizeWindowEvent.hpp>
#include <glm/ext.hpp>


#include <engine/ui/imgui.h>
#include <engine/utils/Remotery.h>

#include <glow/std140.hh>

#undef near
#undef far

using namespace glow;

glm::mat4 interpolate(TransformData a, TransformData b, double t) {
  glm::vec3 pos = a.pos + (b.pos - a.pos) * t;
  glm::quat rot = glm::slerp(a.rot, b.rot, static_cast<float>(t));
  glm::vec3 s = a.scale + (b.scale - a.scale) * t;

  return glm::translate(pos) * glm::mat4_cast(rot) * glm::scale(s);
}

float random(float start, float end) {
  return start + (float(rand()) / RAND_MAX) * (end - start);
}

struct Vertex {
	glm::vec3 pos;
	float u;
	glm::vec3 norm;
	float v;
};

struct Primitive {
	Vertex a;
	Vertex b;
	Vertex c;
};

struct CameraData {
    glm::vec3 pos;
    float fov;
    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;
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

  m_camDataBuffer = ShaderStorageBuffer::create();
  m_primitiveBuffer = ShaderStorageBuffer::create();
  auto boundSSBO = m_primitiveBuffer->bind();
  boundSSBO.reserve(sizeof(Primitive) * MAX_PRIMITIVE_COUNT, GL_DYNAMIC_DRAW);
	  
  // Set up framebuffer for deferred shading
  auto windowSize = m_window->getSize();
  glViewport(0, 0, windowSize.x, windowSize.y);

  auto currentGBufferSize = G_BUFFER_SIZE[(int)m_quality];

  m_normalMotionBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);
  m_depthBuffer = createScreenspaceTexture(currentGBufferSize, GL_DEPTH_COMPONENT24);

  m_gBufferObject =
      Framebuffer::create({{"oNormalMotion", m_normalMotionBuffer}}, m_depthBuffer);

  m_primaryCompositingBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_secondaryCompositingBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_postfxTargetBuffer = Framebuffer::create({ { "oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F) } });

  m_blitProgram = Program::createFromFile("Blit");
  m_passBlitProgram = Program::createFromFile("PassBlit");
  m_txaaProg = Program::createFromFile("TXAA");
  m_raycastComputeProgram = Program::createFromFile("compute/RaycastCompute.csh");
  m_motionVectorProgram = Program::createFromFile("MotionVectors");

  m_raycastComputeProgram->setShaderStorageBuffer("CameraBuffer", m_camDataBuffer);
  m_raycastComputeProgram->setShaderStorageBuffer("PrimitiveBuffer", m_primitiveBuffer);

  m_copyPrimitiveProgram = Program::createFromFile("compute/CopyPrimitive.csh");

  //m_copyPrimitiveProgram->setShaderStorageBuffer("PrimitiveBuffer", m_primitiveBuffer);

  m_events->subscribe<ResizeWindowEvent>([this](const ResizeWindowEvent &e) {
    glViewport(0, 0, (int)e.newSize.x, (int)e.newSize.y);
    for (auto tex : m_screenSpaceTextures) {
      glm::vec2 newSize = e.newSize * 1.0f / (1 >> (int)tex.size);
      tex.texture->bind().resize((int)newSize.x, (int)newSize.y);
    }
  });

  m_events->subscribe<"DrawUI"_sh>([this]() {
    ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Image((void*)m_normalMotionBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
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

  // Prepare camera coords
  auto camTransform = interpolate(trans->lastGlobalTransform, trans->thisGlobalTransform, interp);
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

  auto gBufferRes = m_gBufferObject->getDim();
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

  CameraData camData { camPos, glm::radians(cam->fov), glm::vec4(camForward, 0.0f), glm::vec4(camRight, 0.0f), glm::vec4(camUp, 0.0f) };
  auto camDataBinding = m_camDataBuffer->bind();
  camDataBinding.setData(camData, GL_DYNAMIC_DRAW);

  size_t totalPrimitiveCount = 0;

  {
      auto boundCopyProgram = m_copyPrimitiveProgram->use();

      for (size_t i = 0; i < pass.submittedDrawCallsOpaque.size(); i++) {
          auto drawCall = pass.submittedDrawCallsOpaque[i];

          // No geometry loaded for the draw call
          if (!drawCall.geometry.vao) {
              continue;
          }

          auto mapping = drawCall.geometry.vao->getAttributeMapping();

          auto posBuffer = drawCall.geometry.vao->getBufferForAttribute("aPosition");
          auto idxBuffer = drawCall.geometry.vao->getIdxBuffer();
          auto normBuffer = drawCall.geometry.vao->getBufferForAttribute("aNormal");

          //We need at least positions, indices and normals
          if (!posBuffer || !idxBuffer || !normBuffer) {
              continue;
          }

          {
              auto drawPrimCount = idxBuffer->getIndexCount() / 3;

              glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, posBuffer->getObjectName());
              glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, normBuffer->getObjectName());
              glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, idxBuffer->getObjectName());
              glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_primitiveBuffer->getObjectName());

              boundCopyProgram.setUniform("model2World", drawCall.thisRenderTransform);
              boundCopyProgram.setUniform("model2WorldInvTransp", glm::inverseTranspose(drawCall.thisRenderTransform));
              boundCopyProgram.setUniform("currentPrimitiveCount", (int)drawPrimCount);
              boundCopyProgram.setUniform("writeOffset", (int)totalPrimitiveCount);
              boundCopyProgram.compute(drawPrimCount / 8 + 1);

              totalPrimitiveCount += drawPrimCount;
          }
      }
  }

  {
      auto boundRaycastProgram = m_raycastComputeProgram->use();

      boundRaycastProgram.setUniform("pixelOffset", glm::vec2(currentOffset));
      boundRaycastProgram.setUniform("primitiveCount", (int)totalPrimitiveCount);
      boundRaycastProgram.setUniform("totalTime", (float)totalTime);
      boundRaycastProgram.setImage(0, m_secondaryCompositingBuffer->getColorAttachments()[0].texture, GL_WRITE_ONLY);

      auto compositingSize = m_primaryCompositingBuffer->getDim();
      boundRaycastProgram.compute(compositingSize.x / 8 + 1, compositingSize.y / 8 + 1);
  }

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  // TXAA
  

  {
      // Set up gbuffer
      auto gBufferBind = m_gBufferObject->bind();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      glViewport(0, 0, gBufferRes.x, gBufferRes.y);
      glEnable(GL_CULL_FACE);

      auto boundProgram = m_motionVectorProgram->use();

      for (size_t i = 0; i < pass.submittedDrawCallsOpaque.size(); i++) {
          auto drawCall = pass.submittedDrawCallsOpaque[i];

          boundProgram.setUniform("uFar", cam->far);
          boundProgram.setUniform("uTime", (float)totalTime);
          boundProgram.setUniform("uModelMatrix", drawCall.thisRenderTransform);
          boundProgram.setUniform("uInverseModelMatrix", glm::inverse(drawCall.thisRenderTransform));
          boundProgram.setUniform("uViewProjectionMatrix", viewProjectionMatrix);
          boundProgram.setUniform("uPrevModelMatrix", drawCall.lastRenderTransform);
          boundProgram.setUniform("uPrevViewProjectionMatrix", prevViewProjectionMatrix);
          drawCall.geometry.vao->bind().draw();
      }
  }

  glDisable(GL_BLEND);

  // attribute-less rendering:
  auto vao = VertexArray::create(GL_TRIANGLE_STRIP);
  auto boundVAO = vao->bind(); // 'empty' VAO -> no attributes are defined

  {
      auto boundFB = pass.compositingTarget->bind();
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

      boundVAO.drawRange(0, 4);
  }

  if (!pass.renderToTextureOnly) {
    auto compositingSize = m_primaryCompositingBuffer->getDim();
    glViewport(0, 0, compositingSize.x, compositingSize.y);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    auto boundFrameBuffer = m_primaryCompositingBuffer->bind();
    auto boundPassBlitProgram = m_passBlitProgram->use();
    boundPassBlitProgram.setTexture(
        "uSamplerColor",
        pass.compositingTarget->getColorAttachments()[0].texture);

    boundVAO.drawRange(0, 4);
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
  auto vao = VertexArray::create(GL_TRIANGLE_STRIP);
  auto boundVAO = vao->bind(); // 'empty' VAO -> no attributes are defined

  rmt_BeginOpenGLSample(PostFX);
  rmt_BeginCPUSample(PostFX, 0);
  for (auto& fx : m_effects) {
    fx->apply(std::dynamic_pointer_cast<Texture2D>(m_primaryCompositingBuffer->getColorAttachments()[0].texture), m_postfxTargetBuffer);
    
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
  glDisable(GL_DEPTH_TEST);

  auto boundBlitProgram = m_blitProgram->use();
  boundBlitProgram.setTexture(
      "uSamplerColor",
      m_primaryCompositingBuffer->getColorAttachments()[0].texture);

  boundVAO.drawRange(0, 4);

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

