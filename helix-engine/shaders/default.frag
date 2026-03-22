#version 460 core

#define PI 3.14159265359
#define M_PI PI

/**

Much of this code is copied/modified/referenced from the Godot Engine.
File in question: https://github.com/godotengine/godot/blob/master/servers/rendering/renderer_rd/shaders/scene_forward_lights_inc.glsl
Functions used: SchlickFresnel, D_GGX, V_GGX

**/

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
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

layout (location = 3) uniform sampler2D baseColor;
layout (location = 4) uniform sampler2D metallicRoughness;
layout (location = 5) uniform sampler2D normalTexture;

layout (location = 6) uniform int mode;
layout (location = 7) uniform int submode;

layout (location = 9) uniform vec3 light_position;
layout (location = 10) uniform bool hovering;


const vec3 lightPositionTest = vec3(200.0, 100.0, 10.0);

#define satuarte

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

vec3 computeTangent(vec3 N) {
    // Find a guide vector that is not parallel to N
    vec3 guide = abs(N.x) > 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    // Gram-Schmidt orthogonalization to find a tangent
    vec3 T = normalize(cross(guide, N));
    return T;
}

vec3 calculate_normal_map()
{
    vec4 normalMap = texture(normalTexture, vs.uv0);
    normalMap.g = 1.0 - normalMap.g;
    vec3 tangentNormal = (normalMap.rgb * 2. - 1.);

    vec3 Q1 = dFdx(vs.position);
    vec3 Q2 = dFdy(vs.position);
    vec2 st1 = dFdx(vs.uv0);
    vec2 st2 = dFdy(vs.uv0);
    
    vec3 N = normalize(vs.normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

vec2 spheremap_transform(vec3 n) {
    float p = sqrt(n.z * 8. + 8.);
    return n.xy / p + 0.5;
}


void main() {
    float shade = clamp(1.0 - dot(normalize(vs.camera - vs.position.xyz), vs.normal), 0., 1.);
    bool is_debug_shaded = mode == 4 || mode == 12;
    vec4 color = is_debug_shaded ? vec4(vec3(1.0), texture(baseColor, vs.uv0).a) : texture(baseColor, vs.uv0) ;// * shade;
    vec4 mr = is_debug_shaded ? vec4(0.0, 0.0, 0.75, 0.0) : texture(metallicRoughness, vs.uv0);
    
    mat3 tbn = transpose(vs.basis);
    vec3 nor = is_debug_shaded ? vs.normal : calculate_normal_map();
    vec3 viewModelLightPos = ((view) * vec4(light_position, 1.0)).xyz;
    
    vec3 V = normalize(vec3(0.0) - vs.position);
    vec3 L = normalize(viewModelLightPos - vs.position);
    
    vec3 light = omniLight(
        viewModelLightPos,
        vec4(vec3(3.0, 2.5, 2.9) * 50.0, 1.0),
        vec3(0.0),
        vs.position,
        nor,
        color.rgb,
        mr.gb,
        V,
        L
    );
    //light = vec3(dot(nor, normalize(viewModelLightPos.xyz - vs.position)));
    
    if (color.a < 0.999) discard;
    if (hovering) {
        // coment
        FragColor = vec4(1.0, 0.1, 0.0, 1.0).bgra;
    }
    else
    {
        switch (mode) {
            case 1:
                FragColor = vec4(light, color.a);
                break;
            case 2:
                FragColor = color;
                break;
            case 3:
                FragColor = vec4(vec3(dot(tbn[1] * .5 + .5, nor)), 1.0); //vec4(vec3(dot(vs.normal, normalize(-vs.position))), color.a);
                break;
            case 5:
                vec3 pos = vs.position / 10.0;
                //pos = pos * .5 + .5;
                FragColor = vec4(pos, 1.0);
                break;
            case 6:
                FragColor = vec4(vs.normal * 0.5 + 0.5, 1.0);
                break;
            case 7:
                FragColor = vec4(nor * .5 + .5, color.a);
                break;
            case 8:
                float atten = inversesqrt(length(viewModelLightPos - vs.position));
                atten = atten * atten;
                FragColor = vec4(vec3(atten), color.a);
                break;
            
#define NORMAL_DEBUG_CHANNEL_FUNC(VALUE) vec3(0.0, max(VALUE.x, 0.0), 1.0 - abs(min(VALUE.x, 0.)))
            case 9:
                FragColor = vec4(NORMAL_DEBUG_CHANNEL_FUNC(nor.rrr), color.a);
                break;
            case 10:
                FragColor = vec4(NORMAL_DEBUG_CHANNEL_FUNC(nor.ggg), color.a);
                break;
            case 11:
                FragColor = vec4(NORMAL_DEBUG_CHANNEL_FUNC(nor.bbb), 1.0);
                break;
            default:
                if (color.a < 0.999) discard;
                FragColor = vec4(light, color.a);
                break;
        }
    }

    //vec4(color.rgb * vec3(dot(vs.normal, normalize(lightPositionTest - vs.position))),1.0);//color * light; // vec4(color.rgb, 1.0);
}