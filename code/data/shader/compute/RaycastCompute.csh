#version 430

layout(binding = 0, rgba8) writeonly uniform image2D backBuffer;

struct Primitive {
  vec3 pos;
  float r;
};

layout(std140, binding = 1) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
} data;

layout(std140, binding = 2) buffer CameraBuffer {
  vec3 pos;
  vec3 forward;
  vec3 right;
  vec3 up;
  float fov;
} cam;

struct Ray {
  vec3 pos;
  vec3 dir;
}; 

Ray generateRay(int x, int y, int w, int h) {
  float fovx = cam.fov;             // Horizontal FOV
  float fovy = fovx * h / w; // Vertical FOV

  float halfWidth =  float(h) / 2.0f;
  float halfHeight = float(w) / 2.0f;

  float alpha = tan(fovx / 2.0f) * ((x - halfWidth) / halfWidth);
  float beta = tan(fovy / 2.0f) * ((halfHeight - y) / halfHeight);

  Ray result;
  result.pos = cam.pos;
  result.dir = normalize(cam.right * alpha + cam.up * beta + cam.forward);
  return result;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

void main() {
  ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
  ivec2 imgSize = imageSize(backBuffer);
  
  if(storePos.x >= imgSize.x || storePos.y >= imgSize.y) return;
  
  Ray r = generateRay(storePos.x, storePos.y, imgSize.x, imgSize.y);
  
 }