#version 460 core

#define M_PI 3.14159265359

out vec4 FragColor;

in struct VS_OUT {
    vec3 position;
    vec2 texcoord;
} fs_in;

layout(location = 4)  uniform mat4 inverse_projection;
layout(location = 5)  uniform mat4 inverse_view;
layout(location = 6)  uniform mat4 projection;
layout(location = 7)  uniform mat4 view;

const float rayleigh_strength   = 1.0;
const float mie_strength        = 1.0;
const float ozone_strength      = 1.0;
const float atmosphere_density  = 1.0;
const float exposure = 10.0;

layout(location = 13) uniform vec3  light_direction;
const float sun_disc_feather = 0.1;
const float sundisc_intensity = 10.0;

const float EPS               = 1e-6;
const float INFINITY          = 1.0 / 0.0;
const float PLANET_RADIUS = 6371000.0;
const  vec3 PLANET_CENTER = vec3(0.0, -PLANET_RADIUS, 0.0);
const float ATMOSPHERE_HEIGHT = 100000.0;
const float RAYLEIGH_HEIGHT   = (ATMOSPHERE_HEIGHT * 0.08);
const float MIE_HEIGHT        = (ATMOSPHERE_HEIGHT * 0.012);

const vec3 C_RAYLEIGH = vec3(5.802, 13.558, 33.100) * 1e-6;
const vec3 C_MIE      = vec3(3.996,  3.996,  3.996) * 1e-6;
const vec3 C_OZONE    = vec3(0.650,  1.881,  0.085) * 1e-6;

#define saturate(a) clamp(a, 0.0, 1.0)

struct Geometry {
    vec4 color;
    vec3 position;
    vec3 normal;
    vec3 orm;
};

struct Ray {
    vec3 start;
    vec3 dir;
    float len;
};

vec2 sphere_intersection (Ray ray, vec3 sphereCenter, float sphereRadius) {
	ray.start -= sphereCenter;
	float a = dot(ray.dir, ray.dir);
	float b = 2.0 * dot(ray.start, ray.dir);
	float c = dot(ray.start, ray.start) - (sphereRadius * sphereRadius);
	float d = b * b - 4.0 * a * c;
	if (d < 0.0)
	{
		return vec2(-1.0);
	}
	else
	{
		d = sqrt(d);
		return vec2(-b - d, -b + d) / (2.0 * a);
	}
}

vec2 atmosphere_intersection (Ray ray) {
	return sphere_intersection(ray, PLANET_CENTER, PLANET_RADIUS + ATMOSPHERE_HEIGHT);
}

float rayleigh_phase(float cos_theta) {
    return 3.0 * (1.0 + cos_theta * cos_theta) / (16.0 * M_PI);
}

float mie_phase (float cos_theta, float g) {
    g = min(g, 0.9381);
    float k = 1.55*g - 0.55*g*g*g;
    float k_cos_theta = k*cos_theta;
    return (1.0 - k*k) / ((4.0 * M_PI) * (1.0-k_cos_theta) * (1.0-k_cos_theta));
}

float rayleigh_density (float h) {
    return exp(-max(0.0, h / RAYLEIGH_HEIGHT));
}

float mie_density (float h) {
    return exp(-max(0.0, h / MIE_HEIGHT));
}

float ozone_density (float h) {
	return max(0.0, 1.0 - abs(h - 25000.0) / 15000.0);
}

vec3 atmosphere (float h) {
    return vec3(rayleigh_density(h), mie_density(h), ozone_density(h));
}

float atmosphere_height (vec3 position_world_space) {
    return distance(position_world_space, PLANET_CENTER) - PLANET_RADIUS;
}

vec3 integrate_optical_depth (Ray ray) {
	vec2 intersection   = atmosphere_intersection(ray);
	ray.len    = intersection.y;

	int    sampleCount  = 8;
	float  stepSize     = ray.len / float(sampleCount);
	
	vec3 opticalDepth = vec3(0.0);

	for (int i = 0; i < sampleCount; i++) {
		vec3 localPosition = ray.start + ray.dir * (float(i) + 0.5) * stepSize;
		float localHeight  = atmosphere_height(localPosition);
		vec3 localDensity  = atmosphere(localHeight);

		opticalDepth += localDensity * stepSize;
	}

	return opticalDepth;
}

vec3 absorb (vec3 opticalDepth) {
	return exp(-(
		opticalDepth.x * C_RAYLEIGH * rayleigh_strength + 
		opticalDepth.y * C_MIE      * mie_strength * 1.1 + 
		opticalDepth.z * C_OZONE    * ozone_strength
	) * atmosphere_density);
}

vec3 integrate_scattering (Ray ray, vec3 lightDir, vec3 lightColor, out vec3 transmittance)
{
	float  rayHeight = atmosphere_height(ray.start);
	float  sampleDistributionExponent = 1.0 + saturate(1.0 - rayHeight / ATMOSPHERE_HEIGHT) * 8.0; // Slightly arbitrary max exponent of 9

	vec2 intersection = atmosphere_intersection(ray);
	ray.len = min(ray.len, intersection.y);
	if (intersection.x > 0.0)
	{
		ray.start += ray.dir * intersection.x;
		ray.len -= intersection.x;
	}

	float  costh    = dot(ray.dir, lightDir);
	float  phaseR   = rayleigh_phase(costh);
	float  phaseM   = mie_phase(costh, 0.85);

	int    sampleCount  = 64;

	vec3 opticalDepth = vec3(0.0);
	vec3 rayleigh     = vec3(0.0);
	vec3 mie          = vec3(0.0);

	float  prevRayTime  = 0.0;

	for (int i = 0; i < sampleCount; i++)
	{
		float  rayTime = pow(float(i) / float(sampleCount), sampleDistributionExponent) * ray.len;
		float  stepSize = (rayTime - prevRayTime);

		vec3 localPosition = ray.start + ray.dir * rayTime;
		float localHeight  = atmosphere_height(localPosition);
		vec3 localDensity  = atmosphere(localHeight);

		opticalDepth += localDensity * stepSize;

		vec3 viewTransmittance = absorb(opticalDepth);

		vec3 opticalDepthlight  = integrate_optical_depth(Ray(localPosition, lightDir, INFINITY));
		vec3 lightTransmittance = absorb(opticalDepthlight);

		rayleigh += viewTransmittance * lightTransmittance * phaseR * localDensity.x * stepSize;
		mie      += viewTransmittance * lightTransmittance * phaseM * localDensity.y * stepSize;

		prevRayTime = rayTime;
	}

	transmittance = absorb(opticalDepth);

	return (rayleigh * C_RAYLEIGH * rayleigh_strength + mie * C_MIE * mie_strength) * lightColor * exposure;
}

float sun_disc(vec3 eyedir, vec3 sundir, float theta_r) {
	float cos_angle = dot(eyedir, sundir);
	float cos_inner = cos(theta_r * (1.0 - sun_disc_feather));
	float cos_outer = cos(theta_r * (1.0 + sun_disc_feather));
	return smoothstep(cos_outer, cos_inner, cos_angle);
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	mat4 invProjView = inverse(view * projection);
	vec4 direct = (invProjView * vec4(normalize(vec3(gl_FragCoord.xy, -1.0)), 1.0));
    vec3 eyeDir = direct.xyz / direct.w;
    vec3 eyePos = (invProjView * vec4(0.0, 0.0,  0.0, 1.0)).xyz;

    Ray eyeRay = Ray(eyePos, normalize(eyeDir), INFINITY);

    vec3 transmittance;
    vec3 radiance = integrate_scattering(eyeRay, (vec4(light_direction, 1.0)).xyz, vec3(1.0, 0.996, 0.98), transmittance);
    
    float sun_mask = sun_disc(eyeRay.dir, (vec4(light_direction, 1.0)).xyz, 0.05);
	vec3 sundisc = vec3(sun_mask) * transmittance * sundisc_intensity;
	
	FragColor = vec4(radiance + sundisc, 1.0);
}