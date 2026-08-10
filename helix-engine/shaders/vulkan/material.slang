#include "common.slang"
#include "light.slang"

interface IMaterial
{
    float4 evaluate<L : ILight>(L light, float3 view, float3 position);
}

namespace detail
{
	float fresnelSchlick(float f0, float f90, float VdotH)
	{
		float x = clamp(1.0 - VdotH, 0.0, 1.0);
		float x2 = x * x;
		float x5 = x * x2 * x2;
		return f0 + (f90 - f0) * x5;
	}

	float fresnelSchlick(float f0, float VdotH)
	{
		float f90 = 1.0; // clamp(50.0 * f0, 0.0, 1.0);
		return fresnelSchlick(f0, f90, VdotH);
	}

	float distributionGGX(float NoH, float roughness)
	{
		float roughnessSq = roughness * roughness;
		float f = (NoH * NoH) * (roughnessSq - 1.0) + 1.0;
		return roughnessSq / (PI * f * f);
	}

	float visibilitySmith(float NdotV, float NdotL, float roughness)
	{
		float v = 0.5 / lerp(2.0 * NdotL * NdotV, NdotL + NdotV, roughness);
		return clamp(v, 0., 1.);
	}
}

struct BRDFMaterial : IMaterial
{
	float3 diffuse;
	float3 normal;
	float3 specular;
	float roughness;
	float metallic;

	float4 evaluate<L : ILight>(L light, float3 view, float3 position)
	{
		float3 lightDir = normalize(light.direction(position));

		float3 viewDir = normalize(-position);
		float3 halfway = normalize(viewDir + lightDir);

		float NdotL = dot(this.normal, lightDir);
		float NdotV = dot(this.normal, viewDir);
		
		if (NdotL < 0 || NdotV < 0) return float4(0.0);

		NdotL = saturate(NdotL);
		NdotV = saturate(NdotV);

		float NdotH = saturate(dot(this.normal, halfway));
		float HdotV = saturate(dot(halfway, viewDir));

		float3 F0 = lerp(float3(0.04, 0.04, 0.04), this.specular, this.metallic);
		
		float3 F = F0 + (1.0 - F0) * detail.fresnelSchlick(F0.x, HdotV);

		float NDF = detail.distributionGGX(NdotH, this.roughness);
		float G = detail.visibilitySmith(NdotV, NdotL, this.roughness);
		float3 numerator = NDF * G * F;
		float denominator = 4.0 * NdotV * NdotL + 0.001;
		float3 specular = numerator / denominator;

		float3 kS = F;
		float3 kD = float3(1.0, 1.0, 1.0) - kS;
		kD *= 1.0 - this.metallic;

		float3 radiance = light.radiance(view, position, this.normal);
        float3 resultColor = (kD * this.diffuse / PI + specular) * radiance * NdotL;

		return float4(resultColor, 1.0);
	}
}