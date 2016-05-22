#version 420

uniform sampler2D uSamplerColor;

const int SAMPLE_COUNT=15;
uniform vec2 uSampleOffsets[SAMPLE_COUNT];
uniform float uSampleWeights[SAMPLE_COUNT];

in vec2 vTexCoord;

out vec3 oColor;

void main()
{
  vec3 color = vec3(0);

  // Combine a number of weighted image filter taps.
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    color += texture(uSamplerColor, vTexCoord + uSampleOffsets[i]).rgb * uSampleWeights[i];
  }

  oColor = color;
}
