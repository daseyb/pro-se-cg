#version 430

const float MAX_DISTANCE = 500;
const float PI = 3.14159265359;

#include "PrimitiveCommon.glsl"
#include "Random.glsl"

struct Payload {
  vec4 col;  
};

uniform vec2 pixelOffset;
uniform int primitiveCount;
uniform int lightCount;

uniform float totalTime;
uniform uint uSeed;

const int MAX_TEXTURES = 8;
uniform sampler2D materialTextures[MAX_TEXTURES];

const int uMaxBounces = 2;
const int uSampleCount = 4;

layout(rgba32f, binding = 0) writeonly uniform image2D backBuffer;

layout(std430, binding = 1) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};

layout(std430, binding = 2) buffer CameraBuffer {
   vec3 pos;
   float fov;
   mat4 invProj;
   mat4 invView;
   mat4 view;
   float lensRadius;
   float focalDistance;
} cam;

layout(std430, binding = 3) buffer LightBuffer {
  SphereLight lights[];
};

layout(std430, binding = 4) buffer MaterialBuffer {
  Material materials[];
};



// =============================================================================
// Helper
vec3 igamma(vec3 color) {
  return pow(color, vec3(2.2));
}

vec3 igamma(float r, float g, float b) {
  return igamma(vec3(r, g, b));
}

Ray generateRay(vec2 screenPos, vec2 screenSize, inout uint random) {
  vec2 subpixel = uniformVec2(vec2(-1), vec2(1), random) / screenSize;

  vec4 near = cam.invProj * vec4(screenPos * 2 - 1 + subpixel, 0.0, 1);
  vec4 far  = cam.invProj * vec4(screenPos * 2 - 1 + subpixel, 0.5, 1);

  near /= near.w;
  far  /= far.w;

  vec3 dir = normalize((far - near).xyz);
  
  Ray result;
  result.pos = vec3(0);
  result.dir = dir;
  
  if (cam.focalDistance > 0 && cam.lensRadius > 0) {
    vec2 lensPos = concentricSampleDisk(random) * cam.lensRadius;

    vec3 pFocus = dir * cam.focalDistance / -dir.z;

    result.pos = vec3(lensPos, 0);
    result.dir = normalize(pFocus - result.pos);
  }
  
  result.pos = (cam.view * vec4(0,0,0, 1)).xyz;
  result.dir = normalize( (transpose(cam.invView) * vec4(result.dir, 0)).xyz);
  return result;
}

// =============================================================================
// Material

// GGX BRDF, no cos(theta), no div by PI
float GGX(vec3 N, vec3 V, vec3 L, float roughness, float F0)
{
    // see http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/
    vec3 H = normalize(V + L);

    float dotLH = max(dot(L, H), 0.0);
    float dotNH = max(dot(N, H), 0.0);
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);

    float alpha = roughness * roughness;

    // D (GGX normal distribution)
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (PI * denom * denom);
    // pi because BRDF

    // F (Fresnel term)
    float F_a = 1.0;
    float F_b = pow(1.0 - dotLH, 5); // manually?
    float F = mix(F_b, F_a, F0);

    // G (remapped hotness, see Unreal Shading)
    float k = (alpha + 2 * roughness + 1) / 8.0;
    float G = dotNL / (mix(dotNL, 1, k) * mix(dotNV, 1, k));
    // '* dotNV' - canceled by normalization


    // '/ dotLN' - canceled by lambert NOT
    // '/ dotNV' - canceled by G
    return D * F * G / 4.0 / dotNL;
}

// GGX for Shading, includes cos(theta), no div by PI
vec3 shadingGGX(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0)
{
    // see http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/
    vec3 H = normalize(V + L);

    float dotLH = max(dot(L, H), 0.0);
    float dotNH = max(dot(N, H), 0.0);
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);

    float alpha = roughness * roughness + 0.0001;

    // D (GGX normal distribution)
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (denom * denom);
    // NO pi because BRDF -> lighting

    // F (Fresnel term)
    vec3 F_a = vec3(1.0);
    vec3 F_b = vec3(pow(1.0 - dotLH, 5)); // manually?
    vec3 F = mix(F_b, F_a, F0);

    // G (remapped hotness, see Unreal Shading)
    float k = (alpha + 2 * roughness + 1) / 8.0;
    float G = dotNL / (mix(dotNL, 1, k) * mix(dotNV, 1, k));
    // '* dotNV' - canceled by normalization


    // '/ dotLN' - canceled by lambert
    // '/ dotNV' - canceled by G
    return max(vec3(0.0), min(vec3(10), D * F * G / 4.0));
}


bool intersect(in Ray r, float maxDist, out HitInfo hit) {
  bool didIntersect = false;
  hit.t = maxDist;
  for(int i = 0; i < primitiveCount; i++) {
    Primitive p = primitives[i];
    
    HitInfo currHit;

    if(intersectPrimitive(r, p, currHit)) {
      if(currHit.t < hit.t) {
        didIntersect = true;
        hit = currHit;
      }
    }
  }
  
  hit.material = materials[hit.matId];
  return didIntersect;
}

// =============================================================================
// Illumination

vec3 getNormalFromTexture(vec3 tex) {
  return normalize(tex * 2 - vec3(1));
}

vec3 sampleNormal(HitInfo hit) {
  Material mat = materials[hit.matId];
  
  vec3 tangentspace_normal = mat.normalTexId == MAX_TEXTURES ? 
                             vec3(0, 0, 1) :
                             getNormalFromTexture(texture(materialTextures[mat.normalTexId], hit.uv).xyz);

  
  vec3 worldNormal = hit.norm;
  worldNormal.xy += tangentspace_normal.xy;
  return normalize(worldNormal);
}

vec3 sampleEmissiveColor(HitInfo hit) {
  Material mat = materials[hit.matId];
  vec4 tex = mat.emissiveTexId == MAX_TEXTURES ? vec4(1) : texture(materialTextures[mat.emissiveTexId], hit.uv);
  return tex.rgb * tex.a * mat.emissiveColor;
}

vec3 sampleDiffuseColor(HitInfo hit) {
  Material mat = materials[hit.matId];
  vec4 tex = mat.diffuseTexId == MAX_TEXTURES ? vec4(1) : texture(materialTextures[mat.diffuseTexId], hit.uv);
  return tex.rgb * mat.diffuseColor;
}

// direct illu at a given point
// inDir points TOWARDS the surface
vec3 directIllumination(vec3 pos, vec3 inDir, vec3 N, Material material, inout uint random) {
  HitInfo intr;
  vec3 color = vec3(0);

  vec3 specularColor = material.specularColor;
  vec3 diffuseColor = material.diffuseColor * (vec3(1) - specularColor);

  uint lightId = uniformUInt(0, lightCount, random);

  vec3 V = -inDir;
  vec3 lPos = vec3(0);
  vec3 lColor = vec3(0);

  if(lightId < lightCount) {
    SphereLight l = lights[lightId];
    lPos = l.center + directionUniformSphere(random) * l.radius;
    lColor = l.color.rgb * l.color.a;
  } else {
    lightId -= lightCount;

    Primitive p = primitives[lightId];
    Material m = materials[p.matId];
    lPos = samplePrimitive(p, random);
    lColor = m.emissiveColor;
  }

  vec3 L = lPos - pos;
  float lightDis = length(L);
  L /= lightDis;
    

  Ray r;
  r.pos = pos;
  r.dir = L;

  if (intersect(r, lightDis, intr)) {
    return vec3(0);
  }

  float p = 1.0/(lightDis * lightDis);
  // diffuse
  return max(0.0, dot(L, N)) * lColor * p;
}

// =============================================================================
// tracing
vec3 trace(Ray r, inout uint random) {
  HitInfo intr;
  
  vec3 color = vec3(0);
  vec3 weight = vec3(1);

  for (int b = 0; b < uMaxBounces; ++b) {
    if (!intersect(r, MAX_DISTANCE, intr)) {
      break;
    }
    
    vec3 norm = sampleNormal(intr);

    vec3 outDir = directionCosTheta(norm, random);
    float pdf = 1;
    //vec3 outDir = directionUniform(norm, random);
    //float pdf = dot(norm, outDir) * 2;

    color += sampleEmissiveColor(intr) * weight;
    weight *= sampleDiffuseColor(intr)  * pdf;

    color += directIllumination(intr.pos, r.dir, norm, intr.material, random) * weight;
    r.pos = intr.pos;
    r.dir = outDir;
  }
  
  return color;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  uint random = wang_hash(wang_hash(uint(totalTime * 1003 + storePos.x * 7)) + uint(totalTime * 5000 + storePos.y * 15001));

  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(vec2(storePos)/vec2(imgSize), imgSize, random);
  
  Payload pl;
  pl.col = vec4(0, 0, 0, 1);
  
  for(int i = 0; i < uSampleCount; i++) {
    pl.col.rgb += trace(r, random);
  }
  
  pl.col.rgb *= 1.0/uSampleCount;
  pl.col.rgb = pl.col.rgb;
  
  imageStore(backBuffer, storePos, pl.col);
 }
