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
uniform int uMaxBounces;
uniform int uSampleCount;

uniform float totalTime;
uniform uint uSeed;

const int MAX_TEXTURES = 8;
uniform sampler2D materialTextures[MAX_TEXTURES];

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

vec3 hemisphereSample(float theta, float phi, vec3 n) {
  float xs = sin(theta) * cos(phi);
  float ys = cos(theta);
  float zs = sin(theta) * sin(phi);

  vec3 y = n;
  vec3 h = y;

  if (abs(h.x) <= abs(h.y) && abs(h.x) <= abs(h.z)) {
    h.x = 1.0;
  } else if (abs(h.y) <= abs(h.x) && abs(h.y) <= abs(h.z)) {
    h.y = 1.0;
  } else {
    h.z = 1.0;
  }

  vec3 x = normalize(cross(h, y));
  vec3 z = normalize(cross(x, y));

  return normalize(xs * x + ys * y + zs * z);
}

// =============================================================================
// Material

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

float pow5(float val) {
  return val * val * val * val * val;
}

float fresnel_schlick(vec3 H, vec3 norm, float n1) {
  float r0 = n1 * n1;
  return r0 + (1-r0)*pow5(1 - dot(H, norm));
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

    vec3 outDir;

    vec3 emissiveColor = sampleEmissiveColor(intr);
    vec3 diffuseColor = sampleDiffuseColor(intr);
    vec3 specularColor = intr.material.specularColor;
    vec3 refractionColor = intr.material.specularColor;
    float ior = intr.material.eta;
    
    float inside = sign(dot(r.dir, norm)); // 1 for inside, -1 for outside
    
    float n1 = inside < 0 ? 1.0 / ior : ior;
    float n2 = 1.0 / n1;

    float fresnel = fresnel_schlick(-r.dir, -inside*norm, (n1 - n2)/(n1+n2));

    float rhoS = fresnel;
    float rhoD = (1.0 - fresnel) * (1.0 - intr.material.refractiveness);
    float rhoR = (1.0 - fresnel) * intr.material.refractiveness;
    float rhoE = dot(vec3(1.0/3.0), emissiveColor);
   
    float totalrho = rhoS + rhoD + rhoR + rhoE;
    rhoS /= totalrho;
    rhoD /= totalrho;
    rhoR /= totalrho;
    rhoE /= totalrho;
    
    float rand = uniformFloat(0, 1, random);

    // REFLECT diffuse
    if (rand <= rhoD)
    {
      outDir = directionCosTheta(norm, random);
      weight *= diffuseColor;
    }
    // REFLECT glossy or refract
    else if(rand <= rhoD + rhoS + rhoR) 
    {
      // REFLECT glossy
      if (rand <= rhoD + rhoS)
      {
        outDir = reflect(r.dir, norm);
        weight *= specularColor;
      }
      // REFRACT
      else
      {
        outDir = refract(r.dir, -inside * norm, n1);
        if(dot(outDir, outDir) < 0.9f ) {
          // TOTAL INTERNAL REFLECTION
          outDir = reflect(r.dir, norm);
          weight *= specularColor;
        } else {
          weight *= refractionColor;
        }
      }

      if(intr.material.roughness != 0.0)
      {
        float n = 1.0f / intr.material.roughness;

        float u1 = uniformFloat(0, 1, random), u2 = uniformFloat(0, 1, random);
        float theta = acos(pow(u1, 1.0 / (n + 1)));
        float phi = 2.0 * PI * u2;

        outDir = hemisphereSample(theta, phi, outDir);
      }
    } 
    else 
    {
      color += max(dot(norm, -r.dir), 0.0f) * emissiveColor * weight;
      break;
    }

    color += directIllumination(intr.pos, r.dir, norm, intr.material, random) * weight;

    r.pos = intr.pos;
    r.dir = outDir;
  }
  
  return color;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 imgSize = imageSize(backBuffer);
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

  uint random = wang_hash(wang_hash(uint(totalTime * 1003 + storePos.x * 7)) + uint(totalTime * 5000 + storePos.y * 15001));
/*
  uint random = wang_hash(uint((sin(storePos.x-imgSize.x/2) + cos(storePos.y+imgSize.y/2) )*totalTime*0.005)) + uint(totalTime*10+storePos.x+storePos.y);
  */

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
