#version 410 core

uniform mat4 uModelMatrix;
uniform mat4 uInverseModelMatrix;
uniform mat4 uViewProjectionMatrix;

uniform float uFar;
uniform float uTime;

in vec3 aNormal;
in vec3 aPosition;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vPosition;

vec3 normal();
vec2 texCoord();
vec3 position();

void main()
{
    vNormal = normal();
    vTexCoord = texCoord();
    vPosition = position();
	
	  gl_Position = uViewProjectionMatrix  * vec4(vPosition, 1);
}
