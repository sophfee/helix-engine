#version 460 core

layout ( location = 0 ) in
    vec3 aPosition;

layout ( location = 1 ) in
    vec2 aTexCoord;

out InOutData {
    vec3 position;
    vec2 texcoord;
} vs_out;

void main() {
    vs_out.position = aPosition;
    vs_out.texcoord = aTexCoord;
    gl_Position     =  vec4(aPosition.xy, 1.0, 1.0);
}