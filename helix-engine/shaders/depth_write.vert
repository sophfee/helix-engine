#version 460 core

layout(location = 0) in vec3  aPosition;  // 0x00 | 00
layout(location = 1) in vec3  aNormal;    // 0x0C | 12
layout(location = 2) in vec2  aTexCoord0; // 0x20 | 36

out struct VS {
    vec3 fragCoord;
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
    mat3 basis;
} fs_in;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 light;
layout(location = 2) uniform mat4 projection;

void main() {
    /*
    vec4 position = vec4(0.0);
    for (int i = 0; i < 4; i++) {
        uint index = uint(aJoints0[i]);
        position += (vec4(aPosition, 1.0) * (skin.world[index] * skin_bind.inv[index])) * aWeights0[i];
    }
    */
    vec4 frag = projection * light * model * vec4(aPosition, 1.0);
    gl_Position = frag.xyzw;
    fs_in.fragCoord = frag.xyz / frag.www;
    fs_in.position = (light * model * vec4(aPosition, 1.0)).xyz;
    fs_in.uv0 = aTexCoord0;
    fs_in.camera = vec3(0.0);
}