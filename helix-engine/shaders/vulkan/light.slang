interface ILight {
	float attenuation(float3 position);
	float3 normal(float3 position);
	float3 radiance(float3 view, float3 position, float3 normal);
}

struct PointLight : ILight {
	float3 position;
	float intensity;
    float3 color;
    int filler;

	float attenuation(float3 pos) {
		float dist = length(this.position - pos);
		return 1.0 / max(dist * dist, 1e-4); // avoid divide-by-near-zero singularity
	}

	float3 normal(float3 pos) {
		return normalize(this.position - pos);
	}

	float3 radiance(float3 view, float3 pos, float3 normal) {
		// Just the incoming light radiance — no NdotL here,
		// that belongs in the BRDF evaluation.
		return this.color * this.intensity * this.attenuation(pos);
	}
}

struct SpotLight : ILight {
	float3 position;
	float intensity;
	float3 direction;
	float innerCone;
	float3 color;
	float outerCone;

	float attenuation(float3 pos) {
		float dist = length(this.position - pos);
		return 1.0 / max(dist * dist, 1e-4);
	}

	float3 normal(float3 pos) {
		return normalize(this.position - pos);
	}

	float3 radiance(float3 view, float3 pos, float3 normal) {
		float3 lightDir = normalize(this.position - pos);
		float cosTheta = dot(lightDir, normalize(-this.direction));
		float spot = smoothstep(this.outerCone, this.innerCone, cosTheta);
		return this.color * this.intensity * this.attenuation(pos) * spot;
	}
};

struct DirectionalLight : ILight {
	float3 dir;
	float intensity;
	float3 color;

	float attenuation(float3 pos) {
		return 1.0; // directional lights have no attenuation
	}

	float3 normal(float3 pos) {
		return normalize(-this.dir);
	}

	float3 radiance(float3 view, float3 pos, float3 normal) {
		return this.color * this.intensity;
	}
};

struct PointLightData {
	PointLight[] pointLights;
}

struct SpotLightData {
	SpotLight[] spotLights;
}

struct DirectionalLightData {
	DirectionalLight[] directionalLights;
}

struct LightData {
	PointLightData* pointLightData;
	SpotLightData* spotLightData;
	// DirectionalLightData* directionalLightData;
}