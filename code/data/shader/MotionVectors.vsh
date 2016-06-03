#include "CommonDeferredVert.glsl"

vec3 normal() {
  return aNormal.xyz;
}

vec2 texCoord() {
  return aTexCoord;
}

vec3 position() {
    return (uModelMatrix * vec4(aPosition.xyz, 1)).xyz;
}
