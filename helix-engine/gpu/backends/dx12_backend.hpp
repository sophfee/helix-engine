#pragma once

// #define HELIX_SUPPORT_D3D12_API

#ifdef HELIX_SUPPORT_D3D12_API

#include "graphics_backend_concept.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace DX12 {
	struct SurfaceStorage {
		HWND hWnd = NULL;
		ComPtr<IDXGISurface2> pSurface;
		ComPtr<IDXGISwapChain4> pSwapChain;
		ComPtr<ID3D12Fence1> pGraphicsFence;
		UINT64 ui64FenceValue = 0llu;
		UINT uiFrameIndex = 0u;
	};
	
	struct BufferStorage {
		ComPtr<ID3D12Resource> pResource;
		PVOID pMappedData;
	};
	
	struct ImageStorage {
		ComPtr<ID3D12Resource> pResource;
	};
}

class D3D12DriverBackend final : public GraphicsBackend {
	
	HRESULT RequestAdapter(ComPtr<IDXGIAdapter1> &pAdapter) const;
	
public:
	D3D12DriverBackend();
	~D3D12DriverBackend() override;
	
	[[nodiscard]] RenderingApiBackend backend() const override { return RenderingApiBackend::eDirectX12; }
	
	void Stop() override;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	bool Init() override;
	RID CreateFence(const Optional<String> &label, bool signaled) override;
	void DestroyFence(RID fence_rid) override;
	RID CreateSemaphore(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) override;
	void DestroySemaphore(RID semaphore_rid) override;
	[[nodiscard]] RID CreateBuffer(const BufferDescriptor &desc) override;
	void DestroyBuffer(RID buffer_rid) override;
	void FlushBuffer(RID buffer_rid, ivec2 range) override;
	void SetBufferName(RID buffer_rid, const char *name) override;
	[[nodiscard]] GpuDeviceAddress GetBufferVirtualAddress(const RID buffer_rid) override;
	[[nodiscard]] void * Map(const RID buffer_rid) override;
	void Unmap(const RID buffer_rid) override;
	[[nodiscard]] void * GetMappedData(const RID buffer_rid) override;
	[[nodiscard]] RID CreateImage(const ImageDescriptor &desc) override;
	void DestroyImage(const RID image_rid) override;
	void SetImageName(RID image_rid, const char *name) override;
	[[nodiscard]] RID CreateImageView(const ImageViewDescriptor &desc) override;
	void DestroyImageView(const RID image_view_rid) override;
	[[nodiscard]] RID CreateSampler(const SamplerDescriptor &desc) override;
	void DestroySampler(const RID sampler_rid) override;
	[[nodiscard]] RID CreateSurface(Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] Vector<gfx::Format> GetFormats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format GetColorFormat(const RID surface_rid) override;
	[[nodiscard]] RID GetActiveImage(const RID surface_rid) override;
	[[nodiscard]] RID GetActiveImageView(const RID surface_rid) override;
	void UpdateSurfaceConfiguration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void DestroySurface(const RID surface_rid) override;
	[[nodiscard]] RID CreateShader(const SpirvDescriptor &spirv_descriptor) override;
	void DestroyShader(RID id) override;
	RID CreateBindGroupLayout(const BindGroupLayoutDescriptor &desc) override;
	void DestroyBindGroupLayout(const RID bind_group_layout_rid) override;
	[[nodiscard]] RID CreateBindGroup(const BindGroupDescriptor &desc) override;
	void DestroyBindGroup(const RID bind_group_rid) override;
	void UpdateBindGroup(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) override;
	[[nodiscard]] RID CreatePipelineLayout(const PipelineLayoutDescriptor &desc) override;
	void DestroyPipelineLayout(const RID pipeline_layout_rid) override;
	[[nodiscard]] RID CreateGraphicsPipeline(const GraphicsPipelineDescriptor &desc) override;
	void DestroyPipeline(const RID pipeline_rid) override;
	void PushConstants(const RID command_rid, const RID pipeline_layout_rid,
		const PushConstantRangeDescriptor &descriptor, const void *data) override;
	void BindIndexBuffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void BindVertexBuffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void BindVertexBuffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) override;
	void BindPipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	[[nodiscard]] RID Begin(RID surface_rid) override;
	uint32_t BeginRendering(RID surface_rid, const RID command_rid, const RID pipeline_rid,
		const RID depth_image_view) override;
	void FinishRendering(const RID command_rid) const override;
	void Finish(const RID command_rid) const override;
	void Transition(RID command_rid, const ImageTransitionDescriptor &descriptor) override;
	void Transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) override;
	void DrawIndexedInstanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index,
		i32 vertex_offset, u32 first_instance) override;
	void Submit(RID surface_rid, RID command_rid) override;
	void Present(RID surface_rid) override;
	void WaitForDeviceIdle() override;

private:
	ComPtr<ID3D12CommandQueue> pGraphicsQueue;
	ComPtr<ID3D12CommandAllocator> pCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> pCommandList;
	ComPtr<IDXGIFactory4> pFactory;
	ComPtr<IDXGIAdapter1> pAdapter;
	ComPtr<ID3D12Device> pDevice;
	
	SlotPool<DX12::SurfaceStorage> mSurfaceStoragePool;
	SlotPool<DX12::BufferStorage> mBufferPool;
	SlotPool<ComPtr<IDXGIResource1>> mResourcePool;
};

// static_assert(Backend<D3D12DriverBackend>, 
//               "DirectX12GraphicsDriverBackend must satisfy GraphicsBackend concept");

#endif