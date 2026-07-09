
#pragma include "shaders/types.glsl"

/* Nearly all of our data can be crammed into one singular uninterleaved buffer */
layout (std430, binding = 1) restrict /* read+write */ buffer DrawCommandBuffer {
    DrawElementsIndirectCommand drawCommands[];
} drawCommandSSBO;

layout (std430, binding = 2) restrict buffer DrawCommandCountBuffer {
    uint drawCommandCount;
} drawCommandCountSSBO;

layout (std430, binding = 3) restrict buffer VertexBuffer {
    Vertex vertices[];
} vertexSSBO;

layout (std430, binding = 4) restrict buffer IndexBuffer {
    uint16_t indices[];
} indexSSBO;

layout (std430, binding = 5) restrict readonly buffer MeshBuffer {
    GpuMesh meshes[];
} meshSSBO;

layout (std430, binding = 6) restrict readonly buffer MeshInstanceBuffer {
    GpuMeshInstance meshInstances[];
} meshInstanceSSBO;

layout (std430, binding = 7) restrict readonly buffer MeshTransformBuffer {
    GpuMeshTransform meshTransforms[];
} meshTransformSSBO;

layout (std430, binding = 8) restrict buffer VisibleMeshesBuffer {
    uint visibleMeshes;
} visibleMeshesSSBO;

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

layout (std430, binding = 12) readonly buffer PointShadowBuffer {
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

layout (std430, binding = 16) restrict readonly buffer FrameDataBuffer {
    FrameData frameData;
} frameDataSSBO;


FrameData GetFrameData() {
    return frameDataSSBO.frameData;
}