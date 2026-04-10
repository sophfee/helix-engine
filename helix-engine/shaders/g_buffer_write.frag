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
	float x0 = InterleavedGradientNoise(st1, 0);
	float x1 = InterleavedGradientNoise(st2, 1);

	vec3 T = normalize(Q1 * x1 - Q2 * x0);

	vec3 B = normalize(cross(N, T));
	return mat3(T, B, N);
}
vec3 normalFromMap(out mat3 TBN)
{
	vec3 tangentNormal = (texture(normalTexture, vs.uv0).xyz * 2.0 - 1.0);
    tangentNormal.y *= -1.0;

	TBN = GetTBN(normalize(vs.normal));

	return normalize(TBN * tangentNormal);
}

void main() {
    vec4 color = texture(baseColor, vs.uv0);
    vec4 mr    = texture(metallicRoughness, vs.uv0);
    mat3 tbn   = transpose(vs.basis);
    vec3 nor   = mix(vs.normal, normalFromMap(tbn), 1.0);
    
    if (color.a < 0.5) discard;
    
    Albedo                     = color.rgba;
    Normal                     = vec4(nor, 0.0);
    Position                   = vec4(vs.position, 0.0);
    OcclusionRoughnessMetallic = vec4(mr.rgb, 0.0);
}