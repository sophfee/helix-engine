interface ILight {
    float3 direction(float3 position);
    float attenuation(float3 position);
    float3 radiance(float3 view, float3 position, float3 normal);
}

struct PointLight : ILight {
    float3 position;
    float3 color;
    float intensity;

    float attenuation(float3 position) {
        float distance = length(this.position - position);
        return 1.0 / (distance * distance);
    }

    float3 direction(float3 position) {
        return normalize(this.position - position);
    }

    float3 radiance(float3 view, float3 position, float3 normal) {
        float3 lightDir = normalize(this.position - position);
        float diff = max(dot(normal, lightDir), 0.0);
        return this.color * diff * this.intensity;
    }
}