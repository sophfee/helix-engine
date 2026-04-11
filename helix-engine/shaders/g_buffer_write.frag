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
} fs_in;

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
    // mat4 invView = inverse(view);
    // vec3 eyePos = view[3].xyz;
	
	// put into view space

    vec3 fragPos = ( inverse( view ) * vec4( fs_in.position, 1.0 ) ).xyz;

	vec3 Q1 = dFdxCoarse( fragPos );
	vec3 Q2 = dFdyCoarse( fragPos );
	vec2 st1 = dFdxCoarse( fs_in.uv0 );
	vec2 st2 = dFdyCoarse( fs_in.uv0 );

    // vec3 rev = inverse( mat3( view ) ) * normalize( normal );

	vec3 N = normalize(normal );
    vec3 T = cross( Q1 * st2.t - Q2 * st1.t, N );
    // vec3 T = normalize(  Q1 * st2.t - Q2 * st1.t );
    // vec3 S = normalize( -Q1 * st2.s + Q2 * st1.s );
    // vec3 V = normalize( -fs_in.position );
    vec3 S = cross( N, T );
	vec3 B = normalize( cross( N, T ) );// * (dot(cross(N, S), T)) ); //cross( S, T ) * ( sign( dot( cross(S, T), N ) ) ) );
    // vec3 C = mix(T, S, step(0.0, dot(cross(S, T), N)));
	return mat3(S, T, N);
}
vec3 normalFromMap(out mat3 TBN)
{
	vec3 tangentNormal = (texture(normalTexture, fs_in.uv0).xyz );
    tangentNormal = tangentNormal * 2.0 - 1.0;
    tangentNormal.y = 1.0 - tangentNormal.y;

	TBN = GetTBN(normalize(fs_in.normal));

	return normalize(TBN * tangentNormal);
}

void main() {
    vec4 color = texture(baseColor, fs_in.uv0);
    vec4 mr    = texture(metallicRoughness, fs_in.uv0);
    mat3 tbn   = transpose(fs_in.basis);
    vec3 nor   = mix(fs_in.normal, normalFromMap(tbn), 1.0);
    
    // if (color.a < 0.5) discard;
    
    Albedo                     = color.rgba;
    Normal                     = vec4(nor, 1.0);
    Position                   = vec4(fs_in.position, 0.0);
    OcclusionRoughnessMetallic = vec4(mr.rgb, 0.0);
}