#pragma import "CommonDeferredVert.glsl"

vec3 normal() {
  return aNormal;
}

vec2 texCoord() {
  return aTexCoord;
}

vec3 position() {
    vec4 modelPos = vec4(aPosition, 1.0);
    vec4 worldPosition = uModelMatrix * modelPos;    
    return worldPosition.xyz;
}
