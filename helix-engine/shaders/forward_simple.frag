#version 460 core

out vec4 FragColor;

in InOutData {
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec2 TexCoord;
} fs_in;

void main() {
    FragColor = vec4(vec3(1.0), 1.0);
}