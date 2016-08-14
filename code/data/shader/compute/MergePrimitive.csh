#version 430

#include "PrimitiveCommon.glsl"

uniform int primitiveCount;
uniform int groupCount;

layout(std140, binding = 0) buffer PrimitiveBuffer { 
	Primitive primitives[]; 
};

void merge(uint start, uint end) {
  for(uint i = start+1; i < end; i++) {
    uint j = i;
    while (j > start && primitives[j-1].sortCode > primitives[j].sortCode) {
        Primitive temp = primitives[j];
        primitives[j] = primitives[j-1];
        primitives[j-1] = temp;
        j--;
    }
  }  
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  uint blockSize = primitiveCount/(8*groupCount);
  uint blockIdx = gl_GlobalInvocationID.x;

  uint primStart = blockIdx * blockSize;
  uint primEnd = min( (blockIdx + 1) * blockSize, primitiveCount);
  merge(primStart, primEnd);
}