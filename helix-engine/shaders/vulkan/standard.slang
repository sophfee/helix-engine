struct VertexInput {
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord0 : UV0;
	float2 texCoord1 : UV1;
}

struct FragmentInput {
	float4 clip : SV_Position;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord0 : UV0;
	float2 texCoord1 : UV1;
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

struct PerModelData {
	float4x4 model;
    float4x4 inverseModel;
}

[shader("vertex")]
FragmentInput main(VertexInput input, uniform PerModelData* perModel, uniform CameraData* camera) {
	FragmentInput output;

	float4 worldPosition = mul(perModel->model, float4(input.position, 1.0));
	float4 viewPosition = mul(camera->view, worldPosition);
	float4 projPosition = mul(camera->projection, viewPosition);

	output.clip = projPosition;
	output.normal = input.normal;
	output.tangent = input.tangent;
	output.texCoord0 = input.texCoord0;
	output.texCoord1 = input.texCoord1;
	return output;
}

[shader("fragment")]
FragmentOutput main(FragmentInput input) {
	FragmentOutput output;
	output.color = float4(1.0, 1.0, 1.0, 1.0);
	return output;
}