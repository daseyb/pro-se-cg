#include <engine/graphics/RendererSystem.hpp>
#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

#include <engine/graphics/DrawCall.hpp>
#include <engine/events/DrawEvent.hpp>
#include <engine/events/ResizeWindowEvent.hpp>
#include <glm/ext.hpp>

#include <engine/ui/imgui.h>

using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

const glm::vec2 SHADOW_MAP_RESOLUTION = { 2048, 2048 };

glm::mat4 interpolate(TransformData a, TransformData b, double t, glm::dvec3 camPos) {
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
  m_depthBuffer = createScreenspaceTexture(currentGBufferSize, GL_DEPTH24_STENCIL8);
  m_specularBuffer = createScreenspaceTexture(currentGBufferSize, GL_RGBA16F);

  m_gBufferObject = SharedFrameBufferObject(new FrameBufferObject());
  m_gBufferObject->attachColorTexture("oColor", m_colorBuffer);
  m_gBufferObject->attachColorTexture("oEmissive", m_emissiveBuffer);
  m_gBufferObject->attachColorTexture("oNormalMotion", m_normalMotionBuffer);
  m_gBufferObject->attachColorTexture("oSpecularSmoothness", m_specularBuffer);
  m_gBufferObject->setDepthTexture(m_depthBuffer);
  m_gBufferObject->validate(); // always a good idea

  m_primaryCompositingBuffer = SharedFrameBufferObject(new FrameBufferObject());
  m_primaryCompositingBuffer->attachColorTexture("oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F));
  m_primaryCompositingBuffer->validate();

  m_secondaryCompositingBuffer = SharedFrameBufferObject(new FrameBufferObject());
  m_secondaryCompositingBuffer->attachColorTexture("oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F));
  m_secondaryCompositingBuffer->validate();

  m_ssaoTarget = SharedFrameBufferObject(new FrameBufferObject());
  m_ssaoTarget->attachColorTexture("oColor", createScreenspaceTexture(ScreenSpaceSize::HALF, GL_R8));
  m_ssaoTarget->validate();


  m_postfxTargetBuffer = SharedFrameBufferObject(new FrameBufferObject());
  m_postfxTargetBuffer->attachColorTexture("oColor", createScreenspaceTexture(currentGBufferSize, GL_RGBA32F));
  m_postfxTargetBuffer->validate();


  m_deferredCombineProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("DeferredCombine"));
  m_blitProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("Blit"));
  m_passBlitProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("PassBlit"));
  m_shadowMapProg = ShaderProgramFileManager::the()->get(ShaderProgramCreator("ShadowMap"));
  m_txaaProg = ShaderProgramFileManager::the()->get(ShaderProgramCreator("TXAA").fragmentDataLocations(m_secondaryCompositingBuffer->getAttachmentLocations()));

  m_ssaoComputeProg = ShaderProgramFileManager::the()->get(ShaderProgramCreator("SSAO/SSAOCompute").fragmentDataLocations(m_ssaoTarget->getAttachmentLocations()));
  m_ssaoBlurProg = ShaderProgramFileManager::the()->get(ShaderProgramCreator("SSAO/SSAOBlur"));


  m_events->subscribe<ResizeWindowEvent>([this](const ResizeWindowEvent &e) {
    glViewport(0, 0, e.newSize.x, e.newSize.y);
    for (auto tex : m_screenSpaceTextures) {
      tex.texture->resize(e.newSize * 1.0f/(1 >> (int)tex.size));
    }
  });

  m_dummyShadowMap = SharedTexture2D(new Texture2D({ 1, 1 }, GL_DEPTH24_STENCIL8));
  m_dummyShadowMap->setMinFilter(GL_LINEAR);
  m_dummyShadowMap->setMagFilter(GL_LINEAR);
  m_dummyShadowMap->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  m_dummyShadowMap->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
  m_dummyShadowMap->setCompareFunc(GL_LEQUAL);


  for (int i = 0; i < SHADOW_MAP_COUNT; i++) {
    auto shadowFbo = SharedFrameBufferObject(new FrameBufferObject);
    auto shadowMap = SharedTexture2D(new Texture2D(SHADOW_MAP_RESOLUTION * SHADOW_MAP_SCALE_FACTOR[(int)m_quality], GL_DEPTH24_STENCIL8));

    shadowMap->setMinFilter(GL_LINEAR);
    shadowMap->setMagFilter(GL_LINEAR);
    shadowMap->setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    shadowMap->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
    shadowMap->setCompareFunc(GL_LEQUAL);

    shadowFbo->setDepthTexture(shadowMap);
    shadowFbo->validate(); // always a good idea
    m_shadowMaps.push_back(shadowFbo);
  }

  const int kernelSize = 16;
  GLubyte* kernel = new GLubyte[kernelSize * 4];

  GLubyte* pixel = kernel;

  for (int i = 0; i < kernelSize; ++i) {
    glm::vec4 kern = glm::vec4(
      random(-1.0f, 1.0f),
      random(-1.0f, 1.0f),
      random(0.0f, 1.0f), 0);
    kern = glm::normalize(kern);
   
    float scale = float(i) / float(kernelSize);
    scale = glm::lerp(0.1f, 1.0f, scale * scale);
    kern *= scale;

    kern = (kern + glm::vec4(1.0, 1.0, 0, 0)) * glm::vec4(0.5, 0.5, 1.0, 1.0);

    pixel[0] = (GLubyte)(kern.r * 255);
    pixel[1] = (GLubyte)(kern.g * 255);
    pixel[2] = (GLubyte)(kern.b * 255);
    pixel[3] = (GLubyte)(kern.a * 255);
    pixel += 4;
  }

  SharedTextureData kernelData = std::make_shared<TextureData>();
  kernelData->setWidth(kernelSize);
  kernelData->setHeight(1);
  kernelData->setData(kernel);
  kernelData->setFormat(GL_RGBA);
  kernelData->setType(GL_UNSIGNED_BYTE);

  m_ssaoKernel = std::make_shared<Texture2D>(GL_RGBA);
  m_ssaoKernel->setMinFilter(GL_NEAREST);
  m_ssaoKernel->setMagFilter(GL_NEAREST);
  m_ssaoKernel->setImageData(kernelData);

  const int noiseSize = 16;
  GLubyte* noise = new GLubyte[noiseSize * 4];
  pixel = noise;

  for (int i = 0; i < noiseSize; ++i) {
    auto nos = glm::vec4(random(-1.0f, 1.0f), random(-1.0f, 1.0f), 0.0f, 0);

    nos = glm::normalize(nos);
    nos = (nos + glm::vec4(1.0, 1.0, 0, 0)) * glm::vec4(0.5, 0.5, 1.0, 1.0);

    pixel[0] = (GLubyte)(nos.r * 255);
    pixel[1] = (GLubyte)(nos.g * 255);
    pixel[2] = (GLubyte)(nos.b * 255);
    pixel[3] = (GLubyte)(nos.a * 255);
    pixel += 4;
  }

  SharedTextureData noiseData = std::make_shared<TextureData>();
  noiseData->setWidth(noiseSize);
  noiseData->setHeight(1);
  noiseData->setData(noise);
  noiseData->setFormat(GL_RGBA);
  noiseData->setType(GL_UNSIGNED_BYTE);

  m_ssaoNoise = std::make_shared<Texture2D>(GL_RGBA);
  m_ssaoNoise->setMinFilter(GL_NEAREST);
  m_ssaoNoise->setMagFilter(GL_NEAREST);
  m_ssaoNoise->setImageData(noiseData);

  m_events->subscribe<"DrawUI"_sh>([this]() {
    ImGui::Begin("Reload");
    if (ImGui::Button("Shaders", {100, 20})) {
      ShaderProgramFileManager::the()->updateAll();
    }
    ImGui::End();

    ImGui::Begin("GBuffer", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Image((void*)m_colorBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_emissiveBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_normalMotionBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_specularBuffer->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_ssaoTarget->getColorAttachments()[0].texture->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::End();
    
    ImGui::Begin("Render Passes", 0, ImGuiWindowFlags_AlwaysAutoResize);
    for (int i = 0; i < m_passes.size(); i++) {
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
  m_gBufferObject->setClearColor(glm::vec4{ 0, 0, 0, 0 });
  m_gBufferObject->bind();
  m_gBufferObject->clearBuffers();

  auto counterStart = SDL_GetPerformanceCounter();

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  auto gBufferRes = m_gBufferObject->getSize();
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

  aaProj = glm::perspectiveFov<float>(glm::radians(cam->fov), windowSize.x, windowSize.y, cam->near, cam->far);
  glm::mat4 viewMatrix = glm::inverse(camTransform);
  glm::mat4 viewMatrixInverse = camTransform;


  glm::mat4 viewProjectionMatrixNoOffset = aaProj * viewMatrix;
  glm::mat4 viewProjectionMatrix = glm::translate(currentOffset) * viewProjectionMatrixNoOffset;
  glm::mat4 prevViewProjectionMatrix = glm::translate(currentOffset) * aaProj * static_cast<glm::mat4>(glm::inverse(trans->lastRenderTransform));


  m_lastGBufferRenderSetupTime = 0;
  m_lastGBufferRenderSubmitTime = 0;

  auto camPos = glm::vec3(camTransform * glm::vec4{ 0, 0, 0, 1 });
  auto camForward = glm::normalize(glm::vec3(camTransform * glm::vec4{ 0, 0, -1, 0 }));


  for (size_t i = 0; i < pass.submittedDrawCallsOpaque.size(); i++) {

      auto counter2Start = SDL_GetPerformanceCounter();

      auto& drawCall = pass.submittedDrawCallsOpaque[i];

      m_lastGBufferRenderSetupTime += (double)(SDL_GetPerformanceCounter() - counter2Start) * 1000.0 / SDL_GetPerformanceFrequency();
      counter2Start = SDL_GetPerformanceCounter();

      drawCall.material.prog->use();

      if (drawCall.material.mainTexture) {
          drawCall.material.prog->setTexture("uTexture", drawCall.material.mainTexture, 0);
          drawCall.material.prog->setUniform("uHasAlbedoMap", true);
      } else {
          drawCall.material.prog->setUniform("uHasAlbedoMap", false);
      }

      if (drawCall.material.normalTexture) {
          drawCall.material.prog->setTexture("uNormalMap", drawCall.material.normalTexture, 1);
          drawCall.material.prog->setUniform("uHasNormalMap", true);
      } else {
          drawCall.material.prog->setUniform("uHasNormalMap", false);
      }

      if (drawCall.material.specularSmoothnessTexture) {
          drawCall.material.prog->setTexture("uSpecularSmoothnessMap", drawCall.material.specularSmoothnessTexture, 2);
          drawCall.material.prog->setUniform("uHasSpecularSmoothnessMap", true);
      } else {
          drawCall.material.prog->setUniform("uHasSpecularSmoothnessMap", false);
      }

      if (drawCall.material.emissiveTexture) {
          drawCall.material.prog->setTexture("uEmissiveMap", drawCall.material.emissiveTexture, 3);
          drawCall.material.prog->setUniform("uHasEmissiveMap", true);
      } else {
          drawCall.material.prog->setUniform("uHasEmissiveMap", false);
      }

      drawCall.material.prog->setUniform("uFar", cam->far);
      drawCall.material.prog->setUniform("uTime", (float)totalTime);
      drawCall.material.prog->setUniform("uTintColor", drawCall.material.tintColor);
      drawCall.material.prog->setUniform("uEmissiveColor", drawCall.material.emissiveColor);

      drawCall.material.prog->setUniform("uModelMatrix", static_cast<glm::mat4>(drawCall.thisRenderTransform));
      drawCall.material.prog->setUniform("uInverseModelMatrix", static_cast<glm::mat4>(glm::inverse(drawCall.thisRenderTransform)));
      drawCall.material.prog->setUniform("uViewProjectionMatrix", viewProjectionMatrix);

      drawCall.material.prog->setUniform("cameraPosition", glm::vec3{ camPos.x, camPos.y, camPos.z });
      drawCall.material.prog->setUniform("uPrevModelMatrix", static_cast<glm::mat4>(drawCall.lastRenderTransform));
      drawCall.material.prog->setUniform("uPrevViewProjectionMatrix", prevViewProjectionMatrix);

      // Draw directly to screen
      if (drawCall.material.cullSide == GL_NONE) {
          glDisable(GL_CULL_FACE);
      } else {
          glEnable(GL_CULL_FACE);
          glCullFace(drawCall.material.cullSide);
      }

      drawCall.geometry.vao->render();

      m_lastGBufferRenderSubmitTime += (double)(SDL_GetPerformanceCounter() - counter2Start) * 1000.0 / SDL_GetPerformanceFrequency();
  }



  m_lastGBufferRenderTime = (double)(SDL_GetPerformanceCounter() - counterStart) * 1000.0 / SDL_GetPerformanceFrequency();
  counterStart = SDL_GetPerformanceCounter();

  // Draw shadow maps
  m_shadowMapProg->use();
  m_shadowMapProg->setUniform("uTime", (float)totalTime);
  glCullFace(GL_FRONT);

  for (size_t i = 0; i < pass.submittedLights.size(); i++) {
    auto light = pass.submittedLights[i];
    if (!light.shadowMap) {
      continue;
    }

    light.shadowFbo->setClearColor(glm::vec4{ 0, 0, 0, 0 });
    light.shadowFbo->bind();
    light.shadowFbo->clearBuffers();

    auto shadowRes = light.shadowFbo->getSize();
    glViewport(0, 0, shadowRes.x, shadowRes.y);

    light.projMatrix = light.projMatrix * interpolate(light.lastSimulateTransform, light.thisSimulateTransform, interp, origPos);

    m_shadowMapProg->setUniform("uViewProjectionMatrix", light.projMatrix);

    for (size_t i = 0; i < pass.submittedDrawCallsOpaque.size(); i++) {
      auto drawCall = pass.submittedDrawCallsOpaque[i];

      if (drawCall.material.castShadow) {
        if (drawCall.material.cullSide == GL_NONE) {
          glDisable(GL_CULL_FACE);
        } else {
          glEnable(GL_CULL_FACE);
          glCullFace(drawCall.material.cullSide);
        }

        m_shadowMapProg->setUniform("uTintColor", drawCall.material.tintColor);
        m_shadowMapProg->setUniform("uModelMatrix", static_cast<glm::mat4>(drawCall.thisRenderTransform));
        drawCall.geometry.vao->render();
      }
    }
  }

  m_lastShadowMapRenderTime = (double)(SDL_GetPerformanceCounter() - counterStart) * 1000.0 / SDL_GetPerformanceFrequency();
  counterStart = SDL_GetPerformanceCounter();

  // attribute-less rendering:
  VertexArrayObject vao;
  vao.bind(); // 'empty' VAO -> no attributes are defined

  if (pass.hasSSAO) {
    auto ssaoRes = glm::vec2(m_ssaoTarget->getSize());
    glViewport(0, 0, ssaoRes.x, ssaoRes.y);

    m_ssaoTarget->bind();
    m_ssaoTarget->setClearColor(glm::vec4{ 0, 0, 0, 0 });
    m_ssaoTarget->clearBuffers();
    
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

    m_ssaoComputeProg->use();
    m_ssaoComputeProg->setTexture("uSamplerNormalMotion", m_normalMotionBuffer, 0);
    m_ssaoComputeProg->setTexture("uSamplerDepth", m_depthBuffer, 1);
    m_ssaoComputeProg->setTexture("uSamplerKernel", m_ssaoKernel, 2);
    m_ssaoComputeProg->setTexture("uSamplerNoise", m_ssaoNoise, 3);
    m_ssaoComputeProg->setUniform("uProjectionMatrix", aaProj);
    m_ssaoComputeProg->setUniform("uViewProjectionMatrix", viewProjectionMatrix);
    m_ssaoComputeProg->setUniform("uViewInverseMatrix", viewMatrixInverse);
    m_ssaoComputeProg->setUniform("uViewProjectionInverseMatrix", glm::inverse(viewProjectionMatrix));
    m_ssaoComputeProg->setUniform("uNear", cam->near);
    m_ssaoComputeProg->setUniform("uFar", cam->far);
    m_ssaoComputeProg->setUniform("uTime", (float)totalTime);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    pass.compositingTarget->bind();
    glViewport(0, 0, pass.compositingTarget->getSize().x, pass.compositingTarget->getSize().y);

    m_ssaoBlurProg->use();
    m_ssaoBlurProg->setTexture("uSamplerSSAO", m_ssaoTarget->getColorAttachments()[0].texture, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  // Deferred lighting (accumulation)
  m_secondaryCompositingBuffer->setClearColor(glm::vec4{ 0, 0, 0, 0 });
  m_secondaryCompositingBuffer->bind();
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  //glDepthMask(GL_FALSE);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);
  glCullFace(GL_BACK);

  auto compositingRes = glm::vec2(m_secondaryCompositingBuffer->getSize());
  glViewport(0, 0, compositingRes.x, compositingRes.y);

  m_deferredCombineProgram->use();
  m_deferredCombineProgram->setTexture("uSamplerColor", m_colorBuffer, 0);
  m_deferredCombineProgram->setTexture("uSamplerEmissive", m_emissiveBuffer, 1);
  m_deferredCombineProgram->setTexture("uSamplerNormalMotion", m_normalMotionBuffer, 2);
  m_deferredCombineProgram->setTexture("uSamplerDepth", m_depthBuffer, 3);
  m_deferredCombineProgram->setTexture("uSamplerSpecularSmoothness", m_specularBuffer, 4);

  if (pass.hasSSAO) {
    m_deferredCombineProgram->setTexture("uSamplerSSAO", pass.compositingTarget->getColorAttachments()[0].texture, 6);
    m_deferredCombineProgram->setUniform("uHasSSAO", true);
  } else {
    m_deferredCombineProgram->setUniform("uHasSSAO", false);
  }

  m_deferredCombineProgram->setUniform("uNear", cam->near);
  m_deferredCombineProgram->setUniform("uFar", cam->far);

  m_deferredCombineProgram->setUniform("uOneOverLightCount", 1.0f/pass.submittedLights.size());

  m_deferredCombineProgram->setUniform("cameraPosition",
    glm::vec3{ camPos.x, camPos.y, camPos.z });

  glm::mat4 invViewProj = glm::inverse(viewProjectionMatrix);
  m_deferredCombineProgram->setUniform("uViewProjectionInverseMatrix",
    invViewProj);



  const static glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0);

  m_deferredCombineProgram->setTexture("uSamplerShadowMap", m_dummyShadowMap, 5);

  for (size_t i = 0; i < pass.submittedLights.size(); i++) {
    auto light = pass.submittedLights[i];

    bool hasShadow = light.shadowFbo != nullptr;

    m_deferredCombineProgram->setUniform("uLightHasShadow", hasShadow);
    m_deferredCombineProgram->setUniform("uLightIsDirectional", light.type == LightType::DIRECTIONAL);


    if (light.type == LightType::DIRECTIONAL) {
      m_deferredCombineProgram->setUniform("uLightDir", light.dir);
    }

    if (hasShadow) {
      m_deferredCombineProgram->setUniform("uLightProjMatrix",
        biasMatrix * light.projMatrix);
      m_deferredCombineProgram->setTexture("uSamplerShadowMap", light.shadowMap, 5);

      auto size = light.shadowMap->getSize();
      m_deferredCombineProgram->setUniform(
        "uOneOverShadowTexSize", glm::vec2{ 1.0f / size.x, 1.0f / size.y });
    }

    glm::vec3 interpolatedLight = light.lastSimulateTransform.pos + (light.thisSimulateTransform.pos - light.lastSimulateTransform.pos) * interp - origPos;
    m_deferredCombineProgram->setUniform("uLightPosition", interpolatedLight);
    m_deferredCombineProgram->setUniform("uLightColor", light.color);
    glDrawArrays(
      GL_TRIANGLE_STRIP, 0,
      4); // create 2 triangles (defined in shader) with no attributes
  }

  m_lastGBufferResolveTime = (double)(SDL_GetPerformanceCounter() - counterStart) * 1000.0 / SDL_GetPerformanceFrequency();
  counterStart = SDL_GetPerformanceCounter();



  // TXAA
  glDisable(GL_BLEND);

  vao.bind(); // 'empty' VAO -> no attributes are defined




  pass.compositingTarget->bind();
  int width = pass.compositingTarget->getSize().x;
  int height = pass.compositingTarget->getSize().y;
  glViewport(0, 0, width, height);

  m_txaaProg->use();
  m_txaaProg->setTexture(
    "uSamplerColor", m_secondaryCompositingBuffer->getColorAttachments()[0].texture,
    0);

  m_txaaProg->setTexture(
    "uSamplerHistory", pass.txaaHistory->getColorAttachments()[0].texture,
    1);

  m_txaaProg->setTexture("uSamplerNormalMotion", m_normalMotionBuffer, 2);
  m_txaaProg->setTexture("uSamplerDepth", m_depthBuffer, 3);

  auto colorSize = glm::vec2(m_secondaryCompositingBuffer->getSize());
  m_txaaProg->setUniform("uOneOverColorSize", glm::vec2(1.0) / colorSize);

  auto motionSize = glm::vec2(m_normalMotionBuffer->getSize());
  m_txaaProg->setUniform("uOneOverMotionSize", glm::vec2(1.0) / motionSize);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes

  if (!pass.renderToTextureOnly) {
    glViewport(0, 0, m_primaryCompositingBuffer->getSize().x, m_primaryCompositingBuffer->getSize().y);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    m_primaryCompositingBuffer->bind();
    m_passBlitProgram->use();
    m_passBlitProgram->setTexture(
      "uSamplerColor", pass.compositingTarget->getColorAttachments()[0].texture,
      0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes

    if (pass.submittedDrawCallsTransparent.size() > 0) {
      m_secondaryCompositingBuffer->bind();
      glDisable(GL_BLEND);

      m_passBlitProgram->use();
      m_passBlitProgram->setTexture(
        "uSamplerColor", m_primaryCompositingBuffer->getColorAttachments()[0].texture,
        0);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes

      // Transparent rendering
      //glBlendFuncSeparate(GL_ONE, GL_ONE, GL_SRC_ALPHA, GL_ONE);
      glCullFace(GL_FRONT);
      for (size_t i = 0; i < pass.submittedDrawCallsTransparent.size(); i++) {
        auto& drawCall = pass.submittedDrawCallsTransparent[i];
        drawCall.material.prog->use();
        drawCall.material.prog->setTexture("uSamplerDepth", m_depthBuffer, 0);
        drawCall.material.prog->setTexture("uSamplerColor", m_primaryCompositingBuffer->getColorAttachments()[0].texture, 1);

        if (drawCall.material.mainTexture) {
          drawCall.material.prog->setTexture("uTexture", drawCall.material.mainTexture, 2);
          drawCall.material.prog->setUniform("uHasAlbedoMap", true);
        } else {
          drawCall.material.prog->setUniform("uHasAlbedoMap", false);
        }

        if (drawCall.material.normalTexture) {
          drawCall.material.prog->setTexture("uNormalMap", drawCall.material.normalTexture, 3);
          drawCall.material.prog->setUniform("uHasNormalMap", true);
        } else {
          drawCall.material.prog->setUniform("uHasNormalMap", false);
        }

        if (drawCall.material.specularSmoothnessTexture) {
          drawCall.material.prog->setTexture("uSpecularSmoothnessMap", drawCall.material.specularSmoothnessTexture, 4);
          drawCall.material.prog->setUniform("uHasSpecularSmoothnessMap", true);
        } else {
          drawCall.material.prog->setUniform("uHasSpecularSmoothnessMap", false);
        }

        drawCall.material.prog->setUniform("uFar", cam->far);
        drawCall.material.prog->setUniform("uTime", (float)totalTime);
        drawCall.material.prog->setUniform("uTintColor", drawCall.material.tintColor);
        drawCall.material.prog->setUniform("uEmissiveColor", drawCall.material.emissiveColor);
        drawCall.material.prog->setUniform("uOneOverScreenSize", 1.0f / compositingRes);

        drawCall.material.prog->setUniform("uModelMatrix", static_cast<glm::mat4>(drawCall.thisRenderTransform));
        drawCall.material.prog->setUniform("uViewProjectionMatrix", viewProjectionMatrixNoOffset);

        drawCall.material.prog->setUniform("uViewProjectionInverseMatrix", invViewProj);

        glm::vec3 pos = { drawCall.thisRenderTransform[3][0], drawCall.thisRenderTransform[3][1], drawCall.thisRenderTransform[3][2] };
        drawCall.material.prog->setUniform("uObjectPosition", pos);
        drawCall.material.prog->setUniform("uCameraPosition", camPos);
        drawCall.material.prog->setUniform("uSunDir", pass.submittedLights[0].dir);
        drawCall.material.prog->setUniform("uPrevModelMatrix", static_cast<glm::mat4>(drawCall.lastRenderTransform));
        drawCall.material.prog->setUniform("uPrevViewProjectionMatrix", prevViewProjectionMatrix);

        drawCall.geometry.vao->render();
      }
      glCullFace(GL_BACK);

      glEnable(GL_BLEND);
      auto temp = m_primaryCompositingBuffer;
      m_primaryCompositingBuffer = m_secondaryCompositingBuffer;
      m_secondaryCompositingBuffer = temp;
    }
  }

  // Swap TAA buffers
  auto temp = pass.compositingTarget;
  pass.compositingTarget = pass.txaaHistory;
  pass.txaaHistory = temp;

  trans->lastRenderTransform = static_cast<glm::dmat4>(camTransform);
}

void RendererSystem::frame(double interp, double totalTime) {
  m_frameIndex++;

  m_primaryCompositingBuffer->setClearColor(glm::vec4{ 0, 0, 0, 0 });
  m_primaryCompositingBuffer->bind();
  m_primaryCompositingBuffer->clearBuffers();

  for (auto& pass : m_passes) {
    if (pass.active) {
      render(pass, interp, totalTime);
    }
    pass.submittedDrawCallsOpaque.reset();
    pass.submittedDrawCallsTransparent.reset();
    pass.submittedLights.reset();
  }

  m_totalLightCount = 0;

  // attribute-less rendering:
  VertexArrayObject vao;
  vao.bind(); // 'empty' VAO -> no attributes are defined

  Uint64 counterStart = SDL_GetPerformanceCounter();

  for (auto& fx : m_effects) {
    fx->apply(m_primaryCompositingBuffer->getColorAttachments()[0].texture, m_postfxTargetBuffer);
    
    auto temp = m_primaryCompositingBuffer;
    m_primaryCompositingBuffer = m_postfxTargetBuffer;
    m_postfxTargetBuffer = temp;
  }

  glViewport(0, 0, m_window->getSize().x, m_window->getSize().y);
  // Blit to backbuffer with tonemapping
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_blitProgram->use();
  m_blitProgram->setTexture(
    "uSamplerColor", m_primaryCompositingBuffer->getColorAttachments()[0].texture,
      0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // create 2 triangles (defined in shader) with no attributes

  m_lastPostprocessingTime = (double)(SDL_GetPerformanceCounter() - counterStart) * 1000.0 / SDL_GetPerformanceFrequency();
}

void RendererSystem::shutdown() {
  for (auto& fx : m_effects) {
    fx->shutdown();
  }
}

glm::mat4 RendererSystem::getProjectionMatrix() {
    return aaProj;
}

GLfloat RendererSystem::getDepthAtPixel(int x, int y) {
    m_gBufferObject->bind();
    
    GLfloat depth;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
};
