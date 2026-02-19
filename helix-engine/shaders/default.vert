#version 460 core

layout (location = 0) in vec3 aPosition; // w is ignored, better spacing for cache lines.
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord0;

uniform mat4 modelViewProjection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out struct VS {
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
} vs;

void main() {
    gl_Position =  projection * view * model * vec4(aPosition, 1.0);
    vs.position = (view * model * vec4(aPosition.xyz, 1.0)).xyz;
    vs.normal = normalize(transpose(inverse(mat3(view*model))) * aNormal);
    vs.uv0 = aTexCoord0;
    vs.camera = view[3].xyz;
}