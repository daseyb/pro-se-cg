#pragma once

#include <engine/graphics/PostFX.hpp>
#include <array>

class BloomPostFX : public PostFX {
private:
  ACGL::OpenGL::SharedShaderProgram m_blitProgram;
  ACGL::OpenGL::SharedShaderProgram m_extractProgram;
  ACGL::OpenGL::SharedShaderProgram m_blurProgram;

  ACGL::OpenGL::SharedFrameBufferObject m_extractBuffer;
  ACGL::OpenGL::SharedFrameBufferObject m_blurBufferHorizontal;
  ACGL::OpenGL::SharedFrameBufferObject m_blurBufferVertical;

  ACGL::OpenGL::SharedTexture2D m_extractTexture;
  ACGL::OpenGL::SharedTexture2D m_blurTextureHorizontal;
  ACGL::OpenGL::SharedTexture2D m_blurTextureVertical;

  std::array<float, 15> m_gaussianWeights;
  std::array<float, 15> m_sampleOffsets;

public:
  BloomPostFX(RendererSystem* renderer, EventSystem* events, QualitySetting quality) : PostFX(renderer, events, quality) { };

  void startup() override;
  void apply(ACGL::OpenGL::ConstSharedTextureBase inputBuffer, ACGL::OpenGL::SharedFrameBufferObject outputBuffer) override;
  void shutdown() override;
};