#version 330

uniform sampler2D uSamplerBlur;
uniform float uBloomFactor;

in vec2 vTexCoord;

out vec4 oColor;

void main()
{
  vec3 blurColor = texture(uSamplerBlur, vTexCoord).rgb;
  
  oColor = vec4(blurColor, uBloomFactor);
}
