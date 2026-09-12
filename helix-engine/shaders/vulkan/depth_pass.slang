import scene;

struct VertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texCoord0 : UV0;
};

struct PixelIn
{
    float4 position : SV_POSITION;
    float2 texCoord0 : UV0;
};

struct PixelOut {
    float depth : SV_DEPTH;
};

struct PerModelData {
    float4x4 modelMatrix;
};

struct PushConstant {
    SceneData *sceneData;
    PerModelData *modelData;
};

[shader("vertex")]
PixelIn main(VertexIn input, uniform PushConstant pushConstant) {
    PixelIn output;

    float4 worldPosition = mul(pushConstant.modelData->modelMatrix, float4(input.position, 1.0));
    float4 clipPosition = mul(pushConstant.sceneData->viewProj, worldPosition);

    output.position = clipPosition;
    output.texCoord0 = input.texCoord0;

    return output;
}

[shader("fragment")]
PixelOut main(PixelIn input) {
    PixelOut output;
    output.depth = input.position.z / input.position.w;
    return output;
}