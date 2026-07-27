#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "../types.hpp"
#include "engine/rid.hpp"

class Window;

class GraphicsDriverThreadLocalStorage {
	vk::CommandPool command_pool;
	vk::DescriptorPool descriptor_pool_;
};

struct BufferStorage {
	vk::Buffer buffer;
	VmaAllocation allocation;
	bool is_allocated = false;
};

struct ImageStorage {
	vk::Image image;
	VmaAllocation allocation;
};

struct PipelineStorage {
	vk::PipelineLayout layout;
	vk::Pipeline pipeline;
	std::vector<vk::DescriptorSetLayout> set_layouts;
};

/**
 *\brief Contains all relevant information that needs to persist in one contiguous state.
 */
class GraphicsDriver {
protected:
	void createInstance();
	void requestAdapter();
	void createDeviceAndQueues();
	void createAllocator();
	void createPools();

public:
	GraphicsDriver();
	~GraphicsDriver();

	void start();
	void stop();

	static GraphicsDriver *singleton();

	// TODO: Make a generic interchange format, I would like to make this class as API agnostic as possible. Luckily, the same people that make VMA make DX12MA so they likely share many of the same semantics.
	[[nodiscard]] RID buffer_create(const VkBufferCreateInfo &buffer_create_info,
	                                const VmaAllocationCreateInfo &allocation_info);
	void buffer_delete(RID id);
	
	void buffer_set_allocation_name(RID handle, const char* name);
	
	[[nodiscard]] bool buffer_exists(RID id) const;
	[[nodiscard]] VmaAllocationInfo buffer_get_allocation_info(RID id);
	[[nodiscard]] void *buffer_get_mapped_address(RID id);
	[[nodiscard]] vk::DeviceAddress buffer_get_device_address(RID id);

	[[nodiscard]] RID image_create(const VkImageCreateInfo &image_create_info,
	                               const VmaAllocationCreateInfo &allocation_info);
	void image_load_from_buffer(RID image, RID buffer, const vk::BufferImageCopy &copy);
	void image_load_from_buffer(RID image, RID buffer, const Vec<vk::BufferImageCopy> &copy);
	void image_delete(RID id);

	[[nodiscard]] RID image_view_create(RID image, vk::ImageViewCreateInfo &image_view_create_info);
	/// \brief alternative way to create image_views if the image is not an engine image but rather a native image (i.e. swapchain images)
	[[nodiscard]] RID image_view_create(const vk::ImageViewCreateInfo &image_view_create_info);
	void image_view_delete(RID id);

	[[nodiscard]] RID sampler_create(const vk::SamplerCreateInfo &sampler_create_info);
	void sampler_delete(RID sampler);

	[[nodiscard]] RID shader_create();
	void shader_delete(RID id);
	void shader_load_spirv_from_file(RID shader_module, std::string_view file_path);
	void shader_upload_spirv(RID shader_module, Vec<u32> spirv);
	void shader_upload_spirv(RID shader_module, const u32 *spirv, size_t spirv_length);

	void prune_dead_objects();

	RID start_recording(Window *render_target);
	void start_rendering(const Window *render_target, RID cmd_rid, RID pipeline_rid);
	void stop_rendering(Window *render_target, RID cmd_rid);
	void stop_recording(const Window *render_target, RID cmd_rid);
	void submit(const Window *render_target, RID cmd_rid);
	void present(Window *render_target);
	void push_constants(RID command_rid, RID pipeline_rid, vk::ShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data);
	void bind_index_buffer(RID command_rid, RID buffer_rid, vk::IndexType index_type = vk::IndexType::eUint16, vk::DeviceSize offset = 0);
	void bind_vertex_buffer(RID command_rid, RID buffer_rid);
	void bind_vertex_buffers(RID command_rid, size_t buffer_count, const RID* buffer_rids);
	
	void draw_indexed(RID command_rid, uint32_t start, uint32_t count);
	
	template <size_t N> void bind_vertex_buffers(const RID command_rid, RID (&buffer_rids)[N]) { bind_vertex_buffers(command_rid, N, buffer_rids); }

	RID pipeline_create(Window *surface, RID shader);
	void pipeline_delete(RID pipeline_rid);
	void pipeline_bind(RID pipeline, RID cmd_rid, vk::PipelineBindPoint bind_point);
	void pipeline_bind_descriptor_sets(RID pipeline, RID cmd_rid, vk::PipelineBindPoint bind_point, uint32_t first = 0, uint32_t set_count = 0, const RID *descriptor_set_rids = nullptr, uint32_t offset_count = 0, const uint32_t* offsets = nullptr);
	
	
	
	template <size_t N> void pipeline_bind_descriptor_sets(const RID pipeline, const RID cmd_rid, const vk::PipelineBindPoint bind_point, const uint32_t first, std::array<RID, N> const &descriptor_sets) {
		pipeline_bind_descriptor_sets(pipeline, cmd_rid, bind_point, first, N, descriptor_sets.data(), 0, nullptr);
	}
	template <size_t N0, size_t N1> 
	void pipeline_bind_descriptor_sets(const RID pipeline, const RID cmd_rid, const vk::PipelineBindPoint bind_point, 
		const uint32_t first, std::array<RID, N0> const &descriptor_sets, std::array<uint32_t, N1> const &offsets) {
		pipeline_bind_descriptor_sets(pipeline, cmd_rid, bind_point, first, N0, descriptor_sets.data(), N1, offsets.data());
	}
	
	RID descriptor_set_create(RID pipeline, uint32_t set_index);
	void descriptor_set_write(RID descriptor_set_rid, uint32_t binding, RID buffer_rid);
	
	RID fence_create(bool signaled = false);
	void fence_delete(RID fence_rid);
	
	RID semaphore_create();
	void semaphore_delete(RID semaphore_rid);
private:
	
	const BufferStorage &_buffer_storage_inner(RID handle) const;
	BufferStorage &_buffer_storage_inner(RID handle);
	vk::Buffer _buffer_inner(RID handle) const;

public:
	RID pipeline_layout_create();
	RID command_pool_create();

private:
	Vec<Window*> windows;

	vk::Instance instance;
	vk::PhysicalDevice adapter;
	vk::Device device;

	std::uint32_t buffers_made = 0;
	std::uint32_t buffer_views_made = 0;
	std::uint32_t images_made = 0;
	std::uint32_t image_views_made = 0;
	std::uint32_t samplers_made = 0;
	std::uint32_t shader_modules_made = 0;
	std::uint32_t command_pools_made = 0;
	std::uint32_t pipelines_made = 0;
	std::uint32_t descriptor_sets_made = 0;
	std::uint32_t fences_made = 0;
	std::uint32_t semaphores_made = 0;
	std::uint32_t command_buffers_made = 0;
	std::uint32_t allocations = 0;

	Vec<BufferStorage> buffers;
	UnorderedMap<u64, ImageStorage> images;
	UnorderedMap<u64, vk::ImageView> image_views;
	UnorderedMap<u64, vk::Sampler> samplers;
	UnorderedMap<u64, vk::ShaderModule> shader_modules;

	UnorderedMap<u64, vk::CommandPool> command_pools;
	UnorderedMap<u64, PipelineStorage> pipelines;
	UnorderedMap<u64, vk::DescriptorSet> sets;
	UnorderedMap<u64, vk::Fence> fences;
	UnorderedMap<u64, vk::Semaphore> semaphores;
	UnorderedMap<u64, vk::CommandBuffer> command_buffers;

	std::uint32_t graphics_queue_index;
	std::uint32_t compute_queue_index;
	std::uint32_t transfer_queue_index;
	std::vector<vk::Queue> graphics_queue;
	std::vector<vk::Queue> compute_queue;
	std::vector<vk::Queue> transfer_queue; //< dedicated transfer queue

	struct DeadCommandBuffer {
		vk::CommandBuffer command_buffer;
		vk::Fence fence;
	};

	Vec<SharedPtr<DeadCommandBuffer>> dead_command_buffers;

	vk::CommandPool command_pool;
	vk::CommandPool transfer_command_pool;

	
	vk::DescriptorPool descriptor_pool;

	VmaAllocator allocator;

public:
	friend class Window;
};
