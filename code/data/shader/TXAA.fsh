#version 410 core

uniform sampler2D uSamplerColor;
uniform sampler2D uSamplerHistory;
uniform sampler2D uSamplerNormalMotion;
uniform sampler2D uSamplerDepth;

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


void main()
{
  vec2 motion = texture(uSamplerNormalMotion, vTexCoord).zw;
  vec2 prevMotion = texture(uSamplerNormalMotion, vTexCoord - motion).zw;
  float depth = texture(uSamplerDepth, vTexCoord).x;
  
  for(int i = 0; i < 4; i++) {
    vec2 sampleCoord = vTexCoord + SAMPLE_OFFSETS[i] * uOneOverMotionSize;
    float depthSample = texture(uSamplerDepth, sampleCoord).x;
    if(depthSample < depth) {
      motion = texture(uSamplerNormalMotion, sampleCoord).zw;
	    depth = depthSample;
    }
  }
  
  float factor = 0.9; //0.5 * max(0, 1.0-30.0 * sqrt(length(motion) - length(prevMotion)));
  
  vec4 history = texture(uSamplerHistory, vTexCoord - motion);  
  vec4 current = texture(uSamplerColor, vTexCoord);
  

  vec4 minNeighbour = current;
  vec4 maxNeighbour = current;
  
  for(int x = -1; x <= 1; x++) {
    for(int y = -1; y <= 1; y++) {
      vec4 colorSample = texture(uSamplerColor, vTexCoord + vec2(x, y)*uOneOverColorSize);
      minNeighbour = min(minNeighbour, colorSample);
      maxNeighbour = max(maxNeighbour, colorSample);
    }
  }
  
  history = clamp(history, minNeighbour, maxNeighbour);
  
  oColor = mix(current, history, factor);
  oColor.a = clamp(oColor.a, 0, 1);
}
