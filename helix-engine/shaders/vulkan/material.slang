import common;
import light;

interface IMaterial
{
    float4 evaluate<L : ILight>(L light, float3 view, float3 position);
}

namespace detail
{
    float3 fresnelSchlick(float3 f0, float3 f90, float VdotH)
    {
        float x = clamp(1.0 - VdotH, 0.0, 1.0);
        float x2 = x * x;
        float x5 = x * x2 * x2;
        return f0 + (f90 - f0) * x5;
    }

    float3 fresnelSchlick(float3 f0, float VdotH)
    {
        float3 f90 = float3(1.0, 1.0, 1.0);
        return fresnelSchlick(f0, f90, VdotH);
    }

    float distributionGGX(float NoH, float roughness)
    {
        float roughnessSq = roughness * roughness;
        float f = (NoH * NoH) * (roughnessSq - 1.0) + 1.0;
        return roughnessSq / (PI * f * f);
    }

    // Returns the full visibility term V = G / (4 * NdotL * NdotV),
    // already divided — do NOT divide by 4*NdotL*NdotV again outside this.
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
        float3 lightDir = normalize(light.normal(position));
        float3 viewDir = normalize(-position);
        float3 halfway = normalize(viewDir + lightDir);

        float NdotL = saturate(dot(this.normal, lightDir));
        float NdotV = saturate(dot(this.normal, viewDir));
        float NdotH = saturate(dot(this.normal, halfway));
        float HdotV = saturate(dot(halfway, viewDir));

        // Avoid NDF singularities/fireflies at roughness -> 0
        float roughness = max(this.roughness, 0.045);

        float3 F0 = lerp(float3(0.04, 0.04, 0.04), this.specular, this.metallic);
        float3 F = detail.fresnelSchlick(F0, HdotV);

        float NDF = detail.distributionGGX(NdotH, roughness);
        float V = detail.visibilitySmith(NdotV, NdotL, roughness); // already includes 1/(4*NdotL*NdotV)

        // NOTE: no extra division here — V already folds in the 4*NdotL*NdotV term.
        float3 specularTerm = NDF * V * F;

        float3 kS = F;
        float3 kD = (float3(1.0, 1.0, 1.0) - kS) * (1.0 - this.metallic);

        float3 radiance = light.radiance(view, position, this.normal);
        float3 resultColor = (kD * this.diffuse / PI + specularTerm) * radiance * NdotL;

        return float4(resultColor, 1.0);
	}
}