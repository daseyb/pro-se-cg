#pragma once

#include <engine/graphics/PostFX.hpp>
#include <array>

class BloomPostFX : public PostFX {
private:
  glow::SharedProgram m_blitProgram;
  glow::SharedProgram m_extractProgram;
  glow::SharedProgram m_blurProgram;

  glow::SharedFramebuffer m_extractBuffer;
  glow::SharedFramebuffer m_blurBufferHorizontal;
  glow::SharedFramebuffer m_blurBufferVertical;

  glow::SharedTexture2D m_extractTexture;
  glow::SharedTexture2D m_blurTextureHorizontal;
  glow::SharedTexture2D m_blurTextureVertical;

  std::array<float, 15> m_gaussianWeights;
  std::array<float, 15> m_sampleOffsets;

public:
  BloomPostFX(RendererSystem* renderer, EventSystem* events, QualitySetting quality) : PostFX(renderer, events, quality) { };

  void startup() override;
  void apply(glow::SharedTexture2D inputBuffer, glow::SharedFramebuffer outputBuffer) override;
  void shutdown() override;
};