[[vk::binding(0, 0)]] Texture2D<float> Depth : register(t0);
[[vk::binding(1, 0)]] RWTexture2D<float> HiZ : register(u0);
[[vk::binding(2, 0)]] SamplerState Sampler : register(s0);

groupshared Atomic<uint> MinimumDepth;

[shader("compute")]
[numthreads(4, 4, 1)]
void main(in uint Invocation : SV_GroupIndex, in uint3 GroupThread: SV_GroupThreadID, in uint3 DispatchThread: SV_DispatchThreadID)
{
    if (Invocation == 0) {
        MinimumDepth.store(asuint(1.0));
    }
    GroupMemoryBarrierWithGroupSync();

    uint2 BaseCoords = DispatchThread.xy * 4;
    uint2 TextureDimensions;
    HiZ.GetDimensions(TextureDimensions.x, TextureDimensions.y);
    float2 TexelDimensions = 1.0 / float2(TextureDimensions);

    float2 GatherCoords1 = (BaseCoords + uint2(1, 1)) * TexelDimensions;
    float2 GatherCoords2 = (BaseCoords + uint2(3, 1)) * TexelDimensions;
    float2 GatherCoords3 = (BaseCoords + uint2(1, 3)) * TexelDimensions;
    float2 GatherCoords4 = (BaseCoords + uint2(3, 3)) * TexelDimensions;

    float4 GatheredValues1 = Depth.Gather(Sampler, GatherCoords1);
    float4 GatheredValues2 = Depth.Gather(Sampler, GatherCoords2);
    float4 GatheredValues3 = Depth.Gather(Sampler, GatherCoords3);
    float4 GatheredValues4 = Depth.Gather(Sampler, GatherCoords4);

    float4 MinGatheredValues = min(min(GatheredValues1, GatheredValues2), min(GatheredValues3, GatheredValues4));
    float MinDepthValue = min(min(MinGatheredValues.x, MinGatheredValues.y), min(MinGatheredValues.z, MinGatheredValues.w));
    MinimumDepth.min(asuint(MinDepthValue));

    GroupMemoryBarrierWithGroupSync();

    if (Invocation == 0) {
        HiZ.Store(GroupThread.xy, asfloat(MinimumDepth.load()));
    }
}