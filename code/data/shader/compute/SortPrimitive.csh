#version 430

#include "PrimitiveCommon.glsl"

uniform int primitiveCount;

layout(std140, binding = 0) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};


layout(local_size_x = 8, local_size_y = 1, local_size_z = 1) in;
void main() {
  uint primIdx = gl_GlobalInvocationID.x;
  
  if(primIdx >= primitiveCount) return;
  
  Primitive currPrim = primitives[primIdx];
  
  primitives[primIdx] = currPrim;
 }