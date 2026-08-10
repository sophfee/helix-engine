#include "material.slang"

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texCoord0 : UV0;
    // float2 texCoord1 : UV1;
}

struct FragmentInput
{
    float4 clip : SV_Position;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texCoord0 : UV0;

    float3 viewPosition : VIEW_POSITION;
    float3 worldPosition : WORLD_POSITION;

    float4x4 view : VIEW;
    float4x4 proj : PROJ;
}

struct FragmentOutput
{
    float4 color : SV_Target<0>;
}

// [[vk::binding(0, 0)]]
struct CameraData
{
    float4x4 view;
    float4x4 projection;
    float4x4 viewProjection;
    float4x4 inverseView;
    float4x4 inverseProjection;
    float4x4 inverseViewProjection;
}

[[vk::binding(0, 0)]]
Texture2D<float4> diffuseTexture;

[[vk::binding(1, 0)]]
Texture2D<float4> ormTexture;

[[vk::binding(2, 0)]]
Texture2D<float4> normalTexture;

[[vk::binding(3, 0)]]
SamplerState samplerState;

struct PerModelData
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4x4 projView;
    float4x4 normalMatrix;
}

[shader("vertex")]
FragmentInput main(VertexInput input, uniform PerModelData *perModel)
{
    FragmentInput output;

    float4 worldPosition = mul(perModel->model, float4(input.position, 1.0));
    float4 viewPosition = mul(perModel->view, worldPosition);
    float4 projPosition = mul(perModel->proj, viewPosition);

    float4x4 modelView = mul(perModel->view, perModel->model);

    output.clip = projPosition;
    output.normal = normalize(mul(float3x3(perModel->normalMatrix), input.normal).xyz);
    output.tangent = float4(mul(float3x3(perModel->normalMatrix), input.tangent.xyz).xyz, input.tangent.w);
    // output.tangent.xyz = normalize(output.tangent.xyz - output.normal * dot(output.normal, output.tangent.xyz)); // reorthogonalize tangent
    output.texCoord0 = input.texCoord0;

    output.worldPosition = worldPosition.xyz;
    output.viewPosition = viewPosition.xyz;

    output.view = perModel->view;
    output.proj = perModel->proj;

    return output;
}

[shader("fragment")]
FragmentOutput main(FragmentInput input)
{
    BRDFMaterial material; // for testing

    float3 bitangent = normalize(cross(input.normal, input.tangent.xyz) * input.tangent.w);
    float3x3 TBN = float3x3(input.tangent.xyz, bitangent, input.normal);
    float3 normalMap = normalTexture.Sample(samplerState, input.texCoord0).xyz * 2.0 - 1.0;
    normalMap.y = -normalMap.y; // Flip Y for Vulkan
    // material.normal = normalize(mul(TBN, normalMap));
    // input.normal = material.normal;
    material.normal = input.normal;
    float4 diffuseColor = diffuseTexture.Sample(samplerState, input.texCoord0);
    float4 ormColor = ormTexture.Sample(samplerState, input.texCoord0);
    material.diffuse = diffuseColor.xyz;
    material.roughness = ormColor.y;              // Assuming roughness is stored in the red channel of the ORM texture
    material.metallic = ormColor.z;               // Assuming metallic is stored in the green channel of the ORM texture
    // material.specular = float3(1.0); // Default specular value for non-metallic surfaces
    material.specular = diffuseColor.rgb;

    float4 accumulatedLight = float4(0.0, 0.0, 0.0, 0.0);

    PointLight light;
    light.position = mul(input.view, float4(0.0, 10.0, 6.0, 1.0)).xyz;
    light.color = float3(1.0, 0.0, 1.0);
    light.intensity = 6.0;
    
    // popp
    
    accumulatedLight += material.evaluate(light, float3(0.0, 0.0, 0.0), input.viewPosition);
    light.position = mul(input.view, float4(0.0, 10.0, -6.0, 1.0)).xyz;
    light.color = float3(1.0, 0.0, 0.0);
    accumulatedLight += material.evaluate(light, float3(0.0, 0.0, 0.0), input.viewPosition);
    light.position = mul(input.view, float4(6.0, 10.0, 0.0, 1.0)).xyz;
    light.color = float3(0.0, 1.0, 0.0);
    accumulatedLight += material.evaluate(light, float3(0.0, 0.0, 0.0), input.viewPosition);
    light.position = mul(input.view, float4(-6.0, 10.0, 0.0, 1.0)).xyz;
    light.color = float3(0.0, 0.0, 1.0);
    accumulatedLight += material.evaluate(light, float3(0.0, 0.0, 0.0), input.viewPosition);

    FragmentOutput output;
    output.color = float4((accumulatedLight.xyz), 1.0);
    return output;
}
