#pragma import "setup/vertex.glsl"

#pragma import "CommonDeferredFrag.glsl"
#pragma import "Utils.glsl"

void initShader(){};

vec4 color() {
  vec4 res = uTintColor;
  if(uHasAlbedoMap) {
      res *= texture(uTexture, vTexCoord).rgba;
  }
  return res;
}

vec4 emissive() {
  if(uHasEmissiveMap) {
     return texture(uEmissiveMap, vTexCoord);
  }
  return uEmissiveColor;
}

vec3 normal() {
  if(false && uHasNormalMap) {
    vec3 tangentNormal = -normalize(texture(uNormalMap, vTexCoord).xyz * 2 - vec3(1.0));
    
    vec3 Q1 = dFdx(vPosition);
    vec3 Q2 = dFdy(vPosition);
    vec2 st1 = dFdx(vTexCoord);
    vec2 st2 = dFdy(vTexCoord);

    vec3 T = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B = normalize(-Q1*st2.s + Q2*st1.s);

    // the transpose of texture-to-eye space matrix
    mat3 TBN = mat3(T, B, vNormal);

    // transform the normal to eye space 
    return normalize(tangentNormal*TBN);
  }
  return vNormal;
}

vec4 specularSmoothness() {
    if(uHasSpecularSmoothnessMap) {
      return texture(uSpecularSmoothnessMap, vTexCoord);
    }
    return vec4(1, 1, 1, 1);
}
