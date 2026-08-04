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
	shutdown();
}

void D3D12DriverBackend::shutdown() {
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

bool D3D12DriverBackend::initialize() {
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

RID D3D12DriverBackend::fence_create(const Optional<String> &label, bool signaled) {
}

void D3D12DriverBackend::fence_delete(RID fence_rid) {
}

vk::Fence D3D12DriverBackend::get_fence(RID id) const {
}

RID D3D12DriverBackend::semaphore_create(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) {
}

void D3D12DriverBackend::semaphore_delete(RID semaphore_rid) {
}

vk::Semaphore D3D12DriverBackend::get_semaphore(RID id) const {
}

RID D3D12DriverBackend::buffer_create(const BufferDescriptor &desc) {
	ComPtr<ID3D12Resource> pBuffer;
	
	D3D12_HEAP_PROPERTIES mHeapProperties{
		.Type = D3D12_HEAP_TYPE_UPLOAD
	};
	
	D3D12_RESOURCE_DESC mResourceDesc{
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
	
	pDevice->CreateCommittedResource(
		&mHeapProperties,
		D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS,
		&mResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pBuffer)
	);

	SlotPool<DX12::BufferStorage>::Handle handle = mBufferPool.emplace(DX12::BufferStorage{ .pResource = pBuffer.Get(), .pMappedData = nullptr });
	return { handle.slot, handle.generation };
}

void D3D12DriverBackend::buffer_delete(RID buffer_rid) {
	assert(mBufferPool.erase(buffer_rid.upper, buffer_rid.lower));
}

void D3D12DriverBackend::buffer_flush(RID buffer_rid, ivec2 range) {
}

void D3D12DriverBackend::buffer_set_name(RID buffer_rid, const char *name) {
}

GpuDeviceAddress D3D12DriverBackend::buffer_virtual_address(const RID buffer_rid) {
	DX12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->pResource->GetGPUVirtualAddress();
}

void * D3D12DriverBackend::buffer_map(const RID buffer_rid) {
}

void D3D12DriverBackend::buffer_unmap(const RID buffer_rid) {
}

void * D3D12DriverBackend::buffer_mapped_data(const RID buffer_rid) {
	DX12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->pMappedData;
}

RID D3D12DriverBackend::image_create(const ImageDescriptor &desc) {
	
	D3D12_HEAP_PROPERTIES mHeapProperties{
		.Type = D3D12_HEAP_TYPE_DEFAULT
	};

	
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&D3D12_HEAP_PROPERTIES{ .Type = D3D12_HEAP_TYPE_DEFAULT },
		D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES,
		&D3D12_RESOURCE_DESC{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = desc.width,
			.Height = desc.height,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {.Count = 1, .Quality = 0 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		},
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&pImage)
	));

	SlotPool<DX12::BufferStorage>::Handle handle = mBufferPool.emplace(DX12::BufferStorage{ .pResource = pImage.Get(), .pMappedData = nullptr });
	return { handle.slot, handle.generation };
}

void D3D12DriverBackend::image_delete(const RID image_rid) {
}

void D3D12DriverBackend::image_set_name(RID image_rid, const char *name) {
}

RID D3D12DriverBackend::image_view_create(const ImageViewDescriptor &desc) {
}

void D3D12DriverBackend::image_view_delete(const RID image_view_rid) {
}

RID D3D12DriverBackend::sampler_create(const SamplerDescriptor &desc) {
}

void D3D12DriverBackend::sampler_delete(const RID sampler_rid) {
}

RID D3D12DriverBackend::surface_create(Window *window, const SurfaceDescriptor &desc) {
}

Vec<gfx::Format> D3D12DriverBackend::surface_get_formats(const RID surface_rid) {
}

gfx::Format D3D12DriverBackend::surface_get_color_format(const RID surface_rid) {
}

RID D3D12DriverBackend::surface_get_active_image(const RID surface_rid) {
}

RID D3D12DriverBackend::surface_get_active_image_view(const RID surface_rid) {
}

void D3D12DriverBackend::update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) {
}

void D3D12DriverBackend::surface_delete(const RID surface_rid) {
}

RID D3D12DriverBackend::shader_create(const SpirvDescriptor &spirv_descriptor) {
}

void D3D12DriverBackend::shader_delete(RID id) {
}

RID D3D12DriverBackend::bind_group_layout_create(const BindGroupLayoutDescriptor &desc) {
}

void D3D12DriverBackend::bind_group_layout_delete(const RID bind_group_layout_rid) {
}

RID D3D12DriverBackend::bind_group_create(const BindGroupDescriptor &desc) {
}

void D3D12DriverBackend::bind_group_delete(const RID bind_group_rid) {
}

void D3D12DriverBackend::bind_group_update(const RID bind_group_rid, const Vec<BindGroupEntryDescriptor> &entries) {
}

RID D3D12DriverBackend::pipeline_layout_create(const PipelineLayoutDescriptor &desc) {
}

void D3D12DriverBackend::pipeline_layout_delete(const RID pipeline_layout_rid) {
}

RID D3D12DriverBackend::pipeline_create(const GraphicsPipelineDescriptor &desc) {
}

void D3D12DriverBackend::pipeline_delete(const RID pipeline_rid) {
}

void D3D12DriverBackend::push_constants(const RID command_rid, const RID pipeline_layout_rid,
	const PushConstantRangeDescriptor &descriptor, const void *data) {
}

void D3D12DriverBackend::bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) {
}

void D3D12DriverBackend::bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) {
}

void D3D12DriverBackend::bind_vertex_buffers(const RID command_rid, const Vec<VertexBufferDescriptor> &desc) {
}

void D3D12DriverBackend::pipeline_bind(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) {
}

RID D3D12DriverBackend::begin_recording(RID surface_rid) {
}

uint32_t D3D12DriverBackend::begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid,
	const RID depth_image_view) {
}

void D3D12DriverBackend::finish_rendering(const RID command_rid) const {
}

void D3D12DriverBackend::finish_recording(const RID command_rid) const {
}

void D3D12DriverBackend::transition(RID command_rid, const ImageTransitionDescriptor &descriptor) {
}

void D3D12DriverBackend::transition(RID command_rid, const Vec<ImageTransitionDescriptor> &descriptors) {
}

void D3D12DriverBackend::draw_indexed_instanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index,
	i32 vertex_offset, u32 first_instance) {
}

void D3D12DriverBackend::command_submit(RID surface_rid, RID command_rid) {
}

void D3D12DriverBackend::present(RID surface_rid) {
}

void D3D12DriverBackend::force_wait_for_device_idle() {
}


#endif
