// ReSharper disable CppVariableCanBeMadeConstexpr
#include "dx12_backend.hpp"

#ifdef HELIX_SUPPORT_D3D12_API

#include <cassert>
#include <stdexcept>
#include "detail/DXHelper.h"

#include <d3d12.h>

#include "dx12_enums.hpp"
#include "gpu/window.hpp"

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

	const D3D12_DESCRIPTOR_HEAP_DESC heapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1000,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0
	};
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pCbvSrvUavHeap)));

	const D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		.NumDescriptors = 100,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 0
	};
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&pSamplerHeap)));

	const D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = 100,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pRtvHeap)));

	const D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		.NumDescriptors = 100,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pDsvHeap)));

	return true;
}

RID D3D12DriverBackend::create_fence(const Optional<String> &label, bool signaled) {
}

void D3D12DriverBackend::destroy_fence(RID fence_rid) {
}

vk::Fence D3D12DriverBackend::GetFence(RID id) const {
}

RID D3D12DriverBackend::create_semaphore(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) {
}

void D3D12DriverBackend::destroy_semaphore(RID semaphore_rid) {
}

vk::Semaphore D3D12DriverBackend::GetSemaphore(RID id) const {
}

RID D3D12DriverBackend::create_buffer(const BufferDescriptor &desc) {
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

	SlotPool<D3D12::BufferStorage>::Handle handle = mBufferPool.emplace(D3D12::BufferStorage{ .resource = pBuffer.Get(), .mapped_data = nullptr });
	return { handle.slot, handle.generation };
}

void D3D12DriverBackend::destroy_buffer(RID buffer_rid) {
	
	assert(mBufferPool.erase(buffer_rid.upper, buffer_rid.lower));
}

void D3D12DriverBackend::flush_buffer(RID buffer_rid, ivec2 range) {
}

void D3D12DriverBackend::set_buffer_name(RID buffer_rid, const char *name) {
}

GpuDeviceAddress D3D12DriverBackend::get_buffer_virtual_address(const RID buffer_rid) {
	D3D12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->resource->GetGPUVirtualAddress();
}

void * D3D12DriverBackend::map_buffer(const RID buffer_rid) {
}

void D3D12DriverBackend::unmap_buffer(const RID buffer_rid) {
}

void * D3D12DriverBackend::get_mapped_data(const RID buffer_rid) {
	D3D12::BufferStorage* pBuffer = mBufferPool.get(buffer_rid.upper, buffer_rid.lower);
	return pBuffer->mapped_data;
}

RID D3D12DriverBackend::create_image() {
	return mResourcePool.emplace(ComPtr<IDXGIResource1>());
}

RID D3D12DriverBackend::create_image(const ImageDescriptor &desc) {
	const RID ridImage = create_image();
	create_image(ridImage, desc);
	return ridImage;
}

void D3D12DriverBackend::create_image(const RID image_rid, const ImageDescriptor &desc) {
	ComPtr<IDXGIResource1> *pImage = mResourcePool.get(image_rid.upper, image_rid.lower);
	
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
		IID_PPV_ARGS(&*pImage)
	));
}

void D3D12DriverBackend::destroy_image(const RID image_rid) {
	mResourcePool.get(image_rid.upper, image_rid.lower)->Reset();
}

void D3D12DriverBackend::set_image_name(RID image_rid, const char *name) {
}

void D3D12DriverBackend::create_image_view_shader_resource_view(ComPtr<ID3D12Resource1> &image_view, const ImageViewDescriptor &desc) {
	D3D12_CPU_DESCRIPTOR_HANDLE srvHeap = pDevice->GetCPU
	pDevice->CreateShaderResourceView()
}

RID D3D12DriverBackend::create_image_view(const ImageViewDescriptor &desc) {
	ComPtr<IDXGIResource1> pImageView;
	switch (desc.usage.value_or(gfx::ImageUsage::eSampled)) {
		case gfx::ImageUsage::eSampled:
			pImageView = CreateImageViewShaderResourceView(desc);
			break;
		case gfx::ImageUsage::eColorAttachment:
			pImageView = CreateImageViewRenderTargetView(desc);
			break;
		case gfx::ImageUsage::eDepthStencilAttachment:
			pImageView = CreateImageViewDepthStencilView(desc);
			break;
		case gfx::ImageUsage::eStorage:
			pImageView = CreateImageViewUnorderedAccessView(desc);
			break;
		default:
			throw std::runtime_error("Unsupported image usage for image view creation.");
	}
}

void D3D12DriverBackend::destroy_image_view(const RID image_view_rid) {
}

RID D3D12DriverBackend::create_sampler(const SamplerDescriptor &desc) {
	D3D12_SAMPLER_DESC mSamplerDesc{
		.Filter = D3D12::Filter(desc.mag_filter.value_or(gfx::Filter::eLinear)),
		.AddressU = D3D12::AddressMode(desc.address_mode_u.value_or(gfx::AddressMode::eRepeat)),
		.AddressV = D3D12::AddressMode(desc.address_mode_v.value_or(gfx::AddressMode::eRepeat)),
		.AddressW = D3D12::AddressMode(desc.address_mode_w.value_or(gfx::AddressMode::eRepeat)),
		.MipLODBias = desc.mip_lod_bias,
		.MaxAnisotropy = static_cast<UINT>(desc.max_anisotropy),
		.ComparisonFunc = D3D12::CompareOp(desc.compare_op.value_or(gfx::CompareOp::eAlways)),
		.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f },
		.MinLOD = 0.0f,
		.MaxLOD = 1.0f
	};
	const D3D12_CPU_DESCRIPTOR_HANDLE pSamplerHandle = pSamplerHeap->GetCPUDescriptorHandleForHeapStart();
	pDevice->CreateSampler(&mSamplerDesc, pSamplerHandle);
}

void D3D12DriverBackend::destroy_sampler(const RID sampler_rid) {
}

RID D3D12DriverBackend::create_surface(IWindow *window, const SurfaceDescriptor &desc) {
	const HWND hWindow = window->windowHandle();
	ThrowIfFailed(pFactory->MakeWindowAssociation(hWindow, DXGI_MWA_NO_ALT_ENTER));

	const uint2 vWindowSize = window->size();
	
	DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc{
		.Width = vWindowSize.x,
		.Height = vWindowSize.y,
		.Format = D3D12::Format(desc.format.value_or(gfx::Format::eRgba8Unorm)),
		.Stereo = 0,
		.SampleDesc = DXGI_SAMPLE_DESC{
			.Count = 1U,
			.Quality = 0U,
		},
		.BufferUsage = 0,
		.BufferCount = 0,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_IGNORE,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};
	ComPtr<IDXGISwapChain1> pSwapChain;
	ThrowIfFailed(pFactory->CreateSwapChainForHwnd(
		pGraphicsQueue.Get(),
		hWindow,
		&mSwapChainDesc,
		nullptr,
		nullptr,
		&pSwapChain
	));
	ComPtr<IDXGISwapChain4> pSwapChain4;
	pSwapChain4.As(&pSwapChain);
}

Vector<gfx::Format> D3D12DriverBackend::GetFormats(const RID surface_rid) {
}

gfx::Format D3D12DriverBackend::GetColorFormat(const RID surface_rid) {
}

RID D3D12DriverBackend::get_active_image(const RID surface_rid) {
}

RID D3D12DriverBackend::get_active_image_view(const RID surface_rid) {
}

void D3D12DriverBackend::update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) {
}

void D3D12DriverBackend::destroy_surface(const RID surface_rid) {
}

RID D3D12DriverBackend::create_shader(const SpirvDescriptor &spirv_descriptor) {
}

void D3D12DriverBackend::destroy_shader(RID id) {
}

RID D3D12DriverBackend::create_bind_group_layout(const BindGroupLayoutDescriptor &desc) {
}

void D3D12DriverBackend::destroy_bind_group_layout(const RID bind_group_layout_rid) {
}

RID D3D12DriverBackend::create_bind_group(const BindGroupDescriptor &desc) {
}

void D3D12DriverBackend::destroy_bind_group(const RID bind_group_rid) {
}

void D3D12DriverBackend::update_bind_group(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) {
}

RID D3D12DriverBackend::create_pipeline_layout(const PipelineLayoutDescriptor &desc) {
}

void D3D12DriverBackend::destroy_pipeline_layout(const RID pipeline_layout_rid) {
}

RID D3D12DriverBackend::create_graphics_pipeline(const GraphicsPipelineDescriptor &desc) {
}

void D3D12DriverBackend::destroy_pipeline(const RID pipeline_rid) {
}

void D3D12DriverBackend::push_constants(const RID command_rid, const RID pipeline_layout_rid,
	const PushConstantRangeDescriptor &descriptor, const void *data) {
}

void D3D12DriverBackend::bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) {
}

void D3D12DriverBackend::bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) {
}

void D3D12DriverBackend::bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) {
}

void D3D12DriverBackend::bind_pipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) {
}

RID D3D12DriverBackend::begin(RID surface_rid) {
}

uint32_t D3D12DriverBackend::begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid,
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

void D3D12DriverBackend::present(RID surface_rid) {
}

void D3D12DriverBackend::wait_for_idle() {
}


#endif
