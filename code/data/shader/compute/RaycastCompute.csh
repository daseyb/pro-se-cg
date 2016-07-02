#version 430

#include "PrimitiveCommon.glsl"


const float MAX_DISTANCE = 500;
const float EPSILON = 0.001;
const float PI = 3.14159265359;

struct Payload {
  vec4 col;  
};

uniform vec2 pixelOffset;
uniform int primitiveCount;
uniform float totalTime;
uniform uint uSeed;
uniform int uMaxBounces = 2;

const vec3 lightPos = vec3(0, 8, 10);
const vec3 dielecSpec = vec3(0.0);

const Material[] materials = Material[]( Material( vec3(0.3, 0.4, 0.4), dielecSpec, 0.9, 0.0, 1.0) );
const SphereLight[] lights = SphereLight[](
  SphereLight( vec3(0, 8, 8), 2.0, vec3(1.0, 0.3, 0.9) ),
  SphereLight( vec3(-8, 8, 8), 1.0, vec3(0.9, 0.9, 1.0) )
);

layout(rgba32f, binding = 0) writeonly uniform image2D backBuffer;

layout(std140, binding = 1) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};

layout(std140, binding = 2) buffer CameraBuffer {
   vec3 pos;
   float fov;
   mat4 invProj;
   mat4 invView;
} cam;



// =============================================================================
// Helper
vec3 igamma(vec3 color) {
  return pow(color, vec3(2.2));
}

vec3 igamma(float r, float g, float b) {
  return igamma(vec3(r, g, b));
}

uint wang_hash(uint seed)
{
  seed = (seed ^ 61) ^ (seed >> 16);
  seed *= 9;
  seed = seed ^ (seed >> 4);
  seed *= 0x27d4eb2d;
  seed = seed ^ (seed >> 15);
  return seed;
}

float wang_float(uint hash)
{
  return hash / float(0x7FFFFFFF) / 2.0;
}

float uniformFloat(float min, float max, inout uint random) {
  random = wang_hash(random);
  return (max - min) * wang_float(random) + min;
}

vec2 uniformVec2(vec2 min, vec2 max, inout uint random) {
  float x = uniformFloat(min.x, max.x, random);
  float y = uniformFloat(min.y, max.y, random);
  return vec2(x, y);
}

vec3 uniformVec3(vec3 min, vec3 max, inout uint random) {
  float x = uniformFloat(min.x, max.x, random);
  float y = uniformFloat(min.y, max.y, random);
  float z = uniformFloat(min.z, max.z, random);
  return vec3(x, y, z);
}

vec3 directionCosTheta(vec3 normal, inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);

  float f = sqrt(1 - u1);

  float x = f * cos(phi);
  float y = f * sin(phi);
  float z = sqrt(u1);

  vec3 xDir = abs(normal.x) < abs(normal.y) ? vec3(1, 0, 0) : vec3(0, 1, 0);
  vec3 yDir = normalize(cross(normal, xDir));
  xDir = cross(yDir, normal);
  return xDir * x + yDir * y + z * normal;
}

vec3 directionUniformSphere(inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);
  float f = sqrt(1 - u1 * u1);

  float x = f * cos(phi);
  float y = f * sin(phi);
  float z = u1;

  vec3 dir = vec3(x, y, z);

  return dir;
}

vec3 directionUniform(vec3 normal, inout uint random) {
  float u1 = uniformFloat(0, 1, random);
  float phi = uniformFloat(0, 2 * PI, random);
  float f = sqrt(1 - u1 * u1);

  float x = f * cos(phi);
  float y = f * sin(phi);
  float z = u1;

  vec3 dir = vec3(x, y, z);
  dir *= sign(dot(dir, normal));

  return dir;
}

Ray generateRay(vec2 screenPos, vec2 screenSize, inout uint random) {
  vec2 subpixel = uniformVec2(vec2(-1), vec2(1), random) / screenSize;

  vec4 near = cam.invProj * vec4(screenPos * 2 - 1 + subpixel, 0.0, 1);
  vec4 far  = cam.invProj * vec4(screenPos * 2 - 1 + subpixel, 0.5, 1);

  near /= near.w;
  far  /= far.w;

  near = cam.invView * near;
  far  = cam.invView * far;

  vec3 dir = normalize((far - near).xyz);
  
  Ray result;
  result.pos = cam.pos;
  result.dir = dir;
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
      didIntersect = true;
      
      if(currHit.t < hit.t) {
        hit = currHit;
      }
    }
  }
  
  hit.material = materials[hit.matId];
  return didIntersect;
}



// =============================================================================
// Illumination

// direct illu at a given point
// inDir points TOWARDS the surface
vec3 directIllumination(vec3 pos, vec3 inDir, vec3 N, Material material, uint random) {
  HitInfo intr;
  vec3 color = vec3(0);

  vec3 specularColor = material.specularColor;
  vec3 diffuseColor = material.diffuseColor * (vec3(1) - specularColor);

  int lightCnt = lights.length();
  for (int i = 0; i < lightCnt; ++i) {
    SphereLight l = lights[i];

    vec3 V = -inDir;
    vec3 L = l.center - pos + directionUniformSphere(random) * l.radius;
    float lightDis = length(L);
    L /= lightDis;
    
    Ray r;
    r.pos = pos;
    r.dir = L;

    if (intersect(r, lightDis, intr))
      continue;

    // diffuse
    color += max(0.0, dot(L, N)) * l.color* diffuseColor;

    // specular
    color += shadingGGX(N, V, L, material.roughness, specularColor);
  }

  return color;
}

// =============================================================================
// tracing
vec3 trace(Ray r, inout uint random) {
  HitInfo intr;
  int bounces = 3;

  vec3 pos = r.pos;
  vec3 dir = r.dir;
  vec3 weight = vec3(1);
  
  if (!intersect(r, MAX_DISTANCE, intr))
      return vec3(0);

  for (int b = 0; b < bounces; ++b) {
    r.pos = pos;
    r.dir = dir;
    
    // step 2: russian roulette
    float rhoS = dot(vec3(1,1,1), intr.material.specularColor);
    float rhoD = dot(vec3(1,1,1), intr.material.diffuseColor);
    float rhoR = intr.material.refractiveness;

    // REFLECT glossy
    if (uniformFloat(0, 1, random) <= rhoS)
    {
      dir = reflect(dir, intr.norm);
      pos = intr.pos;
      weight *= 1; // specular
    }
    // REFLECT diffuse
    else if (uniformFloat(0, 1, random) <= rhoD)
    {
      dir = directionCosTheta(intr.norm, random);
      pos = intr.pos;
      weight *= 1; //dot(dir, intr.norm); // cos theta vs p(w), f vs. rho, pi in p(w) and f
    }
    // REFRACT
    else if (uniformFloat(0, 1, random) <= rhoR)
    {
      float inside = sign(dot(dir, intr.norm)); // 1 for inside, -1 for outside
      dir = refract(dir, -inside * intr.norm, inside < 0 ? 1 / intr.material.eta : intr.material.eta);
      pos = intr.pos;
      weight *= 1; // refractive
    }
    // EMIT
    else
    {
      vec3 color = directIllumination(intr.pos, dir, intr.norm, intr.material, random);
      weight *= color / (1 - rhoS) / (1 - rhoD) / (1 - rhoR);
    }
    
    // step 1: intersect with scene
    if (!intersect(r, MAX_DISTANCE, intr))
      break; // TODO: cubemap
  }

  // last bounce
  vec3 color = directIllumination(intr.pos, dir, intr.norm, intr.material, random);
  return weight * color;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  uint random = wang_hash(wang_hash( uint(3000*totalTime) + storePos.x * 7) + storePos.y * 15001);

  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(vec2(storePos)/vec2(imgSize), imgSize, random);
  
  Payload pl;
  pl.col = vec4(0, 0, 0, 1);
  
  for(int i = 0; i < 2; i++) {
    pl.col.rgb += trace(r, random);
  }
  
  pl.col.rgb *= 1.0/2;
  
  imageStore(backBuffer, storePos, pl.col);
 }