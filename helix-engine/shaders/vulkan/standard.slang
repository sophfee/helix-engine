struct VertexInput {
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord0 : UV0;
	//float2 texCoord1 : UV1;
}

struct FragmentInput {
	float4 clip : SV_Position;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord0 : UV0;
	//float2 texCoord1 : UV1;
}

struct FragmentOutput {
	float4 color : SV_Target<0>;
}

// [[vk::binding(0, 0)]]
struct CameraData {
	float4x4 view;
	float4x4 projection;
	float4x4 viewProjection;
	float4x4 inverseView;
	float4x4 inverseProjection;
	float4x4 inverseViewProjection;
}

[[vk::binding(0, 0)]] Texture2D<float4> diffuseTexture;
[[vk::binding(1, 0)]] Texture2D<float4> ormTexture;
[[vk::binding(2, 0)]] Texture2D<float4> normalTexture;
[[vk::binding(3, 0)]] SamplerState samplerState;

struct PerModelData {
	float4x4 model;
    float4x4 perspectiveView;
}

[shader("vertex")]
FragmentInput main(VertexInput input, uniform PerModelData* perModel) {
	FragmentInput output;

    float4 projPosition = mul(perModel->perspectiveView, mul(perModel->model, float4(input.position, 1.0)));

	output.clip = projPosition;
	output.normal = input.normal;
	output.tangent = input.tangent;
	output.texCoord0 = input.texCoord0;
	// output.texCoord1 = input.texCoord1;
	return output;
}

[shader("fragment")]
FragmentOutput main(FragmentInput input) {
	FragmentOutput output;

	float4 diffuseColor = diffuseTexture.Sample(samplerState, input.texCoord0);

	output.color = float4(diffuseColor.xyz, 1.0);
	return output;
}