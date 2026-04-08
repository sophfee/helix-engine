#version 460 core

layout ( location = 0 ) in
    vec3 aPosition;

layout ( location = 1 ) in
    vec2 aTexCoord;

out struct VS_OUT {
    vec3 position;
    vec2 texcoord;
} fs_in;

void main() {
    fs_in.position = aPosition;
    fs_in.texcoord = aTexCoord;
    gl_Position = vec4(aPosition.xy, 1.0, 1.0);
}