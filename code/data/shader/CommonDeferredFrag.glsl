#version 410 core

#pragma import "Utils.glsl"

uniform bool uHasAlbedoMap;
uniform sampler2D uTexture;

uniform bool uHasNormalMap;
uniform sampler2D uNormalMap;

uniform bool uHasSpecularSmoothnessMap;
uniform sampler2D uSpecularSmoothnessMap;

uniform bool uHasEmissiveMap;
uniform sampler2D uEmissiveMap;

uniform vec4 uEmissiveColor;
uniform vec4 uTintColor;
uniform float uTime;

uniform mat4 uModelMatrix;
uniform mat4 uViewProjectionMatrix;
uniform mat4 uInverseModelMatrix;

uniform mat4 uPrevModelMatrix;
uniform mat4 uPrevViewProjectionMatrix;

uniform vec3 cameraPosition;

out vec4 oColor;
out vec4 oEmissive;
out vec4 oNormalMotion;
out vec4 oSpecularSmoothness;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void initShader();
vec4 color();
vec4 emissive();
vec3 normal();
vec4 specularSmoothness();

void main()
{    
    setupInputs();
    initShader();
    vec4 color = color();

    /*float sampl = rand(gl_FragCoord.xy + vec2(mod(uTime,1)));

    if(sampl > color.a) discard;*/
    
    vec4 thisPosition = uViewProjectionMatrix * vec4(inPosition, 1);
    vec4 prevPosition = uPrevViewProjectionMatrix * uPrevModelMatrix *  uInverseModelMatrix * vec4(inPosition, 1);

    vec2 prevFragCoord = prevPosition.xy/prevPosition.w; 
    vec2 thisFragCoord = thisPosition.xy/thisPosition.w;

    oColor = color;
    oEmissive = emissive();
    oSpecularSmoothness = specularSmoothness();
    vec3 n = normal();
    oNormalMotion.xy = vec2(atan(n.y,n.x)/M_PI, n.z);
    oNormalMotion.zw = (thisFragCoord-prevFragCoord)*0.5;
}
