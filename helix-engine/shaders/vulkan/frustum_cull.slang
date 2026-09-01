struct AABB
{
    float4 min;
    float4 max;
};

struct Primitive
{
    float4x4 model;
    AABB bounds;
    float4 center;
};

struct Frustum
{
    float4 planes[6];
};

static const uint THREAD_COUNT = 64;

struct ThreadData
{
    Primitive primitive[THREAD_COUNT];
};

struct CullResult
{
    uint visiblePrimitives[THREAD_COUNT];
};

struct PushConstant
{
    Frustum frustum;
    ThreadData *threadData;
    CullResult *cullResult;
};

groupshared Atomic<uint> visiblePrimitives;

[shader("compute")]
[NumThreads(THREAD_COUNT, 1, 1)]
void computeMain(in uint groupID: SV_GroupIndex,
                 in uint3 groupThreadID: SV_GroupThreadID,
                 uniform PushConstant pushConstant)
{

    uint groupThreadIndex = groupThreadID.x;

    if (groupThreadIndex == 0)
    {
        visiblePrimitives.store(0);
    }
    GroupMemoryBarrierWithGroupSync();

    // Perform frustum culling for the current thread's primitive
    Primitive primitive = pushConstant.threadData[groupID].primitive[groupThreadIndex];
    bool isVisible = true;
    for (uint i = 0; i < 6; ++i)
    {
        float4 plane = pushConstant.frustum.planes[i];
        float4 center = primitive.center;
        float4 extents = (primitive.bounds.max - primitive.bounds.min) * 0.5;
        float4 absPlane = abs(plane);
        if (dot(center, plane) + dot(extents, absPlane) < 0)
        {
            isVisible = false;
            break;
        }
    }
    if (isVisible)
    {
        AtomicAdd(visiblePrimitives, 1);
        pushConstant.cullResult->visiblePrimitives[groupThreadIndex] = 1;
    }
    else
    {
        pushConstant.cullResult->visiblePrimitives[groupThreadIndex] = 0;
    }
}
