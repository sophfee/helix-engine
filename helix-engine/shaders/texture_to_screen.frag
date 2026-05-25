#version 460 core

out vec4 FragColor;

in InOutData {
    vec3 position;
    vec2 texcoord;
} fs_in;

layout (binding = 0) uniform sampler2D u_texture;

void main() {
    FragColor = texture(u_texture, fs_in.texcoord);
}