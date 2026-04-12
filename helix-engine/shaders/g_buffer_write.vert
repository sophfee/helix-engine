#version 460 core

#ifdef SKINNED
// Vertex total width is 64 bytes. Good size!
layout (location = 0) in vec3  aPosition;  // 0x00 | 00
layout (location = 1) in vec3  aNormal;    // 0x0C | 12
layout (location = 2) in vec3  aCustom0;   // 0x18 | 24
layout (location = 3) in vec2  aTexCoord0; // 0x20 | 36
layout (location = 4) in uvec4 aJoints0;   // 0x30 | 48
layout (location = 5) in vec4  aWeights0;  // 0x40 | 64
#else
layout (location = 0) in vec3  aPosition;  // 0x00 | 00
layout (location = 1) in vec3  aNormal;    // 0x0C | 12
layout (location = 2) in vec4  aTangent;   // 0x18 | 32 << Tangent is placed here so that UV0 aligns on the 32 byte point.
layout (location = 3) in vec2  aTexCoord0; // 0x20 | 24
layout (location = 4) in vec2  aTexCoord1; // 0x20 | 24
#endif

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

layout (location = 9) uniform vec3 light_position;

out struct VS {
    vec3 position;
    vec3 tangent;
    vec3 bitangent;
    float handedness;
    vec3 normal;
    vec2 uv0;
    vec2 uv1;
} fs_in;

mat3 make_basis(vec3 normal)
{
    #if 1
    // Source: "Building an Orthonormal Basis, Revisited"
    float sign_ = sign(normal.z);
    float a = -1.0 / (sign_ + normal.z);
    float b = normal.x * normal.y * a;
    vec3 tangent = vec3(1.0 + sign_ * normal.x * normal.x * a, sign_ * b, -sign_ * normal.x);
    vec3 bitangent = vec3(b, sign_ + normal.y * normal.y * a, -normal.y);
    return mat3(tangent, normal, bitangent);
    #else
    // +X right +Y up -Z forward
    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    return mat3(tangent, bitangent, normal);
    #endif
}

void main() {
    /*
    vec4 position = vec4(0.0);
    for (int i = 0; i < 4; i++) {
        uint index = uint(aJoints0[i]);
        position += (vec4(aPosition, 1.0) * (skin.world[index] * skin_bind.inv[index])) * aWeights0[i];
    }
    */
    vec4 frag = projection * view * model * vec4(aPosition, 1.0);
    gl_Position = frag.xyzw;
    fs_in.position = (view * model * vec4(aPosition, 1.0)).xyz;
    fs_in.uv0 = aTexCoord0;
    fs_in.uv1 = aTexCoord1;

    mat3 normalViewModelMatrix = transpose(inverse(mat3(view*model)));

    vec3 T = normalize(normalViewModelMatrix * aTangent.xyz) * aTangent.w;
    vec3 N = normalize(normalViewModelMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    // float det = determinant(model);
    // bool flipped = det < 0.0; 

    // // N.x = flipped ? -N.x : N.x;


    //mat3 tbn = make_tbn(fs_in.position, fs_in.uv0, fs_in.normal);
    fs_in.tangent    =  T;//*aTangent.w;//normalize(normalViewModelMatrix * aTangent);
    // fs_in.tangent    =  normalize(T - dot(T, N) * N);
    fs_in.normal     =  N;//normalize(normalViewModelMatrix * aNormal);
    fs_in.bitangent  =  B;//normalize(cross(N, T));
    fs_in.handedness =  (aTangent.w);
}