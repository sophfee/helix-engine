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

in struct VS {
    vec3 fragCoord;
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
    mat3 basis;
} vs;

layout (location = 0)  uniform mat4 model;
layout (location = 1)  uniform mat4 view;
layout (location = 2)  uniform mat4 projection;

layout (location = 3)  uniform sampler2D baseColor;
layout (location = 4)  uniform sampler2D metallicRoughness;
layout (location = 5)  uniform sampler2D normalTexture;

layout (location = 6)  uniform int mode;
layout (location = 7)  uniform int submode;

layout (location = 9)  uniform vec3 light_position;
layout (location = 10) uniform bool hovering;

layout (location = 11) uniform uint object_id;

float InterleavedGradientNoise(vec2 imgCoord, uint index)
{
	// Source: https://www.shadertoy.com/view/WsfBDf

	imgCoord += float(index) * 5.588238;
	return fract(52.9829189 * fract(0.06711056 * imgCoord.x + 0.00583715 * imgCoord.y));
}

mat3 GetTBN(vec3 normal) {
	
	// put into view space
	vec3 Q1 = dFdx(vs.position);
	vec3 Q2 = dFdy(vs.position);
	vec2 st1 = dFdx(vs.uv0);
	vec2 st2 = dFdy(vs.uv0);

	vec3 N = normalize(normal);
	float x0 = InterleavedGradientNoise(vs.uv0, 0);
	float x1 = InterleavedGradientNoise(vs.uv0, 1);

	vec3 T = normalize(Q1 * x1 - Q2 * x0);

	vec3 B = normalize(cross(N, T));
	return mat3(T, B, N);
}
vec3 normalFromMap(out mat3 TBN)
{
	vec3 tangentNormal = (texture(normalTexture, vs.uv0).xyz * 2.0 - 1.0);

	TBN = GetTBN(normalize(vs.normal));

	return normalize(TBN * tangentNormal);
}


vec3 calculate_normal_map()
{
    vec4 normalMap = texture(normalTexture, vs.uv0);
    //normalMap.g = 1.0 - normalMap.g;
    vec3 tangentNormal = (normalMap.rgb * 2. - 1.);

    vec3 Q1 = dFdx(vs.position);
    vec3 Q2 = dFdy(vs.position);
    vec2 st1 = dFdx(vs.uv0);
    vec2 st2 = dFdy(vs.uv0);

    vec3 N = normalize(vs.normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}


vec2 OctWrap(vec2 v) {
    vec2 w = 1.0 - abs(v.yx);
    if (v.x < 0.0) w.x = -w.x;
    if (v.y < 0.0) w.y = -w.y;
    return w;
}

/// Source: https://www.shadertoy.com/view/cljGD1
// vec3 in range [-1.0, 1.0] with length=1 ->
// vec2 in range [ 0.0, 1.0]
vec2 encode_unit_vector(vec3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z > 0.0 ? n.xy : OctWrap(n.xy);
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}
// vec2 in range [ 0.0, 1.0] ->
// vec3 in range [-1.0, 1.0] with length=1
vec3 decode_unit_vector(vec2 f)
{
    f = f * 2.0 - 1.0;

    // https://twitter.com/Stubbesaurus/status/937994790553227264
    vec3 n = vec3(f.xy, 1.0 - abs(f.x) - abs(f.y));
    float t = max(-n.z, 0.0);
    n.x += n.x >= 0.0 ? -t : t;
    n.y += n.y >= 0.0 ? -t : t;
    return normalize(n);
}

vec2 spheremap_transform(vec3 n) {
    float p = sqrt(n.z * 8. + 8.);
    return n.xy / p + 0.5;
}

void main() {
    vec4 color = texture(baseColor, vs.uv0);
    vec4 mr    = texture(metallicRoughness, vs.uv0);
    // mat3 tbn   = transpose(vs.basis);
    // vec3 nor   = calculate_normal_map();
    
    if (color.a < 0.5) discard;
    
    Albedo                     = color.rgba;
    Normal                     = vec4(vs.normal, 0.0);
    Position                   = vec4(vs.position, 0.0);
    OcclusionRoughnessMetallic = vec4(mr.rgb, 0.0);
}