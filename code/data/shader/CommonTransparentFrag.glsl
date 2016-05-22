#version 410 core

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vPosition;

uniform sampler2D uSamplerDepth;

uniform bool uHasAlbedoMap;
uniform sampler2D uTexture;

uniform bool uHasNormalMap;
uniform sampler2D uNormalMap;

uniform bool uHasSpecularSmoothnessMap;
uniform sampler3D uSpecularSmoothnessMap;

uniform vec4 uEmissiveColor;
uniform vec4 uTintColor;
uniform float uTime;
uniform vec2 uOneOverScreenSize;

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;

uniform mat4 uPrevModelMatrix;
uniform mat4 uPrevViewProjectionMatrix;

uniform vec3 uCameraPosition;
uniform mat4 uViewProjectionInverseMatrix;
uniform vec3 uCameraForward;

out vec4 oColor;

vec4 color();
vec4 emissive();
vec3 normal();

void main()
{    
	vec4 color = color();
	oColor = color;
}
