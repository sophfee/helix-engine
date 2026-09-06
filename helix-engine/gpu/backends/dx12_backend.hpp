#pragma once

// #define HELIX_SUPPORT_D3D12_API

#ifdef HELIX_SUPPORT_D3D12_API

#include "graphics_backend_concept.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace D3D12 {
	struct SurfaceStorage {
		HWND window_handle = NULL;
		ComPtr<IDXGISurface2> surface;
		ComPtr<IDXGISwapChain4> swap_chain;
		ComPtr<ID3D12Fence1> graphics_fence;
		UINT64 fence_value = 0llu;
		UINT frame_index = 0u;
	};
	
	struct BufferStorage {
		ComPtr<ID3D12Resource> resource;
		PVOID mapped_data;
	};
	
	struct ImageStorage {
		ComPtr<ID3D12Resource> resource;
	};
}

class D3D12DriverBackend final : public IGpuDriver {
	
	HRESULT RequestAdapter(ComPtr<IDXGIAdapter1> &pAdapter) const;
	
public:
	D3D12DriverBackend();
	~D3D12DriverBackend() override;
	
	[[nodiscard]] RenderingApiBackend backend() const override { return RenderingApiBackend::eDirectX12; }
	
	void shutdown() override;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	bool initialize() override;
	RID create_fence(const Optional<String> &label, bool signaled);
	void destroy_fence(RID fence_rid);
	
	RID create_semaphore(const gfx::SemaphoreType semaphore_type, const Optional<String> &label);
	void destroy_semaphore(RID semaphore_rid);
	
	[[nodiscard]] RID create_buffer(const BufferDescriptor &desc) override;
	void destroy_buffer(RID buffer_rid) override;
	void flush_buffer(RID buffer_rid, ivec2 range) override;
	void set_buffer_name(RID buffer_rid, const char *name) override;
	[[nodiscard]] GpuDeviceAddress get_buffer_virtual_address(const RID buffer_rid) override;
	[[nodiscard]] void * map_buffer(const RID buffer_rid) override;
	void unmap_buffer(const RID buffer_rid) override;
	[[nodiscard]] void * get_mapped_data(const RID buffer_rid) override;
	
	[[nodiscard]] RID create_image() override;
	[[nodiscard]] RID create_image(const ImageDescriptor &desc) override;
	void create_image(RID image_rid, const ImageDescriptor &desc) override;
	void destroy_image(const RID image_rid) override;
	void set_image_name(RID image_rid, const char *name) override;
	[[nodiscard]] bool is_image_valid(RID image_rid) override;
	
private:
	void create_image_view_shader_resource_view(ComPtr<ID3D12Resource1> &image_view, const ImageViewDescriptor &desc);
	void create_image_view_render_target_view(ComPtr<ID3D12Resource1> &image_view, const ImageViewDescriptor &desc);
	void create_image_view_depth_stencil_view(ComPtr<ID3D12Resource1> &image_view, const ImageViewDescriptor &desc);
	void create_image_view_unordered_access_view(ComPtr<ID3D12Resource1> &image_view, const ImageViewDescriptor &desc);
	
public:
	[[nodiscard]] RID create_image_view(const ImageViewDescriptor &desc) override;
	void destroy_image_view(const RID image_view_rid) override;
	[[nodiscard]] bool is_image_view_valid(const RID image_view_rid) override;
	
	[[nodiscard]] RID create_sampler(const SamplerDescriptor &desc) override;
	void destroy_sampler(const RID sampler_rid) override;
	
	[[nodiscard]] RID create_surface(IWindow *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID create_surface_universal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID create_surface_sdl2(SDL2Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID create_surface_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] Vector<gfx::Format> get_surface_formats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format get_surface_color_format(const RID surface_rid) override;
	[[nodiscard]] RID get_active_image(const RID surface_rid) override;
	[[nodiscard]] RID get_active_image_view(const RID surface_rid) override;
	void update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void destroy_surface(const RID surface_rid) override;
	
	[[nodiscard]] RID create_shader(const SpirvDescriptor &spirv_descriptor) override;
	void destroy_shader(RID id) override;
	
	RID create_bind_group_layout(const BindGroupLayoutDescriptor &desc) override;
	void destroy_bind_group_layout(const RID bind_group_layout_rid) override;
	
	[[nodiscard]] RID create_bind_group(const BindGroupDescriptor &desc) override;
	void destroy_bind_group(const RID bind_group_rid) override;
	void update_bind_group(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) override;
	void set_bind_group(const RID command_rid, const RID pipeline_layout_rid, u32 index, const RID bind_group_rid, gfx::ShaderStage stage) override;
	
	[[nodiscard]] RID create_pipeline_layout(const PipelineLayoutDescriptor &desc) override;
	void destroy_pipeline_layout(const RID pipeline_layout_rid) override;
	
	[[nodiscard]] RID create_graphics_pipeline(const GraphicsPipelineDescriptor &desc) override;
	void destroy_pipeline(const RID pipeline_rid) override;
	void push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) override;
	void bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) override;
	void bind_pipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	void bind_shader(RID command_rid, RID shader_rid, gfx::ShaderStage stage) override;
	void bind_shader(RID command_rid, Vector<RID> shader_rids, Vector<gfx::ShaderStage> stages) override;
	void bind_shader(RID command_rid, Vector<gfx::BindShaderDescriptor> stages) override;
	[[nodiscard]] RID begin(RID surface_rid) override;
	uint32_t begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) override;
	void FinishRendering(const RID command_rid) const override;
	void Finish(const RID command_rid) const override;
	void Transition(RID command_rid, const ImageTransitionDescriptor &descriptor) override;
	void Transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) override;
	void dispatch(RID command_rid, uvec3 groups) override;
	void dispatch(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;
	void dispatch_mesh(RID command_rid, uvec3 groups) override;
	void dispatch_mesh(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;
	void DrawIndexedInstanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) override;
	void Submit(RID surface_rid, RID command_rid) override;
	void present(RID surface_rid) override;
	void push_label(RID command_rid, const String &label) override;
	void pop_label(RID command_rid) override;
	void wait_for_idle() override;
	void yield_for_commands() override;
	
	[[nodiscard]] uint32_t queue_family(gfx::QueueFamilyType queue_family) const override;

private:
	ComPtr<ID3D12CommandQueue> pGraphicsQueue;
	ComPtr<ID3D12CommandAllocator> pCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> pCommandList;
	ComPtr<IDXGIFactory4> pFactory;
	ComPtr<IDXGIAdapter1> pAdapter;
	ComPtr<ID3D12Device> pDevice;
	
	
	ComPtr<ID3D12DescriptorHeap> pCbvSrvUavHeap;
	ComPtr<ID3D12DescriptorHeap> pSamplerHeap;
	ComPtr<ID3D12DescriptorHeap> pRtvHeap;
	ComPtr<ID3D12DescriptorHeap> pDsvHeap;
	
	SlotPool<D3D12::SurfaceStorage> mSurfaceStoragePool;
	SlotPool<D3D12::BufferStorage> mBufferPool;
	SlotPool<ComPtr<IDXGIResource1>> mResourcePool;
};

// static_assert(Backend<D3D12DriverBackend>, 
//               "DirectX12GraphicsDriverBackend must satisfy GraphicsBackend concept");

#endif