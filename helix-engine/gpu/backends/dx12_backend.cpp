// ReSharper disable CppVariableCanBeMadeConstexpr
#include "dx12_backend.hpp"

#ifdef HELIX_SUPPORT_D3D12_API

#include <cassert>
#include <stdexcept>
#include "detail/DXHelper.h"

#include <d3d12.h>

template <typename TEnum>
[[nodiscard]] static bool has_flag(const TEnum value, const TEnum flag) {
	using RawType = std::underlying_type_t<TEnum>;
	return (static_cast<RawType>(value) & static_cast<RawType>(flag)) != 0;
}

HRESULT D3D12DriverBackend::RequestAdapter(ComPtr<IDXGIAdapter1> &pAdapter) const {
	if (SUCCEEDED(pFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter)))) {
		return S_OK;
	}
	
	ComPtr<IDXGIAdapter1> pBestFitAdapter;
	FLOAT fBestFitAdapterScore = 0.0F;
	
	for (UINT uiAdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(uiAdapterIndex, &pAdapter); ++uiAdapterIndex) {
		DXGI_ADAPTER_DESC1 mDesc1;
		ThrowIfFailed(pAdapter->GetDesc1(&mDesc1));

		FLOAT fAdapterScore = 0.0F;

		fAdapterScore += static_cast<FLOAT>(mDesc1.DedicatedVideoMemory) * 1000.0F;

		if (!(mDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
			fAdapterScore *= 100.0F;

		if (fAdapterScore > fBestFitAdapterScore) {
			fBestFitAdapterScore = fAdapterScore;
			pBestFitAdapter = pAdapter;
		}
	}

	if (pBestFitAdapter) {
		pAdapter = pBestFitAdapter;
		return S_OK;
	}

	return E_FAIL;
}

D3D12DriverBackend::D3D12DriverBackend() = default;

D3D12DriverBackend::~D3D12DriverBackend() {
	Stop();
}

void D3D12DriverBackend::Stop() {
	dispose();
}

void D3D12DriverBackend::dispose() {
	if (disposed()) return;
	pCommandList.Reset();
	pCommandAllocator.Reset();
	pGraphicsQueue.Reset();
	pDevice.Reset();
	pAdapter.Reset();
	pFactory.Reset();
}

bool D3D12DriverBackend::disposed() const {
	return pFactory == nullptr;
}

bool D3D12DriverBackend::Init() {
	UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory)));
	ThrowIfFailed(RequestAdapter(pAdapter));
	ThrowIfFailed(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pGraphicsQueue)));
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&pCommandAllocator)));
	ThrowIfFailed(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pCommandList)));
	ThrowIfFailed(pCommandList->Close());

	return true;
}

RID D3D12DriverBackend::CreateFence(const Optional<String> &label, bool signaled) {
}

void D3D12DriverBackend::DestroyFence(RID fence_rid) {
}

vk::Fence D3D12DriverBackend::GetFence(RID id) const {
}

RID D3D12DriverBackend::CreateSemaphore(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) {
}

void D3D12DriverBackend::DestroySemaphore(RID semaphore_rid) {
}

vk::Semaphore D3D12DriverBackend::GetSemaphore(RID id) const {
}

RID D3D12DriverBackend::CreateBuffer(const BufferDescriptor &desc) {
	ComPtr<ID3D12Resource> pBuffer;

	const D3D12_HEAP_PROPERTIES mHeapProperties{
		.Type = D3D12_HEAP_TYPE_UPLOAD
	};

	const D3D12_RESOURCE_DESC mResourceDesc{
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		.Alignment = 0,
		.Width = desc.size,
		.Height = 1,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = {.Count = 1, .Quality = 0 },
		.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		.Flags = D3D12_RESOURCE_FLAG_NONE
	};
	
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&mHeapProperties,
		D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS,
		&mResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pBuffer)
	));

	SlotPool<DX12::BufferStorage>::Handle handle = mBufferPool.emplace(DX12::BufferStorage{ .pResource = pBuffer.Get(), .pMappedData = nullptr });
	return { handle.slot, handle.generation };
}

void D3D12DriverBackend::DestroyBuffer(RID buffer_rid) {
	
	assert(mBufferPool.erase(buffer_rid.upper, buffer_rid.lower));
}

void D3D12DriverBackend::FlushBuffer(RID buffer_rid, ivec2 range) {
}

void D3D12DriverBackend::SetBufferName(RID buffer_rid, const char *name) {
}

GpuDeviceAddress D3D12DriverBackend::GetBufferVirtualAddress(const RID buffer_rid) {
	DX12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->pResource->GetGPUVirtualAddress();
}

void * D3D12DriverBackend::Map(const RID buffer_rid) {
}

void D3D12DriverBackend::Unmap(const RID buffer_rid) {
}

void * D3D12DriverBackend::GetMappedData(const RID buffer_rid) {
	DX12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->pMappedData;
}

RID D3D12DriverBackend::CreateImage(const ImageDescriptor &desc) {
	
	D3D12_HEAP_PROPERTIES heapProperties{
		.Type = D3D12_HEAP_TYPE_DEFAULT
	};
	
	D3D12_RESOURCE_DESC resourceDesc{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Alignment = 0,
		.Width = desc.size.x,
		.Height = desc.size.y,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = {.Count = 1, .Quality = 0 },
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_NONE
	};
	
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&pImage)
	));

	SlotPool<DX12::BufferStorage>::Handle handle = mBufferPool.emplace(DX12::BufferStorage{ .pResource = pImage.Get(), .pMappedData = nullptr });
	return { handle.slot, handle.generation };
}

void D3D12DriverBackend::DestroyImage(const RID image_rid) {
}

void D3D12DriverBackend::SetImageName(RID image_rid, const char *name) {
}

RID D3D12DriverBackend::CreateImageView(const ImageViewDescriptor &desc) {
}

void D3D12DriverBackend::DestroyImageView(const RID image_view_rid) {
}

RID D3D12DriverBackend::CreateSampler(const SamplerDescriptor &desc) {
}

void D3D12DriverBackend::DestroySampler(const RID sampler_rid) {
}

RID D3D12DriverBackend::CreateSurface(Window *window, const SurfaceDescriptor &desc) {
}

Vector<gfx::Format> D3D12DriverBackend::GetFormats(const RID surface_rid) {
}

gfx::Format D3D12DriverBackend::GetColorFormat(const RID surface_rid) {
}

RID D3D12DriverBackend::GetActiveImage(const RID surface_rid) {
}

RID D3D12DriverBackend::GetActiveImageView(const RID surface_rid) {
}

void D3D12DriverBackend::UpdateSurfaceConfiguration(const RID surface_rid, const SurfaceDescriptor &desc) {
}

void D3D12DriverBackend::DestroySurface(const RID surface_rid) {
}

RID D3D12DriverBackend::CreateShader(const SpirvDescriptor &spirv_descriptor) {
}

void D3D12DriverBackend::DestroyShader(RID id) {
}

RID D3D12DriverBackend::CreateBindGroupLayout(const BindGroupLayoutDescriptor &desc) {
}

void D3D12DriverBackend::DestroyBindGroupLayout(const RID bind_group_layout_rid) {
}

RID D3D12DriverBackend::CreateBindGroup(const BindGroupDescriptor &desc) {
}

void D3D12DriverBackend::DestroyBindGroup(const RID bind_group_rid) {
}

void D3D12DriverBackend::UpdateBindGroup(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) {
}

RID D3D12DriverBackend::CreatePipelineLayout(const PipelineLayoutDescriptor &desc) {
}

void D3D12DriverBackend::DestroyPipelineLayout(const RID pipeline_layout_rid) {
}

RID D3D12DriverBackend::CreateGraphicsPipeline(const GraphicsPipelineDescriptor &desc) {
}

void D3D12DriverBackend::DestroyPipeline(const RID pipeline_rid) {
}

void D3D12DriverBackend::PushConstants(const RID command_rid, const RID pipeline_layout_rid,
	const PushConstantRangeDescriptor &descriptor, const void *data) {
}

void D3D12DriverBackend::BindIndexBuffer(const RID command_rid, const IndexBufferDescriptor &desc) {
}

void D3D12DriverBackend::BindVertexBuffer(const RID command_rid, const VertexBufferDescriptor &desc) {
}

void D3D12DriverBackend::BindVertexBuffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) {
}

void D3D12DriverBackend::BindPipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) {
}

RID D3D12DriverBackend::Begin(RID surface_rid) {
}

uint32_t D3D12DriverBackend::BeginRendering(RID surface_rid, const RID command_rid, const RID pipeline_rid,
	const RID depth_image_view) {
}

void D3D12DriverBackend::FinishRendering(const RID command_rid) const {
}

void D3D12DriverBackend::Finish(const RID command_rid) const {
}

void D3D12DriverBackend::Transition(RID command_rid, const ImageTransitionDescriptor &descriptor) {
}

void D3D12DriverBackend::Transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) {
}

void D3D12DriverBackend::DrawIndexedInstanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index,
	i32 vertex_offset, u32 first_instance) {
}

void D3D12DriverBackend::Submit(RID surface_rid, RID command_rid) {
}

void D3D12DriverBackend::Present(RID surface_rid) {
}

void D3D12DriverBackend::WaitForDeviceIdle() {
}


#endif
