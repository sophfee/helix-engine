#version 460 core

#define PI 3.14159265359
#define M_PI PI

/**

Much of this code is copied/modified/referenced from the Godot Engine.
File in question: https://github.com/godotengine/godot/blob/master/servers/rendering/renderer_rd/shaders/scene_forward_lights_inc.glsl
Functions used: SchlickFresnel, D_GGX, V_GGX

**/

layout (location = 0) out 
    vec4 Albedo;

layout (location = 1) out
    vec4 Normal;

layout (location = 2) out
    vec4 Position;

layout (location = 3) out
    vec4 OcclusionRoughnessMetallic;

layout (location = 4) out
    uint ObjectId;

layout (location = 5) out
    vec4 Emissive;

in struct VS {
    vec3 position;
    vec3 tangent;
    vec3 bitangent;
    float handedness;
    vec3 normal;
    vec2 uv0;
    vec2 uv1;
    mat3 TBN;
} fs_in;

in flat float handedness;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 inverse_model;
uniform mat4 inverse_view;
uniform mat4 inverse_projection;

layout (binding = 0)  uniform sampler2D baseColor;
layout (binding = 1)  uniform sampler2D metallicRoughness;
layout (binding = 2)  uniform sampler2D normalTexture;

layout (location = 6)  uniform int mode;
layout (location = 7)  uniform int submode;

layout (location = 9)  uniform vec3 light_position;
layout (location = 10) uniform bool hovering;

layout (location = 11) uniform uint object_id;

uniform vec4 u_baseColorFactor;
uniform bool u_hasBaseColorTexture;
uniform bool u_hasNormalTexture;
uniform bool u_hasMetallicRoughnessTexture;

layout (binding = 3) uniform sampler2D u_emissiveTexture;
uniform bool u_hasEmissiveTexture;
uniform vec4 u_emissiveBias;

float InterleavedGradientNoise(vec2 imgCoord, uint index)
{
	// Source: https://www.shadertoy.com/view/WsfBDf

	imgCoord += float(index) * 5.588238;
	return fract(52.9829189 * fract(0.06711056 * imgCoord.x + 0.00583715 * imgCoord.y));
}

mat3 GetTBN(vec3 normal) {
    // In fragment shader, replace normalFromMap with this:
    vec3 pos_dx = dFdx(fs_in.position);
    vec3 pos_dy = dFdy(fs_in.position);
    vec2 uv_dx  = dFdx(fs_in.uv0);
    vec2 uv_dy  = dFdy(fs_in.uv0);

    vec3 T = normalize( uv_dy.y * pos_dx - uv_dx.y * pos_dy);
    vec3 B = normalize(-uv_dy.x * pos_dx + uv_dx.x * pos_dy);
    vec3 N = normalize(normal);

	return mat3(T, B, N);
}

mat3 generateTBN(vec3 normal)
{
    vec3 bitangent = vec3(0.0, 1.0, 0.0);

    float NdotUp = dot(normal, vec3(view * model * vec4(0.0, 1.0, 0.0, 0.0)));
    float epsilon = 0.01;

    if (1.0 - abs(NdotUp) <= epsilon)
    {
        // Sampling +Y or -Y, so we need a more robust bitangent.
        if (NdotUp < 0.0)
        {
            bitangent = vec3(0.0, 0.0, 1.0);
        }
        else
        {
            bitangent = vec3(0.0, 0.0, -1.0);
        }
    }

    vec3 tangent = normalize(cross(bitangent, normal));
    bitangent = cross(normal, tangent);

    return mat3(tangent, bitangent, normal);
}

vec3 normalFromMap(out mat3 TBN)
{

    vec3 T = (fs_in.tangent);
    vec3 N = (fs_in.normal); // w=0 to avoid translation
    T = normalize(T - dot(T, N) * N); // re-orthogonalize after interpolation
    vec3 B = (handedness * cross(N, T)); // flat, so no interpolation artifacts
    
    TBN = mat3(T, B, N);
    
#ifdef NORMAL_MAP_OPENGL
    vec2 uv = fs_in.uv0;
#else
    vec2 uv = (vec2(fs_in.uv0.x, fs_in.uv0.y));
#endif
    
//#define NORMAL_MAP_2CHAN
    
#ifdef NORMAL_MAP_2CHAN
    vec2 nml = texture(normalTexture, uv).ga;// * 2.0 - 1.0;
    vec3 tangentNormal = normalize(vec3(nml.xy, sqrt(1. - dot(nml.xy, nml.xy)))); // Compute Z
#else
    vec3 tangentNormal = texture(normalTexture, uv).rgb * 2.0 - 1.0;
    tangentNormal.y = -tangentNormal.y; // Invert Y for DirectX normal maps
#endif
    return normalize(TBN * tangentNormal);
}

float saturate(float x) { return clamp(x, 0.0, 1.0); }

void main() {
    vec4 color = u_baseColorFactor;
    if (u_hasBaseColorTexture)
        color.rgb = texture(baseColor, fs_in.uv0).rgb;
    vec4 mr    = u_hasMetallicRoughnessTexture ? texture(metallicRoughness, fs_in.uv0).rbga : vec4(0.0, 1.0, 1.0, 0.0);
    mat3 tbn;
    vec3 nor   = u_hasNormalTexture ? normalFromMap(tbn) : fs_in.normal;
    
    if (color.a < 0.5) discard;

    vec3 dndx = dFdx(fs_in.normal.xyz);
    vec3 dndy = dFdy(fs_in.normal.xyz);
    float geometricRoughnessFactor = pow(saturate(max(dot(dndx, dndx), dot(dndy, dndy))), 0.333);
    
    mr.b = max(mr.b, geometricRoughnessFactor);
    
    Albedo                     = color;
    Normal                     = vec4(mix(fs_in.normal, nor, 1.0), 1.0);// vec4(vec3(fs_in.handedness >= 0.99 ? 1.0 : 0.0, abs(fs_in.handedness) <= 0.0001 ? 1.0 : 0.0, fs_in.handedness <= -0.99 ? 1.0 : 0.0), 1.0);
    Position                   = vec4(fs_in.position, 0.0);
    OcclusionRoughnessMetallic = vec4(mr.rgb, 0.0);
    Emissive                   = u_hasEmissiveTexture ? texture(u_emissiveTexture, fs_in.uv0) + u_emissiveBias : u_emissiveBias;
}