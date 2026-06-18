#version 460 core

layout (location = 0) in vec3 aPosition;

layout (location = 0) uniform mat4 model;

void main() {
    vec4 frag = model * vec4(aPosition, 1.0);
    gl_Position = frag;
}