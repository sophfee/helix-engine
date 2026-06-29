#version 460 core

out vec4 FragColor;

in InOutData {
    vec2 texcoord;
} fs_in;

layout (binding = 0) uniform sampler2D u_texture;
layout (binding = 1) uniform sampler2D u_ssr;

void main() {
    vec4 baseSample = texture(u_texture, fs_in.texcoord);
    vec4 ssrSample  = texture(u_ssr, fs_in.texcoord);
    FragColor = vec4(baseSample.rgb, 1.0); //  + (ssrSample.rgb * baseSample.a)
}