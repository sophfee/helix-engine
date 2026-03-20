#version 460 core

#define PI 3.14159265359

out vec4 FragColor;

in struct VS {
    vec3 fragCoord;
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
    mat3 basis;
} vs;

layout (location = 0) uniform mat4 model;
uniform mat4 modelViewProjection;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D baseColor;
uniform sampler2D metallicRoughness;
uniform sampler2D normalTexture;

layout (location = 10) uniform bool hovering;
layout (location = 9) uniform vec3 light_position;

uniform int mode;

const vec3 lightPositionTest = vec3(200.0, 100.0, 10.0);

vec3 fresnelSchlick(in float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 omniLight(
    in vec3 lightPos,
    in vec4 lightColEnergy,
    in vec3 viewPos,
    in vec3 fragPos,
    in vec3 normal,
    in vec3 albedo,
    in vec2 metalRough
) {
    vec3 V = normalize(viewPos - fragPos);
    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(L + V);
    vec3 N = normalize(normal);
    
    float NdL = dot(N, V);
    float NdH = max(dot(N, H), 0.0);

    float cosTheta    = max(NdL, 0.0);
    float attenuation = inversesqrt(distance(fragPos, lightPos));
    vec3  radiance    = lightColEnergy.rgb * attenuation * cosTheta;
    
    float metallic  = metalRough.x;
    float roughness = metalRough.y;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float NDF =  DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(NdL, 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    return (kD * albedo / PI + specular) * radiance * NdL;
}

mat3 make_basis(vec3 normal)
{
    // Source: "Building an Orthonormal Basis, Revisited"
    // float sign_ = sign(normal.z);
    // float a = -1.0 / (sign_ + normal.z);
    // float b = normal.x * normal.y * a;
    // vec3 tangent = vec3(1.0 + sign_ * normal.x * normal.x * a, sign_ * b, -sign_ * normal.x);
    // vec3 bitangent = vec3(b, sign_ + normal.y * normal.y * a, -normal.y);
    // return mat3(tangent, normal, bitangent);

    // +X right +Y up -Z forward
    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    return mat3(tangent, bitangent, normal);
}
vec3 calculate_normal_map()
{
    vec3 tangentNormal = texture(normalTexture, vs.uv0).xyz * 2. - 1.;

    vec3 Q1 = dFdx(vs.position);
    vec3 Q2 = dFdy(vs.position);
    vec2 st1 = dFdx(vs.uv0);
    vec2 st2 = dFdy(vs.uv0);

    vec3 N = normalize(vs.normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = vs.basis;

    return normalize(vs.basis * tangentNormal);
}

vec2 spheremap_transform(vec3 n) {
    float p = sqrt(n.z * 8. + 8.);
    return n.xy / p + 0.5;
}


void main() {
    float shade = clamp(1.0 - dot(normalize(vs.camera - vs.position.xyz), vs.normal), 0., 1.);
    bool is_debug_shaded = mode == 4 || mode == 12;
    vec4 color = is_debug_shaded ? vec4(vec3(1.0), texture(baseColor, vs.uv0).a) : texture(baseColor, vs.uv0) ;// * shade;
    vec4 mr =  is_debug_shaded ? vec4(0.0, 0.0, 0.75, 0.0) : texture(metallicRoughness, vs.uv0);
    vec3 nor =  is_debug_shaded ? calculate_normal_map() : calculate_normal_map(); //' normalize(vs.normal * texture(normalTexture, vs.uv0).rgb * vs.basis);
    
    vec3 viewModelLightPos = (view * vec4(light_position, 1.0)).xyz;
    
    vec3 light = omniLight(
        viewModelLightPos,
        vec4(vec3(10.0), 1.0),
        vs.camera,
        vs.position,
        vs.normal,
        color.rgb,
        mr.gb
    );
    
    if (hovering) {
        FragColor = vec4(1.0, 0.8, 0.2, 1.0);
    }
    else
    {
        switch (mode) {
            case 1:
                if (color.a < 0.1) discard;
                FragColor = vec4(light, color.a); 
                break;
            case 2:
                if (color.a < .1) discard;
                FragColor = color;
                break;
            case 3:
                if (color.a < 0.5) discard;
                FragColor = vec4(vec3(dot(vs.normal, normalize(-vs.position))), color.a);
                break;
            case 5:
                if (color.a < 0.5) discard;
                FragColor = vec4(vs.position, 1.0);
                break;
            case 6:
                if (color.a < 0.5) discard;
                FragColor = vec4(vs.normal, 1.0);
                break;
            case 7:
                if (color.a < 0.5) discard;
                FragColor = vec4(nor, color.a);
                break;
            case 8:
                if (color.a < 0.5) discard;
                FragColor = vec4(vec3(inversesqrt(length(viewModelLightPos - vs.position))), color.a);
                break;
            case 9:
                if (color.a < 0.5) discard;
                FragColor = vec4(vec3(dot(nor, normalize(viewModelLightPos - vs.position))), color.a);
                break;
            case 10:
                if (color.a < 0.5) discard;
                FragColor = vec4(spheremap_transform(nor), 0., color.a);
                break;
            case 11:
                FragColor = vec4(vec3(dot(nor, normalize(vs.position - vs.camera))), 1.0);
                break;
            default:
                if (color.a < 0.1) discard;
                FragColor = vec4(light, color.a);
                break;
        }
    }

    //vec4(color.rgb * vec3(dot(vs.normal, normalize(lightPositionTest - vs.position))),1.0);//color * light; // vec4(color.rgb, 1.0);
}