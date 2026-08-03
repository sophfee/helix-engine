#pragma once

#ifdef HELIX_SUPPORT_D3D12_API

#include "graphics_backend_concept.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>


using Microsoft::WRL::ComPtr;

class D3D12DriverBackend final : public GraphicsBackend {
public:
	D3D12DriverBackend();
	~D3D12DriverBackend() override;
	
	[[nodiscard]] RenderingApiBackend backend() const override { return RenderingApiBackend::eDirectX12; }
	
	bool initialize(void* window_handle);
	void shutdown() override;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;
	bool initialize() override;
	RID fence_create(const Optional<String> &label, bool signaled) override;
	void fence_delete(RID fence_rid) override;
	[[nodiscard]] vk::Fence get_fence(RID id) const override;
	RID semaphore_create(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) override;
	void semaphore_delete(RID semaphore_rid) override;
	[[nodiscard]] vk::Semaphore get_semaphore(RID id) const override;
	[[nodiscard]] RID buffer_create(const BufferDescriptor &desc) override;
	void buffer_delete(RID buffer_rid) override;
	void buffer_flush(RID buffer_rid, ivec2 range) override;
	void buffer_set_name(RID buffer_rid, const char *name) override;
	[[nodiscard]] GpuDeviceAddress buffer_virtual_address(const RID buffer_rid) override;
	[[nodiscard]] void * buffer_map(const RID buffer_rid) override;
	void buffer_unmap(const RID buffer_rid) override;
	[[nodiscard]] void * buffer_mapped_data(const RID buffer_rid) override;
	[[nodiscard]] RID image_create(const ImageDescriptor &desc) override;
	void image_delete(const RID image_rid) override;
	void image_set_name(RID image_rid, const char *name) override;
	[[nodiscard]] RID image_view_create(const ImageViewDescriptor &desc) override;
	void image_view_delete(const RID image_view_rid) override;
	[[nodiscard]] RID sampler_create(const SamplerDescriptor &desc) override;
	void sampler_delete(const RID sampler_rid) override;
	[[nodiscard]] RID surface_create(Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] Vec<gfx::Format> surface_get_formats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format surface_get_color_format(const RID surface_rid) override;
	[[nodiscard]] RID surface_get_active_image(const RID surface_rid) override;
	[[nodiscard]] RID surface_get_active_image_view(const RID surface_rid) override;
	void update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void surface_delete(const RID surface_rid) override;
	[[nodiscard]] RID shader_create(const SpirvDescriptor &spirv_descriptor) override;
	void shader_delete(RID id) override;
	RID bind_group_layout_create(const BindGroupLayoutDescriptor &desc) override;
	void bind_group_layout_delete(const RID bind_group_layout_rid) override;
	[[nodiscard]] RID bind_group_create(const BindGroupDescriptor &desc) override;
	void bind_group_delete(const RID bind_group_rid) override;
	void bind_group_update(const RID bind_group_rid, const Vec<BindGroupEntryDescriptor> &entries) override;
	[[nodiscard]] RID pipeline_layout_create(const PipelineLayoutDescriptor &desc) override;
	void pipeline_layout_delete(const RID pipeline_layout_rid) override;
	[[nodiscard]] RID pipeline_create(const GraphicsPipelineDescriptor &desc) override;
	void pipeline_delete(const RID pipeline_rid) override;
	void push_constants(const RID command_rid, const RID pipeline_layout_rid,
		const PushConstantRangeDescriptor &descriptor, const void *data) override;
	void bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void bind_vertex_buffers(const RID command_rid, const Vec<VertexBufferDescriptor> &desc) override;
	void pipeline_bind(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	[[nodiscard]] RID begin_recording(RID surface_rid) override;
	uint32_t begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid,
		const RID depth_image_view) override;
	void finish_rendering(const RID command_rid) const override;
	void finish_recording(const RID command_rid) const override;
	void transition(RID command_rid, const ImageTransitionDescriptor &descriptor) override;
	void transition(RID command_rid, const Vec<ImageTransitionDescriptor> &descriptors) override;
	void draw_indexed_instanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index,
		i32 vertex_offset, u32 first_instance) override;
	void command_submit(RID surface_rid, RID command_rid) override;
	void present(RID surface_rid) override;
	void force_wait_for_device_idle() override;

private:
	ComPtr<ID3D12CommandQueue> command_queue_;
	ComPtr<ID3D12CommandAllocator> command_allocator_;
	ComPtr<ID3D12GraphicsCommandList> command_list_;
	ComPtr<IDXGIFactory4> factory_;
	ComPtr<IDXGIAdapter1> adapter_;
};

// static_assert(Backend<D3D12DriverBackend>, 
//               "DirectX12GraphicsDriverBackend must satisfy GraphicsBackend concept");

#endif