float DotClamped (vec3 a, vec3 b)
{
		return max(0.0, dot(a, b));
}

float BlinnTerm (vec3 normal, vec3 halfDir)
{
	return DotClamped (normal, halfDir);
}

float Pow5(float v) {
  return v * v * v * v * v;
}

vec3 FresnelTerm (vec3 F0, float cosA)
{
	float t = Pow5(1.0 - cosA);	// ala Schlick interpoliation
	return F0 + (1-F0) * t;
}

vec3 FresnelLerp (vec3 F0, vec3 F90, float cosA)
{
	float t = Pow5(1 - cosA);	// ala Schlick interpoliation
	return mix (F0, F90, t);
}

float SmithJointGGXVisibilityTerm (float NdotL, float NdotV, float roughness)
{
    // This is an approximation
	float a = roughness * roughness;
	float gV = NdotL * (NdotV * (1 - a) + a);
	float gL = NdotV * (NdotL * (1 - a) + a);
	return (2.0 * NdotL) / (gV + gL + 1e-5); // This function is not intended to be running on Mobile,
	// therefore epsilon is smaller than can be represented by float
}

float GGXTerm (float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = NdotH * NdotH * (a2 - 1) + 1;
	return a2 / (3.141592 * d * d);
}

vec3 SafeNormalize(vec3 inVec)
{
	float dp3 = max(0.001f, dot(inVec, inVec));
	return inVec / sqrt(dp3);
}

vec4 BRDF(vec3 diffColor, vec3 specColor, float oneMinusReflectivity, float oneMinusRoughness,
	vec3 normal, vec3 viewDir, vec3 lightDir, float lightNdotL, vec3 lightColor)
{

	float roughness = 1.0-oneMinusRoughness;
	vec3 halfDir = SafeNormalize(lightDir + viewDir);

	float nl = lightNdotL;
	float nh = BlinnTerm (normal, halfDir);
	float nv = DotClamped (normal, viewDir);
	float lv = DotClamped (lightDir, viewDir);
	float lh = DotClamped (lightDir, halfDir);

	float V = SmithJointGGXVisibilityTerm (nl, nv, roughness);
	float D = GGXTerm (nh, roughness);
  
	float nlPow5 = Pow5(1-nl);
	float nvPow5 = Pow5(1-nv);
	float Fd90 = 0.5 + 2 * lh * lh * roughness;
	float disneyDiffuse = (1 + (Fd90-1) * nlPow5) * (1 + (Fd90-1) * nvPow5);
	
	float specularTerm = (V * D) * (3.141592/4); // Torrance-Sparrow model, Fresnel is applied later (for optimization reasons)

	specularTerm = max(0, specularTerm * nl);

	float diffuseTerm = disneyDiffuse * nl;
	float grazingTerm = clamp(oneMinusRoughness + (1-oneMinusReflectivity), 0, 1);
  vec3 color =	diffColor * (lightColor * diffuseTerm)
                    + specularTerm * lightColor * FresnelTerm (specColor, lh) * FresnelLerp (specColor, vec3(grazingTerm), nv);
                    
	return vec4(color, 1);
}

float SpecularStrength(vec3 specular)
{
		return max (max (specular.r, specular.g), specular.b);
}

// Diffuse/Spec Energy conservation
vec3 EnergyConservationBetweenDiffuseAndSpecular (vec3 albedo, vec3 specColor, out float oneMinusReflectivity)
{
	oneMinusReflectivity = 1 - SpecularStrength(specColor);
	return albedo * (vec3(1,1,1) - specColor);
}