#pragma import "../Utils.glsl"

uniform sampler2D uSamplerNormalMotion;
uniform sampler2D uSamplerDepth;
uniform sampler2D uSamplerKernel;
uniform sampler2D uSamplerNoise;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uViewProjectionInverseMatrix;
uniform mat4 uViewInverseMatrix;
uniform mat4 uProjectionMatrix;

uniform float uNear;
uniform float uFar;  
uniform float uTime;

uniform float uNoiseScale = 0.002;
uniform int uSampleKernelSize = 16;
uniform float uRadius = 0.05;

in vec2 vTexCoord;

out vec4 oColor;

vec3 unpackWorldPosition(float depth) {
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = vTexCoord * 2.0 - 1.0;
    clipSpaceLocation.z = depth * 2.0 - 1.0;
    clipSpaceLocation.w = 1.0;
    vec4 homogenousLocation = uViewProjectionInverseMatrix * clipSpaceLocation;
    homogenousLocation /= homogenousLocation.w;
    return homogenousLocation.xyz;
}

float linearizeDepth(float depth) {
  return  (2.0 * uNear) / (uFar + uNear - depth * (uFar - uNear));  // convert to linear values
}

vec4 unpackKernel(vec4 kern) {
    return kern * vec4(2, 2, 1, 1) - vec4(1.0, 1.0, 0.0, 0.0);
}

void main() {
	float originDepth = texture(uSamplerDepth, vTexCoord).r;
	vec3 origin = unpackWorldPosition(originDepth);
 
	vec3 normal = unpackNormal(texture(uSamplerNormalMotion, vTexCoord)); 
	normal = normalize( (transpose(uViewInverseMatrix) * vec4(normal, 0)).xyz );
                                    
	vec3 rvec = unpackKernel(texture(uSamplerNoise, vTexCoord * uNoiseScale)).xyz * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);
	float occlusion = 0.0;

	for (int i = 0; i < uSampleKernelSize; ++i) {
		vec3 sample = tbn * unpackKernel(texture(uSamplerKernel, vec2(float(i)/uSampleKernelSize, 0))).xyz;
		sample = sample * uRadius + origin;


		// project sample position:
		vec4 offset = vec4(sample, 1.0);
		offset = uViewProjectionMatrix * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

        //oColor.r = length(offset.xy - vTexCoord) * 10 ; return;

		// get sample depth:
		float sampleDepth = texture(uSamplerDepth, offset.xy).r;
		vec3 samplePos = unpackWorldPosition(sampleDepth);
     

		// range check & accumulate:
		float rangeCheck= length(samplePos-origin) < uRadius ? 1.0 : 0.0;

		occlusion += ( length(origin) >= length(samplePos) ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / uSampleKernelSize);
   
	oColor.r = occlusion;
}
