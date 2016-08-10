#version 410 core

uniform sampler2D uSamplerColor;
uniform sampler2D uSamplerHistory;
uniform sampler2D uSamplerNormalMotion;
uniform sampler2D uSamplerDepth;
uniform sampler2D uSamplerPrevDepth;

uniform float uAlpha;

uniform vec2 uOneOverColorSize;
uniform vec2 uOneOverMotionSize;

in vec2 vTexCoord;

out vec4 oColor;

const vec2 SAMPLE_OFFSETS[4] = vec2[] (
    vec2(-2.0,  2.0),
    vec2(-2.0, -2.0),
    vec2( 2.0,  2.0),
    vec2( 2.0, -2.0)
);

float linearizeDepth(float depth, float near, float far) {
    return (2 * near) / (far + near - depth * (far - near));
}


void main()
{
  vec2 motion =  texture(uSamplerNormalMotion, vTexCoord).zw;
  vec2 prevMotion = texture(uSamplerNormalMotion, vTexCoord - motion).zw;
  
  float factor = uAlpha;
  
  vec4 current = texture(uSamplerColor, vTexCoord);
    
  vec2 prevSamplePos = vTexCoord - motion;
  if(min(prevSamplePos.x, prevSamplePos.y) < 0 || max(prevSamplePos.x, prevSamplePos.y) > 1) {
      oColor = current;
      return;
  }
  
  vec4 history = texture(uSamplerHistory, vTexCoord - motion);  

  float depth = linearizeDepth(texture(uSamplerDepth, vTexCoord).x, 0.01, 100);
  float prevDepth = linearizeDepth(texture(uSamplerPrevDepth, vTexCoord-motion).x, 0.01, 100);

  if(abs(depth-prevDepth) > 0.015) {
      factor = 0;
  }
  
  oColor = mix(current, history, factor);
  oColor.a = clamp(oColor.a, 0, 1);
}
