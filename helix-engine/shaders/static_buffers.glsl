
#pragma include "shaders/types.glsl"

layout (std430, binding = 7) restrict readonly buffer FrameDataBuffer {
    FrameData frameData[];
} frameDataSSBO;

layout (std430, binding = 8) restrict readonly buffer MeshBuffer {
    Mesh meshes[];
} meshBufferSSBO;

layout (std430, binding = 9) restrict readonly buffer MaterialBuffer {
    Material materials[];
} materialBufferSSBO;

layout (std430, binding = 10) restrict readonly buffer PointLightBuffer {
    PointLight pointLights[];
} pointLightBufferSSBO;

PointLight GetPointLight(int index) {
    return pointLightBufferSSBO.pointLights[index];
}

layout (std430, binding = 11) restrict readonly buffer SpotLightBuffer {
    SpotLight spotLights[];
} spotLightBufferSSBO;

SpotLight GetSpotLight(int index) {
    return spotLightBufferSSBO.spotLights[index];
}

layout (std430, binding = 12) restrict readonly buffer PointShadowBuffer {
    PointShadow pointShadows[];
} pointShadowBufferSSBO;

PointShadow GetPointShadow(int index) {
    return pointShadowBufferSSBO.pointShadows[index];
}

layout (std430, binding = 13) restrict readonly buffer SpotShadowBuffer {
    SpotShadow spotShadows[];
} spotShadowBufferSSBO;

SpotShadow GetSpotShadow(int index) {
    return spotShadowBufferSSBO.spotShadows[index];
}