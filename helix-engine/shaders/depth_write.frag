#version 460 core

in struct VS {
    vec3 fragCoord;
    vec3 position;
    vec3 normal;
    vec3 camera;
    vec2 uv0;
    mat3 basis;
} fs_in;

layout(location = 3) uniform sampler2D albedo_texture;

void main() {
    vec4 albedo = texture(albedo_texture, fs_in.uv0);
    if (albedo.a < 1.0)
        discard;
    
    // gl_FragDepth = gl_FragCoord.z / gl_FragCoord.w;
}