in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vPosition;

vec3 inNormal;
vec2 inTexCoord;
vec3 inPosition;
vec3 inPatchDistance;
vec3 inTriDistance;

void setupInputs() {
  inNormal = vNormal;
  inTexCoord = vTexCoord;
  inPosition =vPosition;
}