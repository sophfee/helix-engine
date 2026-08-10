interface ILight {
    float3 direction(float3 position);
    float attenuation(float3 position);
    float3 radiance(float3 view, float3 position, float3 normal);
}

struct PointLight : ILight {
    float3 position;
    float3 color;
    float intensity;

    float attenuation(float3 pos) {
        float dist = length(this.position - pos);
        return 1.0 / max(dist * dist, 1e-4); // avoid divide-by-near-zero singularity
    }

    float3 direction(float3 pos) {
        return normalize(this.position - pos);
    }

    float3 radiance(float3 view, float3 pos, float3 normal) {
        // Just the incoming light radiance — no NdotL here,
        // that belongs in the BRDF evaluation.
        return this.color * this.intensity * this.attenuation(pos);
    }
}