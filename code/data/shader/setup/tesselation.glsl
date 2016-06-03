in vec3 teNormal;
in vec2 teTexCoord;
in vec3 tePosition;

vec3 inNormal;
vec2 inTexCoord;
vec3 inPosition;
vec3 inPatchDistance;
vec3 inTriDistance;

void setupInputs() {
  inNormal = teNormal;
  inTexCoord = teTexCoord;
  inPosition = tePosition;
}