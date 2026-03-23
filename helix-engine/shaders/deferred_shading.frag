#version 460 core

layout ( location = 0 ) 
    out vec4 FragColor;

in struct VS_OUT {
    vec3 position;
    vec2 texcoord;
} fs_in;

layout ( location = 0 ) uniform
    sampler2D texAlbedo;
layout ( location = 1 ) uniform
    sampler2D texPosition;
layout ( location = 2 ) uniform
    sampler2D texNormal;
layout ( location = 3 ) uniform
    sampler2D texOrm;

layout ( location = 4 ) uniform
    mat4 inverseProjection;

layout ( location = 5 ) uniform
    mat4 inverseView;

layout ( location = 6 ) uniform
    mat4 projection;

layout ( location = 7 ) uniform
    mat4 view;

layout ( location = 11 ) uniform
    vec3 point_light;

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

const uint  g_sss_max_steps        = 256;     // Max ray steps, affects quality and performance.
const float g_sss_ray_max_distance = 100.0;  // Max shadow length, longer shadows are less accurate.
const float g_sss_thickness        = 0.01;  // Depth testing thickness.
const float g_sss_step_length      = g_sss_ray_max_distance / float(g_sss_max_steps);

struct OmniLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
};

layout (std430, binding = 0) buffer OmniLightBuffer {
    uint count;
    OmniLight data[];
} omniLights;

vec3 position_at(vec2 uv) {
    return texture(texPosition, uv).xyz;
}

bool in_bounds(vec2 uv) {
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

float ScreenSpaceShadows(vec2 uv, vec3 light_world_space)
{
    vec3 light_pos = (view * vec4(light_world_space, 1.0)).xyz;
    vec3 ray_pos = position_at(uv);
    vec3 start_pos = ray_pos;
    vec3 ray_dir = normalize(light_pos - ray_pos); // In view space!

    vec3 ray_step = ray_dir * g_sss_step_length;

    float occlusion = 0.0;
    vec2 ray_uv     = vec2(0.0);
    for (uint i = 0; i < g_sss_max_steps; i++)
    {
        // Step the ray
        ray_pos += ray_step;
        vec4 projected = (projection * vec4(ray_pos, 1.0));
        projected.xy /= projected.w;
        ray_uv  = projected.xy * .5 + .5;

        // Ensure the UV coordinates are inside the screen
        if (in_bounds(ray_uv))
        {
            // Compute the difference between the ray's and the camera's depth
            float depth_z     = position_at(ray_uv).z / 128.0;
            float depth_delta = ray_pos.z - depth_z;

            // Check if the camera can't "see" the ray (ray depth must be larger than the camera depth, so positive depth_delta)
            if (((depth_delta) > 0.0f) && (depth_delta < g_sss_thickness))
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
    return 1.0f - occlusion;
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
    float a = NoH * roughness;
    float k = roughness / (1.0 - NoH * NoH + a * a);
    float d = k * k * float(1.0 / M_PI);
    return clamp(d, 0., 1.);
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

vec3 omniLight(
in vec3 lightPos,
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

    float NdL = dot(N, V);
    float NdH = max(dot(N, H), 0.0);

    float cosTheta    = max(NdL, 0.0);
    float dist        = distance(fragPos, lightPos);
    float attenuation = 1.0 / (dist * dist);
    vec3  radiance    = lightColEnergy.rgb * attenuation * cosTheta;

    float metallic  = metalRough.x;
    float roughness = metalRough.y;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);
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

void main() {
    vec2 uv = fs_in.position.xy * .5 + .5;
    
    vec4 albedo   = texture( texAlbedo,   uv );
    vec4 position = texture( texPosition, uv );
    vec4 normal   = texture( texNormal,   uv );
    vec4 orm      = texture( texOrm,      uv );
    
    vec3 light = vec3(0.0);
    vec3 V = normalize(vec3(0.0) - position.xyz);
    
    for (uint u = 0u; u < omniLights.count; ++u) {
        OmniLight ol = omniLights.data[u];
        vec3 L = normalize(ol.position - position.xyz); 
        light += omniLight(
            ol.position,
            vec4(ol.color, ol.intensity),
            vec3(0.),
            position.xyz,
            normal.rgb,
            albedo.rgb,
            orm.gb,
            V, L
        );
    }
    
    FragColor = vec4(light.rgb, 1.0);
}