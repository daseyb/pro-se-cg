#include <engine/graphics/BloomPostFX.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/ui/UISystem.hpp>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>

using namespace glow;


float computeGaussian(float n) {
  float theta = 4;
  float val = ((float)((1.0f / sqrtf(2 * (float)M_PI * theta)) * exp(-(n * n) / (2.0f * theta * theta))) - 0.001f);
  if (val < 0) val = 0;
  return val;
}

void BloomPostFX::startup() {

  m_extractTexture = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_extractBuffer = Framebuffer::create({{ "oColor", m_extractTexture }});

  m_blurTextureHorizontal = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_blurBufferHorizontal = Framebuffer::create({ { "oColor", m_blurTextureHorizontal } });

  m_blurTextureVertical = m_renderer->createScreenspaceTexture(ScreenSpaceSize::HALF, GL_RGBA32F);
  m_blurBufferVertical = Framebuffer::create({ { "oColor", m_blurTextureVertical } });

  m_blitProgram = Program::createFromFile("Bloom/Blit");
  m_extractProgram = Program::createFromFile("Bloom/Extract");
  m_blurProgram = Program::createFromFile("Bloom/Blur");

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

}

void BloomPostFX::apply(glow::SharedTexture2D inputBuffer, glow::SharedFramebuffer outputBuffer) {

  auto bloomSize = glm::vec2(m_extractTexture->getWidth(), m_extractTexture->getHeight());

  glDisable(GL_BLEND);

  // Extract bright pixels
  auto boundBuffer = m_extractBuffer->bind();
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, (int)bloomSize.x, (int)bloomSize.y);

  auto extractProgramUsed = m_extractProgram->use();
  extractProgramUsed.setTexture("uSamplerColor", inputBuffer);
  extractProgramUsed.setUniform("uThreshold", 0.9f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  std::array<glm::vec2, 15> offsets;

  const static float weights[] = { 1, 0.75, 0.25 };
  const static float scales[] = { 1, 2, 4.0 };
  
  {
      auto boundFB = outputBuffer->bind();
      glClear(GL_COLOR_BUFFER_BIT);
      glViewport(0, 0, inputBuffer->getWidth(), inputBuffer->getHeight());
      auto blitProgramUsed = m_blitProgram->use();
      blitProgramUsed.setTexture("uSamplerBlur", inputBuffer);
      blitProgramUsed.setUniform("uBloomFactor", 1.0f);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  auto blurSrc = m_extractTexture;

  for (int pass = 0; pass < (int)m_quality + 1; pass++) {
    glViewport(0, 0, (int)bloomSize.x, (int)bloomSize.y);

    {
      auto blurProgramUsed = m_blurProgram->use();

      {
        // Blur horizontally
        auto boundFB = m_blurBufferHorizontal->bind();
        blurProgramUsed.setUniform("uSampleWeights", m_gaussianWeights.size(),
                                   m_gaussianWeights.data());

        blurProgramUsed.setTexture("uSamplerColor", blurSrc);
        for (size_t i = 0; i < offsets.size(); i++) {
          offsets[i] = glm::vec2(
              m_sampleOffsets[i] * scales[pass] / blurSrc->getWidth(), 0);
        }
        blurProgramUsed.setUniform("uSampleOffsets", offsets.size(),
                                   offsets.data());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      }

      {
        // Blur vertically
        auto boundFB = m_blurBufferVertical->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        blurProgramUsed.setTexture("uSamplerColor", m_blurTextureHorizontal);

        for (size_t i = 0; i < offsets.size(); i++) {
          offsets[i] = glm::vec2(0, m_sampleOffsets[i] * scales[pass] /
                                        m_blurTextureHorizontal->getHeight());
        }
        blurProgramUsed.setUniform("uSampleOffsets", offsets.size(),
                                   offsets.data());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      }
    }

    {
      // Add to output
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glViewport(0, 0, inputBuffer->getWidth(), inputBuffer->getHeight());
      auto boundFB = outputBuffer->bind();
      auto blitProgramUsed = m_blitProgram->use();
      blitProgramUsed.setTexture("uSamplerBlur", m_blurTextureVertical);
      blitProgramUsed.setUniform("uBloomFactor", weights[pass]);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glDisable(GL_BLEND);
    }
    blurSrc = m_blurTextureVertical;
  }
}

void BloomPostFX::shutdown() {
}
