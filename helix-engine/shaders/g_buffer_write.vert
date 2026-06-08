#version 460 core
precision highp float;

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
// layout (location = 4) in vec2  aTexCoord1; // 0x20 | 24
#endif

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 inverse_model;
uniform mat4 inverse_view;
uniform mat4 inverse_projection;

layout (location = 9) uniform vec3 light_position;

out struct VS {
    vec3 position;
    vec3 tangent;
    vec3 bitangent;
    float handedness;
    vec3 normal;
    vec2 uv0;
    vec2 uv1;
    mat3 TBN;
} fs_in;

out flat float handedness;
mat3 make_basis(vec3 normal)
{
    #if 1
    // Source: "Building an Orthonormal Basis, Revisited"
    float sign_ = sign(normal.z);
    float a = -1.0 / (sign_ + normal.z);
    float b = normal.x * normal.y * a;
    vec3 tangent = vec3(0.0 + sign_ * normal.x * normal.x * a, sign_ * b, -sign_ * normal.x);
    vec3 bitangent = vec3(b, sign_ + normal.y * normal.y * a, -normal.y);
    return mat3(tangent, bitangent, normal);
    #else
    // +X right +Y up -Z forward
    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    return mat3(tangent, bitangent, normal);
    #endif
}

void main() {
    vec4 frag = projection * view * model * vec4(aPosition, 1.0);
    gl_Position = frag;
    mat4 modelViewMatrix  = mat4(view * model);
    fs_in.position = (modelViewMatrix * vec4(aPosition, 1.0)).xyz;
    fs_in.uv0 = aTexCoord0; // Flip V coordinate for OpenGL
    fs_in.uv1 = aTexCoord0;

    mat3 modelViewNormalMatrix = mat3(
        normalize(vec3(modelViewMatrix[0])),
        normalize(vec3(modelViewMatrix[1])),
        normalize(vec3(modelViewMatrix[2]))
    );

    mat3 modelView_NormalMatrix = mat3(modelViewMatrix);
    mat3 normalMatrix   = transpose(inverse(modelView_NormalMatrix));

    vec3 T = (modelView_NormalMatrix * aTangent.xyz);
    vec3 N = normalize((normalMatrix) * aNormal);
    //T = normalize(T - dot(T, N) * N);
    vec3 B = (aTangent.w * cross(N, T)); // Calculate bitangent using the normal and tangent, and apply handedness

    fs_in.TBN = mat3(T, B, N);

    //mat3 tbn = make_tbn(fs_in.position, fs_in.uv0, fs_in.normal);
    fs_in.tangent    =  T;//*aTangent.w;//normalize(normalViewModelMatrix * aTangent);
    //  fs_in.tangent = vec3(det > 0.0 ? 1.0 : 0.0, 0.0, 0.0); // red=positive, black=negative
    //  fs_in.tangent    =  normalize(T - dot(T, N) * N);
    fs_in.bitangent  =  B;//normalize(cross(N, T));
    fs_in.normal     =  N;//normalize(normalViewModelMatrix * aNormal);
    fs_in.handedness =  (aTangent.w);
    
    handedness = (float(aTangent.w));
}