#version 460 core

#define PI 3.14159265359
#define M_PI PI

#define M_TAU 6.28318530718
#define TAU M_TAU

layout ( location = 0 ) 
    out vec4 FragColor;

in InOutData {
    vec3 position;
    vec2 texcoord;
} fs_in;


layout(binding = 0)  uniform  sampler2D texAlbedo;
layout(binding = 1)  uniform  sampler2D texPosition;
layout(binding = 2)  uniform  sampler2D texNormal;
layout(binding = 3)  uniform  sampler2D texOrm;
layout(binding = 4) uniform usampler2D texObjectId;
layout(binding = 5) uniform sampler2D texEmissive;

layout(binding = 6) uniform sampler3D texVoxel;

layout(location = 4)  uniform mat4 inverseProjection;
layout(location = 5)  uniform mat4 inverseView;
layout(location = 6)  uniform mat4 projection;
layout(location = 7)  uniform mat4 view;

layout(location = 11) uniform vec3 point_light;

layout(location = 16) uniform mat4      lightViewMatrix;
layout(location = 17) uniform mat4      lightProjectionMatrix;
layout(location = 18) uniform vec2      lightClippingPlanes;
layout(location = 19) uniform sampler2D lightDepthTexture;


// vec2 in range [0.0, 1.0] ->
// vec3 in range [-1.0, 1.0] with length=1
// This is useful for getting the normal from a normal map image
vec3 reconstruct_normal(vec2 v)
{
    vec3 result;
    result.xy = v * 2.0 - 1.0;
    result.z = sqrt(max(1.0 - dot(v.rg, v.rg), 0.0));
    return result;
}

const uint  g_sss_max_steps        = 1;     // Max ray steps, affects quality and performance.
const float g_sss_ray_max_distance = 0.05;  // Max shadow length, longer shadows are less accurate.
const float g_sss_thickness        = 0.01;  // Depth testing thickness.
const float g_sss_step_length      = 0.05 / float(g_sss_max_steps);

struct OmniLight {
    vec3 position;
    float intensity;
    vec3 color;
    float range;
};

layout (std430, binding = 4) buffer OmniLightBuffer {
    OmniLight data[];
} omniLights;

layout (std430, binding = 0) restrict buffer VoxelizerSSBO
{
    vec3 gridMin;
    float _pad0;
    vec3 gridMax;
    float _pad1;
} u_voxelizerData;

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};


struct Box
{
    vec3 Min;
    vec3 Max;
};

vec4 getVoxel(ivec3 mapPos) {
    mapPos.y = 512 - mapPos.y;
    return texelFetch(texVoxel, mapPos, 0);
}

#define MAX_RAY_STEPS 512

vec4 traverseVoxels(vec3 rayOrig, vec3 rayDir) {
    ivec3 mapPos = ivec3(floor(rayOrig));
    vec3 deltaDist = abs(vec3(length(rayDir)) / rayDir);
    ivec3 rayStep = ivec3(sign(rayDir));
    vec3 sideDist = (sign(rayDir) * (vec3(mapPos) - rayOrig) + (sign(rayDir) * 0.5) + 0.5) * deltaDist;

    for (int i = 0; i < MAX_RAY_STEPS; i++) {
        // Visit/Check the current voxel (mapPos)
        vec4 vox = getVoxel(mapPos);
        if (vox.a > 0.0) return vox;

        // Determine next step
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                sideDist.x += deltaDist.x;
                mapPos.x += rayStep.x;
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
            }
        } else {
            if (sideDist.y < sideDist.z) {
                sideDist.y += deltaDist.y;
                mapPos.y += rayStep.y;
            } else {
                sideDist.z += deltaDist.z;
                mapPos.z += rayStep.z;
            }
        }
    }
    return vec4(0.0);
}

vec3 GetWorldSpaceDirection(mat4 inverseProj, mat4 inverseView, vec2 ndc)
{
    vec4 rayView;
    rayView.xy = mat2(inverseProj) * ndc;
    rayView.z = -1.0;
    rayView.w = 0.0;

    vec3 rayWorld = normalize((inverseView * rayView).xyz);
    return rayWorld;
}
bool RayBoxIntersect(Ray ray, Box box, out float t1, out float t2)
{
    // Source: https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525

    vec3 invDir = 1.0 / ray.Direction;
    vec3 t0s = (box.Min - ray.Origin) * invDir;
    vec3 t1s = (box.Max - ray.Origin) * invDir;

    vec3 tsmaller = min(t0s, t1s);
    vec3 tbigger = max(t0s, t1s);

    t1 = max(tsmaller.x, max(tsmaller.y, max(tsmaller.z, 0.0)));
    t2 = min(tbigger.x, min(tbigger.y, tbigger.z));

    return t1 <= t2;
}

vec3 position_at(vec2 uv) {
    return texture(texPosition, uv).xyz;
}

bool in_bounds(vec2 uv) {
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

float interleaved_gradient_noise(vec2 position_screen)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * fract(dot(position_screen, magic.xy)));
}

float ScreenSpaceShadows(vec2 uv, vec3 light_world_space)
{
    vec3 light_pos = (view * vec4(light_world_space, 1.0)).xyz;
    vec3 ray_pos = position_at(uv);
    vec3 start_pos = ray_pos;
    vec3 ray_dir = normalize(light_pos - ray_pos); // In view space!

    vec3 ray_step = ray_dir * g_sss_step_length;
    
    ivec2 resolution = textureSize(texPosition, 0);
    
    ivec2 lastSample = ivec2(floor(uv * vec2(resolution)));

    float occlusion = 0.0;
    vec2 ray_uv     = vec2(0.0);
    uint i = 0;
    while (i < g_sss_max_steps) // 1024 = failsafe 
    {
        // Step the ray
        float offset = interleaved_gradient_noise(vec2(resolution) * uv) * 2.0f - 1.0f;
        ray_pos += ray_step;// * ((offset * 0.0002));
        vec4 projected = (projection * vec4(ray_pos, 1.0));
        projected.xy /= projected.w;
        ray_uv  = projected.xy * .5 + .5;
        
        ivec2 texel = ivec2(floor(ray_uv * vec2(resolution)));
        
        if (texel == lastSample) {
            continue;
        }
        
        i++;
        lastSample = texel;

        // Ensure the UV coordinates are inside the screen
        if (in_bounds(ray_uv))
        {
            // Compute the difference between the ray's and the camera's depth
            float depth_z     = position_at(ray_uv).z;
            float depth_delta = ray_pos.z - depth_z;

            // Check if the camera can't "see" the ray (ray depth must be larger than the camera depth, so positive depth_delta)
            if (((depth_delta) < 0.0f) && (depth_delta < g_sss_thickness))
            {
                // Mark as occluded
                vec3 normal = texture(texNormal, ray_uv).xyz;
                
                occlusion = 1.0;//max(dot(normalize(ray_pos - start_pos), normal), 0.0);

                // Fade out as we approach the edges of the screen
                //occlusion *= screen_fade(ray_uv);

                break;
            }
            //occlusion = max(occlusion, abs(depth_delta));
        }
    }

    // Convert to visibility
    return 1.0 - occlusion;
}

vec3 fresnelSchlick(in float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float SchlickFresnel(float u) {
    float m = float(1.0) - u;
    float m2 = m * m;
    return m2 * m2 * m;// pow(m,5)
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float D_GGX(float NoH, float roughness, vec3 n, vec3 h) {
    float roughnessSq = roughness * roughness;
    float f = (NoH * NoH) * (roughnessSq - 1.0) + 1.0;
    return roughnessSq / (PI * f * f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float V_GGX(float NdotL, float NdotV, float alpha) {
    float v = 0.5 / mix(2.0 * NdotL * NdotV, NdotL + NdotV, alpha);
    return clamp(v, 0., 1.);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx  = V_GGX(NdotL, NdotV, roughness);

    return ggx;
}

float saturate(float x) { return clamp(x, 0.0, 1.0); }

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md#range-property
float getRangeAttenuation(float range, float distance)
{
    if (range <= 0.0)
    {
        // negative range means unlimited
        return 1.0 / pow(distance, 2.0);
    }
    return max(min(1.0 - pow(distance / range, 4.0), 1.0), 0.0) / pow(distance, 2.0);
}


vec3 omniLight(
    in vec3 lightPos,
    in float range,
    in vec4 lightColEnergy,
    in vec3 viewPos,
    in vec3 fragPos,
    in vec3 normal,
    in vec3 albedo,
    in vec2 metalRough,
    in vec3 V,
    in vec3 L
) {
    vec3 H = normalize(L + V);
    vec3 N = normalize(normal);

    float NdL = saturate(dot(N, L));
    float NdH = saturate(dot(N, H));

    float dist        = distance(lightPos, fragPos);
    float attenuation = getRangeAttenuation(range, dist);
    vec3  radiance    = lightColEnergy.rgb * attenuation; // removed cosTheta, applied once at the end

    float metallic  = metalRough.x;
    float roughness = metalRough.y;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = F0 + (1.0 - F0) * SchlickFresnel(max(dot(H, V), 0.0));

    float NDF = D_GGX(clamp(NdH, 0., 1.), roughness, N, H);
    float G   = GeometrySmith(N, V, L, roughness);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.01) * max(NdL, 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    return (kD * albedo / PI + specular) * radiance * NdL;
}

vec3 orthoLight(
    in vec4 lightColEnergy,
    in vec3 viewPos,
    in vec3 fragPos,
    in vec3 normal,
    in vec3 albedo,
    in vec2 metalRough,
    in vec3 V,
    in vec3 L
) {
    vec3 H = normalize(L + V);
    vec3 N = normalize(normal);

    float NdL = dot(N, L);
    float NdH = max(dot(N, H), 0.0);

    float cosTheta    = max(NdL, 0.0);
    float attenuation = 1.0;
    vec3  radiance    = lightColEnergy.rgb * attenuation * cosTheta;

    float metallic  = metalRough.x;
    float roughness = metalRough.y;

    vec3 F0 = vec3(0.04);
         F0 = mix(F0, albedo, metallic);
    vec3 F  = vec3(SchlickFresnel(max(dot(H, V), 0.0))) * F0;
    
    float NDF =  D_GGX(clamp(NdH, 0., 1.), roughness, N, H);
    float G   = GeometrySmith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(NdL, 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    return (kD * albedo / PI + specular) * radiance * NdL;
}

const vec3 light_pos = vec3(-21.7048587799072, 43.414340972900390, -6.149883747100830);

float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}
#define float2(x, y) vec2(x, y)
#define PCF_FILTER_SAMPLES 16
vec2 poissonDisk[16] = vec2[16](
	float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790)
);

#define PCF_STEPS 1

float sampleShadow(in vec3 position, in vec3 normal, in vec3 light, in sampler2D depth, in vec2 clip, in mat4 view, in mat4 proj, in float NdotL, out vec2 uv) {
    vec4 ndc = proj * view * (vec4(position, 1.0));
    ndc.xyz /= ndc.w;
    float len = length(ndc.xyz);
    ndc.xyz  = ndc.xyz * .5 + .5;
    uv = ndc.xy;
    
    float shadow = 0.0;
    if (!in_bounds(uv)) return 1.0;
    
    float bias = 0.005; // max(0.005 * (1.0 - NdotL), 0.0005);
    vec2 texelSize = 1.0 / textureSize(depth, 0);

    if (ndc.z < 0.0 || ndc.z > 1.0) return 1.0;

    const int steps = 1;

#if PCF_STEPS == 1
    float pcfDepth = texture(depth, ndc.xy).r;// + ((texelSize*1.2)* fwidthFine(vec2(x + a1, y + a2)))).r; 

    if ((ndc.z - bias) > pcfDepth) {
        shadow += 1.0;// * dist;
    }
#else

    for(int x = -steps; x <= steps; ++x)
    {
        for(int y = -steps; y <= steps; ++y)
        {
            // float a1 = random(ndc.xy-(vec2(x,y)/vec2(1024.0)));
            // float a2 = random(ndc.xy+(vec2(x,y)/vec2(1024.0)));
            
        }
    }
#endif
   
    return 1.0 - (shadow);
}


layout(std430, binding = 2) buffer LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
	float cascadePlaneDistances[16];
};

layout (binding = 7) uniform sampler2DArrayShadow csmTexture;
layout (location = 22) uniform vec3 lightDir;
layout (location = 23) uniform float farPlane;
layout (location = 24) uniform vec3 lightPos;
layout (location = 25) uniform int mode;

const int cascadeCount = 3;
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// Interleaved Gradient Noise
// https://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float quick_hash(vec2 pos) {
	const vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
	return fract(magic.z * fract(dot(pos, magic.xy)));
}


float CSM_AverageBlockDepth(int layer, vec3 projCoords, float w_light, float bias) {
	int sampleCount = 16;
	float blockerSum = 0;
	int blockerCount = 0;

	vec2 texelSize = vec2(1.0) / vec2(textureSize(csmTexture, 0));

	float closestDepth = texture(csmTexture, vec4(projCoords.xy, layer, projCoords.z - bias));
	float currentDepth = projCoords.z;
	//return currentDepth;
	float search_range = w_light * (currentDepth - 0.05) / currentDepth;
	//return search_range;
	if (search_range <= 0) {
		return 0.0;
	}

	int range = int(search_range);
	//return range / 10.0;
	for (int i = 0; i < PCF_FILTER_SAMPLES; i++) {
        
        float sampleDepth = texture(csmTexture, vec4(projCoords.xy + poissonDisk[i] * (search_range * texelSize), layer, projCoords.z - bias));

		if (sampleDepth > 0.0) {
			blockerSum += sampleDepth;
			blockerCount++;
		}
	}


	if (blockerCount > 0) {
		return blockerSum / blockerCount;
	}
	else {
		return 0; //--> not in shadow~~~~
	}
}

//http://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf
float CSM_PenumbraWidth(float d_receiver, float d_blocker, float w_light) {
	return (d_receiver - d_blocker) * w_light / d_blocker;
}

float CSM_Main(vec3 fragPosViewSpace, vec3 normal, out int layer)
{
    // select cascade layer
    vec4 fragPosWorldSpace = inverseView * vec4(fragPosViewSpace, 1.0);

    float depthValue = abs(fragPosViewSpace.z);

    layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue <= cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    // layer = 2;

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace.xyz, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    float w_light = 0.05; // / float(layer + 1);

    float averageBlockerDepth = CSM_AverageBlockDepth(
        layer,
        projCoords,
        w_light,
        0.05
    );

    if (averageBlockerDepth <= 0.0) {
        return 0.0;
    }

    float penumbraWidth = CSM_PenumbraWidth(
        projCoords.z,
        averageBlockerDepth,
        w_light
    );

#define LIGHT_WORLD_SIZE .5
#define LIGHT_FRUSTUM_WIDTH (cascadePlaneDistances[layer] / farPlane)
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

    float filterRadius = max(((penumbraWidth * LIGHT_SIZE_UV * 0.0005) / currentDepth), 0.0001);// * max((cascadePlaneDistances[max(layer-1,0)] / farPlane), 0.01);

    mat2 disk_rotation;
    {
        float r = quick_hash(gl_FragCoord.xy) * TAU;
        float sr = sin(r);
        float cr = cos(r);
        disk_rotation = mat2(cr, -sr, sr, cr);
    }

    // calculate bias (based on depth map resolution and slope)
    vec3 lightPosView = (view * vec4(lightPos, 1.0)).xyz;
    
    // float bias = 0.005 * tan(acos(clamp(dot(normal, normalize(lightPosView - fragPosViewSpace)), 0.0, 1.0)));
    float bias = max(0.05 * (1.0 - dot(normalize(lightPosView - fragPosViewSpace), normalize(normal))), 0.05);// * float(layer + 1); // * max((cascadePlaneDistances[layer-1] / farPlane), 0.001);
    const float biasModifier = 0.5;
    if (layer == cascadeCount)
    {
    }
    else
    {
        // bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }
        bias *= 1 / (farPlane * biasModifier);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(csmTexture, 0));
    for(int x = 0; x < PCF_FILTER_SAMPLES; ++x) {
        float pcfDepth = texture(csmTexture, vec4(projCoords.xy + (poissonDisk[x] * filterRadius * disk_rotation), layer, currentDepth - bias));
        shadow += pcfDepth;
    }
    shadow /= float(PCF_FILTER_SAMPLES);
    
    return shadow;
}

const mat3 SRGB_2_XYZ_MAT = mat3(
    0.4124564, 0.3575761, 0.1804375,
    0.2126729, 0.7151522, 0.0721750,
    0.0193339, 0.1191920, 0.9503041);
const float SRGB_ALPHA = 0.055;

float luminance(vec3 color) {
    vec3 luminanceCoefficients = SRGB_2_XYZ_MAT[1];
    return dot(color, luminanceCoefficients);
}

const mat3 agxTransform = mat3(
    0.842479062253094, 0.0423282422610123, 0.0423756549057051,
    0.0784335999999992, 0.878468636469772, 0.0784336,
    0.0792237451477643, 0.0791661274605434, 0.879142973793104);

const mat3 agxTransformInverse = mat3(
    1.19687900512017, -0.0528968517574562, -0.0529716355144438,
    -0.0980208811401368, 1.15190312990417, -0.0980434501171241,
    -0.0990297440797205, -0.0989611768448433, 1.15107367264116);

vec3 agxDefaultContrastApproximation(vec3 x) {
    vec3 x2 = x * x;
    vec3 x4 = x2 * x2;
    return +15.5 * x4 * x2 - 40.14 * x4 * x + 31.96 * x4 - 6.868 * x2 * x + 0.4298 * x2 + 0.1191 * x - 0.00232;
}

void agx(inout vec3 color) {
    const float minEv = -12.47393;
    const float maxEv = 4.026069;
    color = agxTransform * color;
    color = clamp(log2(color), minEv, maxEv);
    color = (color - minEv) / (maxEv - minEv);
    color = agxDefaultContrastApproximation(color);
}

void agxEotf(inout vec3 color) {
    color = agxTransformInverse * color;
}

void agxLook(inout vec3 color) {
    // Punchy
    const vec3 slope = vec3(1.1);
    const vec3 power = vec3(1.2);
    const float saturation = 1.3;
    float luma = luminance(color);
    color = pow(color * slope, power);
    color = max(luma + saturation * (color - luma), vec3(0.0));
}

float n = 4.;
mat4 colors = mat4(0.,8.,2.,10.,
                   12.,4.,14.,6.,
                   3.,11.,1.,9.,
                   15.,7.,13.,5.) * (1./16.);
float nearestColor(float color){
    return round(color * n)/n;
}

float dither(float color, float range, float x, float y){
    return nearestColor(color + range * (colors[int(mod(x,n))][int(mod(y,n))]-.5));
}

float Bayer2(vec2 a) {
    a = floor(a);
    return fract(a.x / 2. + a.y * a.y * .75);
}

#define Bayer4(a)   (Bayer2 (.5 *(a)) * .25 + Bayer2(a))
#define Bayer8(a)   (Bayer4 (.5 *(a)) * .25 + Bayer2(a))
#define Bayer16(a)  (Bayer8 (.5 *(a)) * .25 + Bayer2(a))
#define Bayer32(a)  (Bayer16(.5 *(a)) * .25 + Bayer2(a))
#define Bayer64(a)  (Bayer32(.5 *(a)) * .25 + Bayer2(a))
float Remap(float value, float valueMin, float valueMax, float mapMin, float mapMax)
{
    return (value - valueMin) / (valueMax - valueMin) * (mapMax - mapMin) + mapMin;
}

vec3 Remap(vec3 value, vec3 valueMin, vec3 valueMax, vec3 mapMin, vec3 mapMax)
{
    return (value - valueMin) / (valueMax - valueMin) * (mapMax - mapMin) + mapMin;
}

vec3 MapToZeroOne(vec3 value, vec3 rangeMin, vec3 rangeMax)
{
    return Remap(value, rangeMin, rangeMax, vec3(0.0), vec3(1.0));
}

ivec3 WorlSpaceToVoxelImageSpace(vec3 worldPos)
{
    vec3 uvw = MapToZeroOne(worldPos, u_voxelizerData.gridMin, u_voxelizerData.gridMax);
    ivec3 voxelPos = ivec3(floor(uvw * vec3(384.0)));
    voxelPos.y = 383 - voxelPos.y; // Flip Y axis since image coordinate system has (0,0) at top left
    return voxelPos;
}

float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

vec2 remap(vec2 value, vec2 inMin, vec2 inMax, vec2 outMin, vec2 outMax) {
    return vec2(remap(value.x, inMin.x, inMax.x, outMin.x, outMax.x), remap(value.y, inMin.y, inMax.y, outMin.y, outMax.y));
}

vec3 remap(vec3 value, vec3 inMin, vec3 inMax, vec3 outMin, vec3 outMax) {
    return vec3(
        remap(value.x, inMin.x, inMax.x, outMin.x, outMax.x),
        remap(value.y, inMin.y, inMax.y, outMin.y, outMax.y),
        remap(value.z, inMin.z, inMax.z, outMin.z, outMax.z)
    );
}

vec4 TraceCone(Ray ray, vec3 normal, float coneAngle, float stepMultiplier, float normalRayOffset, float alphaThreshold)
{
    vec3 voxelGridWorldSpaceSize = u_voxelizerData.gridMax - u_voxelizerData.gridMin;
    vec3 voxelWorldSpaceSize = voxelGridWorldSpaceSize / vec3(textureSize(texVoxel, 0));
    float voxelMaxLength = max(voxelWorldSpaceSize.x, max(voxelWorldSpaceSize.y, voxelWorldSpaceSize.z));
    float voxelMinLength = min(voxelWorldSpaceSize.x, min(voxelWorldSpaceSize.y, voxelWorldSpaceSize.z));
    uint maxLevel = textureQueryLevels(texVoxel) - 1;
    vec4 accumulatedColor = vec4(0.0);

    ray.Origin += normal * voxelMaxLength * normalRayOffset;

    float distFromStart = voxelMaxLength;
    while (accumulatedColor.a < alphaThreshold)
    {
        float coneDiameter = 2.0 * tan(coneAngle) * distFromStart;
        float sampleDiameter = max(voxelMinLength, coneDiameter);
        float sampleLod = log2(sampleDiameter / voxelMinLength);

        vec3 worldPos = ray.Origin + ray.Direction * distFromStart;
        vec3 sampleUVW = MapToZeroOne(worldPos, u_voxelizerData.gridMin, u_voxelizerData.gridMax);
        if (any(lessThan(sampleUVW, vec3(0.0))) || any(greaterThanEqual(sampleUVW, vec3(1.0))) || sampleLod > maxLevel)
        {
            break;
        }

        vec4 samplePremult = textureLod(texVoxel, sampleUVW, sampleLod);

        float weightOfSample = 1.0 - accumulatedColor.a;
        accumulatedColor += weightOfSample * samplePremult;

        distFromStart += sampleDiameter * stepMultiplier;
    }

    return accumulatedColor;
}

vec4 TraceCone(Ray ray, float coneAngle, float stepMultiplier)
{
    const vec3 normal = vec3(0.0);
    const float normalRayOffset = 0.0;
    return TraceCone(ray, normal, coneAngle, stepMultiplier, normalRayOffset, 1.0);
}
    
const int SSR_MAX_STEPS = 512;
const float SSR_STEP_MULTIPLIER = 0.75;
const float SSR_MAX_DISTANCE = 4.0;
const float SSR_THICKNESS = 0.1;

vec3 SSR(vec3 P, vec3 N, vec3 V) {
    vec3 result = vec3(0.0);
    vec3 reflectDir = reflect(-normalize(V), normalize(N));
    
    vec3 traceStart = P;
    vec3 traceEnd = P + reflectDir * SSR_MAX_DISTANCE;
    
    
    #pragma unroll
    for (int i = 0; i < SSR_MAX_STEPS; ++i) {
        float t = float(i) / float(SSR_MAX_STEPS);
        vec3 samplePoint = mix(traceStart, traceEnd, t);
        vec4 sampleUVW = (projection * vec4(samplePoint, 1.0));
        sampleUVW.xyz /= sampleUVW.w;
        vec2 sampleUV = (sampleUVW.xy * 0.5 + 0.5);// / sampleUVW.w;
        
        if (!in_bounds(sampleUV.xy)) {
            break;
        }
        
        vec3 samplePos = texture(texPosition, sampleUV.xy).xyz;
        if (samplePoint.z < (samplePos.z-0.01)) {
            return texture(texAlbedo, sampleUV.xy).rgb;
        }
        
        if (distance(samplePoint, P) > SSR_MAX_DISTANCE) {
            break;
        }
    }
    
    return result;
}

void main() {
    vec2 uv = fs_in.position.xy * .5 + .5;
    
    vec4 albedo   = texture( texAlbedo,   uv );
    vec4 position = texture( texPosition, uv );
    vec4 normal   = texture( texNormal,   uv );
    vec4 orm      = texture( texOrm,      uv );
    uint objectId = texture( texObjectId, uv ).r;
    
    vec3 light = vec3(0.0);

    vec3 worldPos  = (vec4(position.xyz, 1.0)).xyz;
    vec3 worldNorm = normalize(normal.xyz);

    
    vec3 V = normalize( -position.xyz );
    
    float shaded = 0.0;
    
    // Camera light
    
    int u;
    for (u = 0; u < 64; ++u) {
        OmniLight ol = omniLights.data[u+1];
        //ol.position -= vec3(0.0, 2.0, 0.0);
        //ol.position = vec3(-5. + (float(u) * 7.0), 5.0+ sin((float(u)/2.0) * PI), 0.0);
        vec3 omniLightPosition = (view * vec4(ol.position, 1.0)).xyz;
        vec3 L = normalize(omniLightPosition - worldPos);
        
        float dist = distance(omniLightPosition, worldPos);
        
        vec3 lightValue = omniLight(
            omniLightPosition,
            ol.range * 4.,
            vec4(ol.color * 4.0, 1.0),
            vec3(0.0),
            worldPos,
            worldNorm,
            albedo.rgb,
            orm.gb,
            V, L
        );
        
        if (max(lightValue.x, max(lightValue.y, lightValue.z)) > 0.01) {
            light += lightValue * ScreenSpaceShadows(uv, ol.position);
        } else {
            light += lightValue;
        }
        
        ///shaded += 1.0 - ;
        // light = vec3(L);
        //light += ol.color * ol.intensity * getRangeAttenuation(ol.range, dist);
        // shaded = saturate(dot(worldNorm, normalize(V + L)));
    }
    
    // shaded /= 32.0;

    int layer;
    float shadow = smoothstep(0.0, 1.0, CSM_Main(position.xyz, normalize(normal.rgb), layer));

    vec3 colorMod = vec3(1.0);

    switch (layer) {
        case 0:
            colorMod = vec3(1.0, 0.0, 0.0);
            break;
        case 1:
            colorMod = vec3(0.0, 1.0, 0.0);
            break;
        case 2:
            colorMod = vec3(0.0, 0.0, 1.0);
            break;
        case 3:
            colorMod = vec3(0.0, 0.675, 0.80);
            break;
        default:
            colorMod = vec3(1.0, 0.0, 1.0);
    }
    
    vec3 viewSpaceSun = (view * vec4(lightPos, 1.0)).xyz;
    vec3 sunLight;
    if (shadow < 1.0) {
        //shadow *= ScreenSpaceShadows(uv, lightPos);
        sunLight = orthoLight(
            vec4(vec3(20.0, 19.0, 12.0) * (shadow), 1.0),
            vec3(0.0),
            worldPos,
            worldNorm,
            albedo.rgb,
            orm.gb,
            V,
            normalize(viewSpaceSun)
        );
    }

    vec3 finalColor = light + sunLight;
    finalColor += vec3(0.007) * albedo.rgb;
    
    // finalColor *= Bayer2(floor(gl_FragCoord.xy * 0.125)/2);
    // finalColor = vec3(dither(finalColor.r, 1.0/n, gl_FragCoord.x, gl_FragCoord.y),
    // dither(finalColor.g, 1.0/n, gl_FragCoord.x, gl_FragCoord.y),
    // dither(finalColor.b, 1.0/n, gl_FragCoord.x, gl_FragCoord.y));
    
#define AGX

#ifdef AGX
    agx(finalColor);
    agxLook(finalColor);
    agxEotf(finalColor);
#endif
    
#define VOXEL
    
#ifndef VOXEL3
    
    vec4 emmissiveAccum = vec4(0.0);
    
    vec2 resolution = vec2(textureSize(texAlbedo, 0));
    
    // Do like a ring of samples, further out gets more samples
    //for (int dist = 1; dist < 16; dist++) {
    //    int num_samples = 10*dist;
    //    for (int steps = 0; steps < num_samples; steps++) {
    //        float ratio = float(steps) / float(num_samples);
    //        float x = cos(ratio * PI * 2.0) * (dist*3);
    //        float y = sin(ratio * PI * 2.0) * (dist*3);
    //        vec2 smuv = uv + (vec2(x,y) / resolution);
    //        emmissiveAccum += (texture(texEmissive, smuv)/(10*(num_samples*dist))) * (1.0/(num_samples));
    //    }
    //}
    
    // FragColor = vec4(finalColor, 1.0);
    #endif
    
#if 1

#if 0
    Ray worldRay;
    worldRay.Origin = (inverseView * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    worldRay.Direction = GetWorldSpaceDirection(inverse(projection), inverseView, fs_in.position.xy);  // (inverseProjection * vec4(fs_in.position.xy, 1.0, 1.0)).xyz;
    
    Box sceneBounds;
    sceneBounds.Min = u_voxelizerData.gridMin;
    sceneBounds.Max = u_voxelizerData.gridMax;
    
    vec3 voxelSize = vec3(sceneBounds.Max - sceneBounds.Min);
    vec3 voxelColor = vec3(0.0);
#endif
    
    //vec4 voxle = traverseVoxels(worldRay.Origin, worldRay.Direction);
    // + (SSR(worldPos, worldNorm, V) * orm.g)
    
    FragColor = vec4(finalColor, orm.g); // fresnelSchlick(max(dot(normalize(V), normalize(worldNorm)), 0.0), vec3(0.04)) * 
    
    //vec4(vec3(finalColor) + emmissiveAccum.rgb , fresnelSchlick(max(dot(normalize(V), normalize(worldNorm)), 0.0), vec3(0.04)) * orm.g); 
#else
    Ray worldRay;
    worldRay.Origin = (inverseView * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    worldRay.Direction = GetWorldSpaceDirection(inverse(projection), inverseView, fs_in.position.xy);  // (inverseProjection * vec4(fs_in.position.xy, 1.0, 1.0)).xyz;

    vec4 v_currentVoxelSample = texture(texVoxel, WorlSpaceToVoxelImageSpace(worldRay.Origin));
    
    int v_attempts = 0;
    while (v_currentVoxelSample.a < .5 || v_attempts > 8) {
        worldRay.Origin += worldRay.Direction * 0.02;
        v_currentVoxelSample = texture(texVoxel, WorlSpaceToVoxelImageSpace(worldRay.Origin));
        v_attempts++;
    }
    
    FragColor = v_currentVoxelSample;
    
#endif

}