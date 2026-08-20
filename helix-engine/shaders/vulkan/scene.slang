import common;
import light;

public struct SceneData
{
    public float4x4 view;
    public float4x4 proj;
    public float4x4 viewProj;
    public float4x4 frozenView;
    public float4x4 frozenProj;
    public float4x4 frozenProjView;
    public Frustum frustum;
    public Frustum frozenFrustum;
    public float4 cameraWorldPos;
    public float4 frozenCameraWorldPos;
    public LightData lightData;
    public float time;
    public float deltaTime;
    public float2 fuckshit;
    public float3x3 normal;
}
