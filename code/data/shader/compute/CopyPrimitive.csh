#version 430

#include "PrimitiveCommon.glsl"

uniform int materialId;
uniform int currentPrimitiveCount;
uniform int writeOffset;
uniform mat4 model2World;
uniform mat4 model2WorldInvTransp;

uniform vec3 sceneMin;
uniform vec3 sceneMax;


layout(std430, binding = 0) buffer VertexBuffer { 
	vec4 vertices[]; 
};

layout(std430, binding = 1) buffer NormalBuffer { 
	vec4 normals[]; 
};

layout(std430, binding = 2) buffer IndexBuffer {
  uint indices[];
};

layout(std140, binding = 3) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};


uint Part1By2(uint x) {
  x &= 0x000003ff;                  // x = ---- ---- ---- ---- ---- --98 7654 3210
  x = (x ^ (x << 16)) & 0xff0000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
  x = (x ^ (x <<  8)) & 0x0300f00f; // x = ---- --98 ---- ---- 7654 ---- ---- 3210
  x = (x ^ (x <<  4)) & 0x030c30c3; // x = ---- --98 ---- 76-- --54 ---- 32-- --10
  x = (x ^ (x <<  2)) & 0x09249249; // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
  return x;
}

uint EncodeMorton3(uvec3 coords) {
  return (Part1By2(coords.z) << 2) + (Part1By2(coords.y) << 1) + Part1By2(coords.x);
}

uvec3 getIntCoords(vec3 pos) {
  vec3 scaled = (pos - sceneMin)/(sceneMax - sceneMin);
  return uvec3(scaled * vec3(intBitsToFloat(0x7f7fffff)));
}


layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;
void main() {
  uint primIdx = gl_GlobalInvocationID.x;
  
  if(primIdx >= currentPrimitiveCount) return;
  
  uint indexIdxBase = primIdx * 3;  
  Primitive result;
  Vertex verts[3];
  
  vec3 center = vec3(0);
  
  for(uint i = 0; i < 3; i++) {
    uint index = indices[indexIdxBase+i];
    Vertex currVert;
    currVert.pos = (model2World * vec4(vertices[index].xyz, 1)).xyz;
    currVert.u = 0;
    currVert.v = 0;
    currVert.norm = normalize( (model2WorldInvTransp * vec4(normals[index].xyz, 0)).xyz );
    verts[i] = currVert;
    center += verts[i].pos;
  }
  
  center *= 1.0/3.0;
  
  uvec3 coords = getIntCoords(center);
  
  result.a = verts[0];
  result.b = verts[1];
  result.c = verts[2];
  result.matId = materialId;
  result.sortCode = EncodeMorton3(coords);
  
  primitives[writeOffset + primIdx] = result;
 }