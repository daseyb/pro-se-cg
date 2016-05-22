#include <engine/graphics/BloomPostFX.hpp>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/ui/UISystem.hpp>

using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

float computeGaussian(float n) {
  float theta = 4;
  float val = ((float)((1.0f / sqrtf(2 * (float)M_PI * theta)) * exp(-(n * n) / (2.0f * theta * theta))) - 0.001f);
  if (val < 0) val = 0;
  return val;
}

void BloomPostFX::startup() {


  m_extractTexture = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_extractBuffer = SharedFrameBufferObject(new FrameBufferObject());
  m_extractBuffer->attachColorTexture("oColor", m_extractTexture);
  m_extractBuffer->validate();
  m_extractBuffer->setClearColor(glm::vec4(0, 0, 0, 0));

  m_blurTextureHorizontal = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_blurBufferHorizontal = SharedFrameBufferObject(new FrameBufferObject());
  m_blurBufferHorizontal->attachColorTexture("oColor", m_blurTextureHorizontal);
  m_blurBufferHorizontal->validate();
  m_blurBufferHorizontal->setClearColor(glm::vec4(0, 0, 0, 0));

  m_blurTextureVertical = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_blurBufferVertical = SharedFrameBufferObject(new FrameBufferObject());
  m_blurBufferVertical->attachColorTexture("oColor", m_blurTextureVertical);
  m_blurBufferVertical->validate();
  m_blurBufferVertical->setClearColor(glm::vec4(0, 0, 0, 0));

  m_blitProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("Bloom/Blit"));
  m_extractProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("Bloom/Extract"));
  m_blurProgram = ShaderProgramFileManager::the()->get(ShaderProgramCreator("Bloom/Blur").fragmentDataLocations(m_blurBufferHorizontal->getAttachmentLocations()));

  size_t sampleCount = m_gaussianWeights.size();
  // The first sample always has a zero offset.
  m_gaussianWeights[0] = computeGaussian(0);
  m_sampleOffsets[0] = 0;
  // Maintain a sum of all the weighting values.
  float totalWeights = m_gaussianWeights[0];
  // Add pairs of additional sample taps, positioned
  // along a line in both directions from the center.
  for (size_t i = 0; i < sampleCount / 2; i++) {
    // Store weights for the positive and negative taps.
    float weight = computeGaussian((float)i + 1);
    m_gaussianWeights[i * 2 + 1] = weight;
    m_gaussianWeights[i * 2 + 2] = weight;
    totalWeights += weight * 2;

    float sampleOffset = i + .5f;
    m_sampleOffsets[i * 2 + 1] = sampleOffset;
    m_sampleOffsets[i * 2 + 2] = -sampleOffset;
  }

  // Normalize the list of sample weightings, so they will always sum to one.
  for (size_t i = 0; i < m_gaussianWeights.size(); i++) {
    m_gaussianWeights[i] /= totalWeights;
  }

  /*m_events->subscribe<"DrawUI"_sh>([this]() {
    ImGui::Begin("Bloom", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Image((void*)m_extractTexture->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)m_blurTexture->getObjectName(), glm::vec2{ 1280, 720 } *0.2f, { 0, 1 }, { 1, 0 });
    ImGui::End();
  });*/
}

void BloomPostFX::apply(ACGL::OpenGL::ConstSharedTextureBase inputBuffer, ACGL::OpenGL::SharedFrameBufferObject outputBuffer) {

  auto bloomSize = glm::vec2(m_extractTexture->getWidth(), m_extractTexture->getHeight());

  glDisable(GL_BLEND);

  // Extract bright pixels
  m_extractBuffer->bind();
  m_extractBuffer->clearBuffers();
  glViewport(0, 0, (int)bloomSize.x, (int)bloomSize.y);

  m_extractProgram->use();
  m_extractProgram->setTexture("uSamplerColor", inputBuffer, 0);
  m_extractProgram->setUniform("uThreshold", 0.9f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  std::array<glm::vec2, 15> offsets;

  const static float weights[] = { 1, 0.75, 0.25 };
  const static float scales[] = { 1, 2, 4.0 };

  outputBuffer->bind();
  outputBuffer->clearBuffers();
  glViewport(0, 0, inputBuffer->getWidth(), inputBuffer->getHeight());
  m_blitProgram->use();
  m_blitProgram->setTexture("uSamplerBlur", inputBuffer, 0);
  m_blitProgram->setUniform("uBloomFactor", 1.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  auto blurSrc = m_extractTexture;

  for (int pass = 0; pass < (int)m_quality + 1; pass++) {
    glViewport(0, 0, (int)bloomSize.x, (int)bloomSize.y);
    // Blur horizontally
    m_blurBufferHorizontal->bind();
    m_blurBufferHorizontal->clearBuffers();
    m_blurProgram->use();
    m_blurProgram->setUniform("uSampleWeights", m_gaussianWeights.size(), m_gaussianWeights.data());

    m_blurProgram->setTexture("uSamplerColor", blurSrc, 0);
    for (size_t i = 0; i < offsets.size(); i++) {
      offsets[i] = glm::vec2(m_sampleOffsets[i] * scales[pass] / blurSrc->getWidth(), 0);
    }
    m_blurProgram->setUniform("uSampleOffsets", offsets.size(), offsets.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Blur vertically
    m_blurBufferVertical->bind();
    m_blurBufferVertical->clearBuffers();
    m_blurProgram->setTexture("uSamplerColor", m_blurTextureHorizontal, 0);

    for (size_t i = 0; i < offsets.size(); i++) {
      offsets[i] = glm::vec2(0, m_sampleOffsets[i] * scales[pass] / m_blurTextureHorizontal->getHeight());
    }
    m_blurProgram->setUniform("uSampleOffsets", offsets.size(), offsets.data());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Add to output
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glViewport(0, 0, inputBuffer->getWidth(), inputBuffer->getHeight());
    outputBuffer->bind();
    m_blitProgram->use();
    m_blitProgram->setTexture("uSamplerBlur", m_blurTextureVertical, 0);
    m_blitProgram->setUniform("uBloomFactor", weights[pass]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);

    blurSrc = m_blurTextureVertical;
  }
}

void BloomPostFX::shutdown() {
}
