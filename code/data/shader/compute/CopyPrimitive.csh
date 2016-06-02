#version 430

#include "PrimitiveCommon.glsl"

uniform int currentPrimitiveCount;
uniform int writeOffset;
uniform mat4 model2World;
uniform mat4 model2WorldInvTransp;

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


layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;
void main() {
  uint primIdx = gl_GlobalInvocationID.x;
  
  if(primIdx >= currentPrimitiveCount) return;
  
  
  uint indexIdxBase = primIdx * 3;  
  Primitive result;
  Vertex verts[3];
  
  for(uint i = 0; i < 3; i++) {
    uint index = indices[indexIdxBase+i];
    Vertex currVert;
    currVert.pos = (model2World * vertices[index]).xyz;
    currVert.u = 0;
    currVert.v = 0;
    currVert.norm = normalize( (model2WorldInvTransp * normals[index]).xyz );
    verts[i] = currVert;
    
  }
  
  result.a = verts[0];
  result.b = verts[1];
  result.c = verts[2];

  primitives[writeOffset + primIdx] = result;
 }