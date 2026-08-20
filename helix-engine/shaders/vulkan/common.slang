public static const float PI = 3.14159265358979323846;

public struct Frustum
{
    public float4 planes[6];
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