#version 410 core

#include "Utils.glsl"

uniform float uTime;

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;
uniform mat4 uInverseModelMatrix;

uniform mat4 uPrevModelMatrix;
uniform mat4 uPrevViewProjectionMatrix;


out vec4 oNormalMotion;

void initShader();

vec3 normal();

void main()
{    
    setupInputs();

    vec4 thisPosition = uViewProjectionMatrix * vec4(inPosition, 1);
    vec4 prevPosition = uPrevViewProjectionMatrix * uPrevModelMatrix *  uInverseModelMatrix * vec4(inPosition, 1);

    vec2 prevFragCoord = prevPosition.xy/prevPosition.w; 
    vec2 thisFragCoord = thisPosition.xy/thisPosition.w;

    vec3 n = normal();
    oNormalMotion.xy = vec2(atan(n.y,n.x)/M_PI, n.z);
    oNormalMotion.zw = (thisFragCoord-prevFragCoord)*0.5;
}
