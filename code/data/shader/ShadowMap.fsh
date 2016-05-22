#version 410 core

in vec3 tPosition;
in vec2 tTexCoord;

uniform sampler2D uTexture;
uniform vec4 uTintColor;
uniform float uTime;

layout(location = 0) out vec4 oDepth;

void main()
{    
  oDepth = vec4(vec3(gl_FragCoord.z), 1);
}
