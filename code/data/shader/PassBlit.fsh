#version 330

uniform sampler2D uSamplerColor;

in vec2 vTexCoord;

out vec4 oColor;

void main()
{
  oColor = texture(uSamplerColor, vTexCoord);
  oColor.a = min(1, max(0, oColor.a));
}
