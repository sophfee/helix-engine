import common;
import light;
import material;
import scene;

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
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 texCoord0 : UV0;

	float3 viewPosition : VIEW_POSITION;
	float3 worldPosition : WORLD_POSITION;

	float4x4 view : VIEW;
	float4x4 proj : PROJ;
	nointerpolation float handedness : HANDEDNESS;
}

struct FragmentOutput
{
	float4 color : SV_Target<0>;
	// float depth : SV_Depth;
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
    float4x4 normal;
}

struct PushConstant {
    SceneData *sceneData;
    PerModelData *perModel;
}

[shader("vertex")]
FragmentInput main(in VertexInput input, uniform PushConstant pushConstant)
{
	FragmentInput output;

	float4 worldPosition = mul(pushConstant.perModel->model, float4(input.position, 1.0));
    float4 viewPosition = mul(pushConstant.sceneData->view, worldPosition);
    float4 projPosition = mul(pushConstant.sceneData->proj, viewPosition);

    float4x4 modelView = mul(pushConstant.sceneData->view, pushConstant.perModel->model);
	
	float3x3 normalMatrix = (float3x3)pushConstant.perModel->normal;

	output.clip = projPosition;
	output.normal = normalize(mul(normalMatrix, input.normal).xyz);
	output.tangent = mul(normalMatrix, input.tangent.xyz);
	// output.tangent.xyz = normalize(output.tangent.xyz - output.normal * dot(output.normal, output.tangent.xyz)); // reorthogonalize tangent
	output.handedness = input.tangent.w;
	// output.bitangent = normalize(cross(output.normal, output.tangent.xyz) * output.tangent.w);
	output.texCoord0 = input.texCoord0;

	output.worldPosition = worldPosition.xyz;
	output.viewPosition = viewPosition.xyz;

	output.view = pushConstant.sceneData->view;
	output.proj = pushConstant.sceneData->proj;

	return output;
}

[shader("fragment")]
FragmentOutput main(in FragmentInput input, uniform PushConstant pushConstant)
{
	BRDFMaterial material; // for testing

	float3 bitangent = round(input.handedness) * cross(input.normal, input.tangent.xyz);
	float3x3 TBN = float3x3(normalize(input.tangent.xyz), bitangent, normalize(input.normal));
	float3 normalMap = normalTexture.Sample(samplerState, input.texCoord0).xyz * 2.0 - 1.0;
    // normalMap.y = -normalMap.y; // Flip Y for Vulkan
    float3x3 normalMatrix = mul(float3x3(pushConstant.perModel->normal), TBN);
    material.normal = mul(normalMap, TBN);
    input.normal = material.normal;
    // material.normal = input.normal;

	float4 diffuseColor = diffuseTexture.Sample(samplerState, input.texCoord0);
	float4 ormColor = ormTexture.Sample(samplerState, input.texCoord0);
	material.diffuse = diffuseColor.xyz;
	material.roughness = ormColor.y;              // Assuming roughness is stored in the red channel of the ORM texture
	material.metallic = ormColor.z;               // Assuming metallic is stored in the green channel of the ORM texture
	// material.specular = float3(1.0); // Default specular value for non-metallic surfaces
	material.specular = diffuseColor.rgb;

	float4 accumulatedLight = float4(float3(0.002), 0.0);

	for (int i = 0; i < 64; ++i)
	{
        PointLight light = pushConstant.sceneData->lightData.pointLightData->pointLights[i];
        // light.color /= 255.0;
        light.position = mul(pushConstant.sceneData->view, float4(light.position, 1.0)).xyz;
		accumulatedLight += material.evaluate(light, float3(0.0, 0.0, 0.0), input.viewPosition);
	}

	FragmentOutput output;
	output.color = float4(float3(accumulatedLight.xyz), 1.0);
	return output;
}
