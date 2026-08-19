public struct Frustum
{
    public float4 planes[6];
}

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

    public float time;
    public float deltaTime;
}

public struct Vertex
{
    public float4 position;
    public float3 normal;
    internal float _padding;
    public float4 tangent;
    public float2 uv;
    internal float2 uv2;
}

public struct VertexData
{
    public Vertex[] vertices;
}

public struct Meshlet {
    public float4 meshletBoundingSphere;

    public float3 coneApex;
    public float coneCutoff;

    public float3 coneAxis;
    public uint32_t vertexOffset;

    public uint32_t meshletVerticesOffset;
    public uint32_t meshletTriangleOffset;
    public uint32_t meshletVerticesCount;
    public uint32_t meshletTriangleCount;
}

public struct MeshletData {
    public Meshlet[] meshlets;
}

public struct MeshletVertexIndex {
    public uint32_t vertexIndex;
}

public struct MeshletVertexData {
    public MeshletVertexIndex[] meshletVertices;
}

public struct MeshletTriangleIndex {
    public uint8_t triangleIndex;
}

public struct MeshletTriangleData {
    public MeshletTriangleIndex[] meshletTriangles;
}