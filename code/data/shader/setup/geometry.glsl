in vec3 gNormal;
in vec2 gTexCoord;
in vec3 gPosition;
in vec3 gPatchDistance;
in vec3 gTriDistance;

vec3 inNormal;
vec2 inTexCoord;
vec3 inPosition;
vec3 inPatchDistance;
vec3 inTriDistance;

void setupInputs() {
  inNormal = gNormal;
  inTexCoord = gTexCoord;
  inPosition = gPosition;
  inPatchDistance = gPatchDistance;
  inTriDistance = gTriDistance;
}