#version 430

#define PI 3.141592683
#define DEG_TO_RAD PI/180.0

layout(rgba32f, binding = 0) writeonly uniform image2D backBuffer;

struct Primitive {
  vec3 pos;
  float r;
};

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
  result.dir = normalize(cam.right.xyz * alpha + cam.up.xyz * beta + cam.forward.xyz);
  return result;
}

struct HitInfo {
  vec3 pos;
  vec3 norm;
  float t;
};


bool intersectPrimitive(in Ray ray, in Primitive sphere, out HitInfo hit) {
    vec3 oc = ray.pos - sphere.pos;
    float b = 2.0 * dot(ray.dir, oc);
    float c = dot(oc, oc) - sphere.r*sphere.r;
    float disc = b * b - 4.0 * c;

    if (disc < 0.0)
        return false;

    // compute q as described above
    float q;
    if (b < 0.0)
        q = (-b - sqrt(disc))/2.0;
    else
        q = (-b + sqrt(disc))/2.0;

    float t0 = q;
    float t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1) {
        // if t0 is bigger than t1 swap them around
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0.0) {
        return false;
    }
    
    // if t0 is less than zero, the intersection point is at t1
    if (t0 < 0.0) {
        hit.t = t1;
    } else {
        hit.t = t0; 
    }
    
    hit.pos = ray.pos + hit.t * ray.dir;
    hit.norm = normalize(hit.pos - sphere.pos);
    
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

void trace(in Ray r, inout Payload pl) {
  
  HitInfo hit;
  if(findIntersection(r, hit)) {
    vec3 lightDir = normalize(vec3(-1));
    vec3 surfaceNorm = hit.norm;
    r.pos = hit.pos + hit.norm * 0.001;
    r.dir = lightDir;
    pl.col = vec4(0.01, 0.01, 0.01, 1);
    
    if(!findIntersection(r, hit)) {
      pl.col += vec4(vec3(clamp(dot(lightDir, surfaceNorm), 0, 1)), 1);
    }
  }
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(storePos.x + pixelOffset.x, storePos.y + pixelOffset.y, imgSize.x, imgSize.y);
  
  Payload pl;
  pl.col = vec4(0, 0, 0, 1);
  trace(r, pl);

  imageStore(backBuffer, storePos, pl.col);
 }