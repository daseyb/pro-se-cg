#version 410 core

uniform sampler2D uSamplerColor;
uniform sampler2D uSamplerEmissive;
uniform sampler2D uSamplerNormalMotion;
uniform sampler2D uSamplerMotion;
uniform sampler2D uSamplerDepth;
uniform sampler2D uSamplerSpecularSmoothness;

uniform sampler2DShadow  uSamplerShadowMap;

uniform sampler2D uSamplerSSAO;
uniform bool uHasSSAO;

uniform bool uLightHasShadow;
uniform vec2 uOneOverShadowTexSize;
uniform vec3 uLightPosition;
uniform vec4 uLightColor;
uniform mat4 uLightProjMatrix;
uniform bool uLightIsDirectional;
uniform vec3 uLightDir;

uniform float uOneOverLightCount;

uniform float uNear;
uniform float uFar;

uniform vec3 cameraPosition;
uniform mat4 uViewProjectionInverseMatrix;

in vec2 vTexCoord;
out vec4 oColor;

#pragma import "Utils.glsl"
#pragma import "Lighting.glsl"

float linearizeDepth(float depth) {
  return  (2.0 * uNear) / (uFar + uNear - depth * (uFar - uNear));  // convert to linear values
}

float unpackDepth(float depth) {
  return depth;
}

vec3 unpackWorldPosition(float depth) {
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = vTexCoord * 2.0 - 1.0;
    clipSpaceLocation.z = depth * 2.0 - 1.0;
    clipSpaceLocation.w = 1.0;
    vec4 homogenousLocation = uViewProjectionInverseMatrix * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 projShadowCoord(vec3 worldPosition) {
  vec4 shadowCoord =  uLightProjMatrix * vec4(worldPosition, 1); 
  vec3 clipShadowCoord = shadowCoord.xyz/shadowCoord.w;
  return clipShadowCoord;
}

float shadowFactor(vec3 worldPosition) {
  if(!uLightHasShadow) return 1;
  
  vec3 shadowCoord = projShadowCoord(worldPosition.xyz);
  
  float x,y;
  float shadow = 0;
  for (y = -1.5 ; y <=1.5 ; y+=1.0) {
    for (x = -1.5 ; x <=1.5 ; x+=1.0) {
      shadow += texture(uSamplerShadowMap, shadowCoord + vec3(x * uOneOverShadowTexSize.x , y * uOneOverShadowTexSize.y, -0.005));
    }
  }

  shadow /= 16.0 ;

  return shadow;
}

float ssaoFactor() {
  if(!uHasSSAO) return 1;
  return texture(uSamplerSSAO, vTexCoord).r;
}

void main()
{
    vec4 albedo  = texture(uSamplerColor, vTexCoord);
    vec4 specularSmoothness  = texture(uSamplerSpecularSmoothness, vTexCoord);
    vec4 emissive  = texture(uSamplerEmissive, vTexCoord);
    vec3 normal = unpackNormal(texture(uSamplerNormalMotion, vTexCoord));
    
 
    float depth = unpackDepth(texture(uSamplerDepth, vTexCoord).r);
    
    if(depth == 1) discard;
	
    vec3 worldPosition = unpackWorldPosition(depth);
    
    vec3 lightDir = vec3(0); 
    float lightDistance = 1;
    float attenuation = 1;
	
    if(uLightIsDirectional) {
      lightDir = uLightDir;
    } else {
      lightDir = uLightPosition - worldPosition;
      lightDistance = length(lightDir);
      lightDir /= lightDistance;	
      attenuation = max(1, 1.0/pow(lightDistance, 2));
    }

    vec3 viewDir = normalize(cameraPosition - worldPosition);
    
    float oneMinusReflectivity;
    albedo.rgb = EnergyConservationBetweenDiffuseAndSpecular (albedo.rgb, specularSmoothness.rgb, /*out*/ oneMinusReflectivity);

    float lightNdotL = max(0, dot(lightDir, normal));
    vec4 lighting =  BRDF (albedo.rgb, specularSmoothness.rgb, oneMinusReflectivity, specularSmoothness.a, normal, viewDir, lightDir, lightNdotL, uLightColor.rgb * uLightColor.a);
    
    float ssao = ssaoFactor();
    
    
    
    oColor = vec4(lighting.rgb * ssao * shadowFactor(worldPosition) * attenuation  + emissive.rgb, uOneOverLightCount * albedo.a);
}
