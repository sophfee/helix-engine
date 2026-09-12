[[vk::binding(0, 0)]] Texture2D<float> HiZ : register(t0);
[[vk::binding(1, 0)]] RWTexture2D<float> ShadowMask : register(u0);
[[vk::binding(2, 0)]] SamplerState Sampler : register(s0);

[shader("compute")]
[numthreads(4, 4, 1)]
void main(in uint Invocation: SV_GroupIndex, in uint3 GroupThread: SV_GroupThreadID, in uint3 DispatchThread: SV_DispatchThreadID) {
    
}