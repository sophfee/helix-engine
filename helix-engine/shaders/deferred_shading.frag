#version 460 core

out vec4 FragColor;

in struct VS_OUT {
    vec3 position;
    vec2 texcoord;
} fs_in;

layout (location = 0) uniform
    sampler2D texAlbedo;
layout (location = 1) uniform
    sampler2D texPosition;
layout (location = 2) uniform
    sampler2D texNormal;
layout (location = 3) uniform
    sampler2D texOrm;

void main() {
    vec4 albedo   = texture( texAlbedo,    fs_in.texcoord );
    vec4 position = texture( texPosition,  fs_in.texcoord );
    vec4 normal   = texture( texNormal,    fs_in.texcoord );
    vec4 orm      = texture( texOrm,       fs_in.texcoord );
    
    FragColor = vec4(albedo.rgb, 1.0);
}