#version 410 core

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;

in vec3 aPosition;
in vec2 aTexCoord;

out vec3 vPosition;
out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    vec4 worldPosition = uModelMatrix * vec4(aPosition, 1.0);
    vPosition = worldPosition.xyz;
    gl_Position = uViewProjectionMatrix * worldPosition;
}
