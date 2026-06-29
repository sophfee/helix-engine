#version 460 core

#extension GL_NV_gpu_shader5 : require

layout ( location = 0 ) in
    vec2 aPosition;

out InOutData {
    vec2 texcoord;
} vs_out;

void main() {
    vs_out.texcoord = vec2(aPosition.xy * 0.5 + 0.5);
    gl_Position     = vec4(aPosition.xy, 1.0, 1.0);
}