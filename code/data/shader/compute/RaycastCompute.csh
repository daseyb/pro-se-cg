#version 430

#define PI 3.141592683
#define DEG_TO_RAD PI/180.0

layout(rgba32f, binding = 0) writeonly uniform image2D backBuffer;

struct Primitive {
  vec3 pos;
  float r;
};

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

Ray generateRay(int x, int y, int w, int h) {
  float fovx = cam.fov * DEG_TO_RAD;                     // Horizontal FOV
  float fovy = fovx * float(h) / float(w);  // Vertical FOV

  float halfWidth =  float(w) / 2.0f;
  float halfHeight = float(h) / 2.0f;

  float alpha = tan(fovx / 2.0f) * ((float(x) - halfWidth) / halfWidth);
  float beta = tan(fovy / 2.0f) * ((halfHeight - float(y)) / halfHeight);
  
  Ray result;
  result.pos = cam.pos;
  result.dir = normalize(cam.right.xyz * alpha + cam.up.xyz * beta + cam.forward.xyz);
  return result;
}


bool intersectPrimitive(in Ray ray, in Primitive sphere, out float t) {
    t = 100000.0;
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
        t = t1;
    } else {
        t = t0; 
    }
    
    return true;
}


bool findIntersection(in Ray r, out float t) {
  bool didIntersect = false;
  t = 100000.0;
  for(int i = 0; i < primitiveCount; i++) {
    Primitive p = primitives[i];
    float currT = 100000.0;

    if(intersectPrimitive(r, p, currT)) {
      didIntersect = true;
      t = min(currT, t);
    }
  }
  return didIntersect;
}


layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(storePos.x, storePos.y, imgSize.x, imgSize.y);
  
  vec4 col = vec4(0, 0, 0, 1);
  
  float t = 100000.0;
  if(findIntersection(r, t)) {
    col = vec4(vec3(1.0), 1);
  }
  
  imageStore(backBuffer, storePos, col);
 }