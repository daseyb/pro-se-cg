#version 330

uniform sampler2D uSamplerColor;
uniform float uThreshold;

in vec2 vTexCoord;

out vec4 oColor;

float luminance(vec3 color) {
  return 0.2126 * color.r + 0.7152*color.g + 0.0722*color.b;
}

void main()
{
  vec3 texColor = texture(uSamplerColor, vTexCoord).rgb;
  
  if(luminance(texColor) < uThreshold) {
    texColor = vec3(0);
  }
  
  oColor = vec4(texColor,1);
}
