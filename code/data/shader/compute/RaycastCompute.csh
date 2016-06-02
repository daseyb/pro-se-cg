#version 430

#include "PrimitiveCommon.glsl"

#define PI 3.141592683
#define DEG_TO_RAD PI/180.0

layout(rgba32f, binding = 0) writeonly uniform image2D backBuffer;

uniform vec2 pixelOffset;
uniform int primitiveCount;

layout(std140, binding = 1) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};

layout(std140, binding = 2) buffer CameraBuffer {
   vec3 pos;
   float fov;
   vec4 forward;
   vec4 right;
   vec4 up;
} cam;

struct Ray {
  vec3 pos;
  vec3 dir;
};

Ray generateRay(float x, float y, float w, float h) {
  float fovx = cam.fov * DEG_TO_RAD;        // Horizontal FOV
  float fovy = fovx * h / w;  // Vertical FOV

  float halfWidth =  w / 2.0f;
  float halfHeight = h / 2.0f;

  float alpha = tan(fovx / 2.0f) * ((x - halfWidth) / halfWidth);
  float beta = tan(fovy / 2.0f) * ((halfHeight - y) / halfHeight);
  
  Ray result;
  result.pos = cam.pos;
  result.dir = normalize(cam.right.xyz * alpha - cam.up.xyz * beta + cam.forward.xyz);
  return result;
}

struct HitInfo {
  vec3 pos;
  vec3 norm;
  float t;
};

float rand(vec2 co) {
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
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

bool intersectPrimitive(in Ray ray, in Primitive tri, out HitInfo hit) {
    const float INFINITY = 1e10;
    vec3 u, v, n; // triangle vectors
    vec3 w0, w;  // ray vectors
    float r, a, b; // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = tri.b.pos - tri.a.pos;
    v = tri.c.pos - tri.a.pos;
    n = cross(u, v);

    w0 = ray.pos - tri.a.pos;
    a = -dot(n, w0);
    b = dot(n, ray.dir);
    if (abs(b) < 1e-5)
    {
        // ray is parallel to triangle plane, and thus can never intersect.
        return false;
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)
        return false; // ray goes away from triangle.

    vec3 I = ray.pos + r * ray.dir;
    float uu, uv, vv, wu, wv, D;
    uu = dot(u, u);
    uv = dot(u, v);
    vv = dot(v, v);
    w = I - tri.a.pos;
    wu = dot(w, u);
    wv = dot(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)
        return false;

    if(r <= 1e-5) return false;
    
    hit.norm = tri.a.norm * (1.0 - s - t) + tri.b.norm * s + tri.c.norm * t;
    hit.pos = ray.pos + r * ray.dir;
    hit.t = r;
    
    return true;
}


bool findIntersection(in Ray r, out HitInfo hit) {
  bool didIntersect = false;
  hit.t = 10000.0;
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
  return didIntersect;
}

struct Payload {
  vec4 col;  
};

uniform float totalTime;
uniform uint uSeed;

const vec3 lightPos = vec3(0, 8, 10);

void trace(in Ray r, inout Payload pl, inout uint random) {
  
  HitInfo hit;
  if(findIntersection(r, hit)) {
	  vec3 lightOffset = uniformVec3(vec3(-1), vec3(1), random);
    
    vec3 lightDir = lightPos + lightOffset +  - hit.pos;
    float lightDist = length(lightDir);
    lightDir = normalize(lightDir);
    vec3 surfaceNorm = hit.norm;
    r.pos = hit.pos + hit.norm * 0.001;
    r.dir = lightDir;
    pl.col = vec4(0.01, 0.01, 0.01, 1);
    
    if(!findIntersection(r, hit)) {
      pl.col += vec4(vec3(clamp(dot(lightDir, surfaceNorm), 0, 1)) * 50.0/(lightDist*lightDist), 1);
    }
  }
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  uint random = wang_hash(wang_hash( uint(3000*totalTime) + storePos.x * 7) + storePos.y * 15001);

  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(storePos.x + pixelOffset.x, storePos.y + pixelOffset.y, imgSize.x, imgSize.y);
  
  Payload pl;
  pl.col = vec4(0, 0, 0, 1);
  trace(r, pl, random);

  imageStore(backBuffer, storePos, pl.col);
 }