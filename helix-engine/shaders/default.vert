#version 460 core

#extension GL_ARB_explicit_uniform_location : enable

// Vertex total width is 64 bytes. Good size!
layout (location = 0) in vec3 aPosition;  // 0x0C | 12
layout (location = 1) in vec3 aNormal;    // 0x18 | 24
layout (location = 2) in vec2 aTexCoord0; // 0x20 | 32
layout (location = 3) in vec4 aJoints0;   // 0x30 | 48
layout (location = 4) in vec4 aWeights0;  // 0x40 | 64

layout (location = 0) uniform mat4 model;
uniform mat4 modelViewProjection;
uniform mat4 view;
uniform mat4 projection;

out struct VS {
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
} vs;

layout (binding = 0, std430) buffer SkinData {
    mat4 inverseBindMatrices[];
} skin;

void main() {
    gl_Position =  projection * view * model * vec4(aPosition, 1.0);
    vs.position = (view * model * vec4(aPosition.xyz, 1.0)).xyz;
    vs.normal = normalize(transpose(inverse(mat3(view*model))) * aNormal);
    vs.uv0 = aTexCoord0;
    vs.camera = view[3].xyz;
}