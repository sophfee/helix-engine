// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once
#include "graphics_backend_concept.hpp"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

// Forward declarations
class Window;

extern void vkResultCheckInner(VkResult result, const char* file, int line, const char* msg = nullptr);
#define vkCheck(RESULT, MESSAGE) vkResultCheckInner(RESULT, __FILE__, __LINE__, MESSAGE)

namespace vulkan {
	
	struct BufferStorage {
		vk::Buffer buffer;
		VmaAllocation allocation;
		bool is_allocated = false;
	};

	struct ImageStorage {
		vk::Image image;
		VmaAllocation allocation;
		gfx::Format format;
		u32 level_count;
	};
	
	struct ImageTransferStorage {
		VkFence fence;
		VkCommandBuffer command_buffer;
		VkCommandPool owning_pool;
	};
	
	static constexpr auto framesInFlight = 2;
	
	struct ShaderStorage {
		vk::ShaderModule shader_module;
		vk::ShaderEXT shader_ext;
	};
	
	struct SurfaceStorage {
		vk::SurfaceKHR surface;
		vk::SwapchainKHR swapchain;
		gfx::Format color_format;
		gfx::Format depth_format;
		mutable vk::Extent2D extent;
		IWindow* window;
		Vector<RID> swapchain_images;
		Vector<RID> swapchain_image_views;
		Array<RID, framesInFlight> graphics_command_buffers;
		Array<RID, framesInFlight> render_finished_semaphores;
		Array<RID, framesInFlight> image_available_semaphores;
		Array<RID, framesInFlight> graphics_fences;
		u32 frame_index = 0;
		u32 image_index = 0;
	};
	
#ifdef _DEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
#endif
}

/**
 * \brief Vulkan implementation of the graphics backend.
 */
class VkGraphicsBackend final : public GraphicsBackend {
public:
	VkGraphicsBackend();
	~VkGraphicsBackend() override;
	
	[[nodiscard]] RenderingApiBackend backend() const override { return RenderingApiBackend::eVulkan; }
	
	// Lifecycle
	bool initialize() override;
	void shutdown() override;
	void yield_for_all_commands() override;
	
	// buffer
	RID buffer_create(const BufferDescriptor &desc) override;
	void buffer_delete(RID id) override;
	void buffer_set_name(RID buffer_rid, const char *name) override;
	
	[[nodiscard]] void * buffer_map(const RID buffer_rid) override;
	void buffer_unmap(const RID buffer_rid) override;
	[[nodiscard]] void* buffer_mapped_data(RID id) override;
	[[nodiscard]] GpuDeviceAddress buffer_virtual_address(RID id) override;
	void buffer_flush(RID buffer_rid, ivec2 range) override;
	
	// image
	
	RID image_create() override;
	RID image_create(const ImageDescriptor &desc) override;
	void image_create(RID image_rid, const ImageDescriptor &desc) override;
	void image_delete(RID id) override;
	void image_set_name(RID handle, const char* name) override;
	[[nodiscard]] bool image_is_valid(RID image_rid) override;
	VkFence image_load_from_buffer(RID image_rid, RID buffer_rid, const Vector<VkBufferImageCopy2> &copy);
	VkFence image_load_from_buffer(RID image_rid, RID buffer_rid, VkBufferImageCopy2 &copy);
	
	[[nodiscard]] vk::Image get_image(RID id) const;
	[[nodiscard]] const vulkan::ImageStorage& get_image_storage(RID id) const;
	vulkan::ImageStorage &get_image_storage_mut(RID id);

	// image view
	
	RID image_view_create(const ImageViewDescriptor& desc) override;
	void image_view_delete(RID id) override;
	[[nodiscard]] bool image_view_is_valid(const RID image_view_rid) override;
	[[nodiscard]] vk::ImageView get_image_view(RID id) const;
	
	// sampler
	
	RID sampler_create(const SamplerDescriptor &desc) override;
	void sampler_delete(RID sampler) override;
	[[nodiscard]] vk::Sampler get_sampler(const RID id) const;
	
	// bind group layout (descriptor set layout)
	
	RID bind_group_layout_create(const BindGroupLayoutDescriptor &desc) override;
	void bind_group_layout_delete(RID id) override;
	[[nodiscard]] vk::DescriptorSetLayout get_bind_group_layout(RID id) const;
	
	// bind group (descriptor set)
	
	RID bind_group_create(const BindGroupDescriptor &desc) override;
	void bind_group_delete(const RID id) override;
	void bind_group_update(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) override;
	void set_bind_group(const RID command_rid, const RID pipeline_layout_rid, u32 index, const RID bind_group_rid, gfx::ShaderStage stage) override;
	[[nodiscard]] vk::DescriptorSet get_bind_group(RID id) const;
	
	// shader
	
	[[nodiscard]] RID shader_create(const SpirvDescriptor &spirv_descriptor) override;
	void shader_delete(RID id) override;
	[[nodiscard]] vk::ShaderModule get_shader_module(RID id) const;
	[[nodiscard]] vk::ShaderEXT get_shader_ext(RID id) const;
	
	// surface
	
	[[nodiscard]] RID surface_create(IWindow *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID surface_create_universal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID surface_create_sdl2(SDL2Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID surface_create_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] Vector<gfx::Format> surface_get_formats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format surface_get_color_format(const RID surface_rid) override;
	[[nodiscard]] RID surface_get_active_image(const RID surface_rid) override;
	[[nodiscard]] RID surface_get_active_image_view(const RID surface_rid) override;
	void update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void surface_delete(const RID surface_rid) override;
	[[nodiscard]] const vulkan::SurfaceStorage& get_surface_storage(RID id) const;
	[[nodiscard]] vulkan::SurfaceStorage& get_surface_storage_mut(RID id);
	
	// pipeline layout
	
	RID pipeline_layout_create(const PipelineLayoutDescriptor &desc) override;
	void pipeline_layout_delete(const RID pipeline_layout_rid) override;
	vk::PipelineLayout get_pipeline_layout(RID rid);
	
	// graphics pipeline
	
	RID graphics_pipeline_create(const GraphicsPipelineDescriptor &desc) override;
	void pipeline_bind(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	void pipeline_delete(const RID pipeline_rid) override;
	
	[[nodiscard]] vk::Pipeline get_pipeline(RID id) const;
	
	// fence
	
	RID fence_create(const Optional<String> &label = std::nullopt, bool signaled = false) override;
	void fence_delete(RID fence_rid) override;
	[[nodiscard]] vk::Fence get_fence(RID id) const override;
	
	// semaphore
	
	RID semaphore_create(const gfx::SemaphoreType semaphore_type = gfx::SemaphoreType::eBinary, const Optional<String> &label = std::nullopt) override;
	void semaphore_delete(RID semaphore_rid) override;
	[[nodiscard]] vk::Semaphore get_semaphore(RID id) const override;
	
	// commands

	[[nodiscard]] RID begin_recording(RID surface_rid) override;
	uint32_t begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) override;
	void finish_rendering(const RID command_rid) const override;
	void finish_recording(const RID command_rid) const override;
	
	void bind_shader(RID command_rid, RID shader_rid, gfx::ShaderStage stage) override;
	void bind_shader(RID command_rid, Vector<RID> shader_rids, Vector<gfx::ShaderStage> stages) override;
	void bind_shader(RID command_rid, Vector<BindShaderDescriptor> shader_descriptors) override;
	
	void transition(RID command_rid, const ImageTransitionDescriptor &descriptor) override;
	void transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) override;
	
	uint32_t queue_family(gfx::QueueFamilyType queue_family) const override;
	
	void command_submit(RID surface_rid, RID command_rid) override;
	void present(RID surface_rid) override;
	void push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) override;
	
	[[nodiscard]] vk::CommandBuffer get_command_buffer(RID id) const;
	
	void bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) override;
	void bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void draw_indexed(RID command_rid, std::uint32_t first_index, std::uint32_t index_count);
	void draw_indexed_instanced(RID command_rid, std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance) override;
	void draw_mesh_tasks(RID command_rid, uvec3 groups) override;
	void draw_mesh_tasks(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;
	
	void prune_dead_objects();
	
	// Accessors for Vulkan objects
	[[nodiscard]] vk::Instance get_instance() const { return instance_; }
	[[nodiscard]] vk::PhysicalDevice get_adapter() const { return adapter_; }
	[[nodiscard]] vk::Device get_device() const { return device_; }
	[[nodiscard]] VmaAllocator get_allocator() const { return allocator_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_graphics_queue() const { return graphics_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_compute_queue() const { return compute_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_transfer_queue() const { return transfer_queue_; }
	[[nodiscard]] std::uint32_t get_graphics_queue_index() const { return graphics_queue_family_index_; }
	[[nodiscard]] std::uint32_t get_compute_queue_index() const { return compute_queue_family_index_; }
	[[nodiscard]] std::uint32_t get_transfer_queue_index() const { return transfer_queue_family_index_; }
	[[nodiscard]] vk::CommandPool get_command_pool() const { return command_pool_; }
	[[nodiscard]] vk::CommandPool get_transfer_command_pool() const { return transfer_command_pool_; }
	[[nodiscard]] vk::DescriptorPool get_descriptor_pool() const { return descriptor_pool_; }

	[[nodiscard]] vk::Buffer get_buffer(RID id);
	[[nodiscard]] VmaAllocation get_buffer_allocation(RID id);
	[[nodiscard]] VmaAllocationInfo get_buffer_allocation_info(RID id);
	void force_wait_for_device_idle() override;
	
	void prune();
	
private:
	void create_instance();
	void request_adapter();
	void create_device_and_queues();
	void create_allocator();
	void create_default_pools();

public:
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

private:
	// Vulkan state - all Vulkan-specific data lives here
	Vector<Window*> windows_;
	
	vk::Instance instance_;
	vk::PhysicalDevice adapter_;
	vk::Device device_;
#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debug_messenger_;
	
	struct {
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ nullptr };
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ nullptr };
		PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT{ nullptr };
		PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT{ nullptr };
		PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT{ nullptr };
		PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT{ nullptr };
		PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT{ nullptr };
		PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT{ nullptr };
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{ nullptr };
		PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasks{nullptr};
		PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirect{nullptr};
		PFN_vkCreateShadersEXT vkCreateShaders{nullptr};
		PFN_vkDestroyShaderEXT vkDestroyShader{nullptr};
		PFN_vkCmdBindShadersEXT vkCmdBindShaders{nullptr};
	} ext;
	
	void load_instance_extension_functions();
	void load_device_extension_functions();
	
	VkResult vkCreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
	void vkDestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);
	void vkCmdBeginDebugUtilsLabel(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo);
	void vkCmdInsertDebugUtilsLabel(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo);
	void vkCmdEndDebugUtilsLabel(VkCommandBuffer commandBuffer);
	void vkQueueBeginDebugUtilsLabel(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo);
	void vkQueueInsertDebugUtilsLabel(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo);
	void vkQueueEndDebugUtilsLabel(VkQueue queue);
	VkResult vkSetDebugUtilsObjectName(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo);
	void vkCmdDrawMeshTasks(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
	void vkCmdDrawMeshTasksIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
	VkResult vkCreateShaders(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders);
	void vkDestroyShader(VkDevice device, VkShaderEXT shader, const VkAllocationCallbacks* pAllocator);
	void vkCmdBindShaders(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders);
#endif
	std::uint64_t allocations_ = 0;
	
	SlotPool<vulkan::BufferStorage> buffers_;
	SlotPool<vulkan::ImageStorage> images_;
	SlotPool<vk::ImageView> image_views_;
	SlotPool<vk::Sampler> samplers_;
	SlotPool<vulkan::ShaderStorage> shader_modules_;
	SlotPool<vk::CommandPool> command_pools_;
	SlotPool<vk::Fence> fences_;
	SlotPool<vk::Semaphore> semaphores_;
	SlotPool<vk::CommandBuffer> command_buffers_;
	SlotPool<vk::DescriptorSetLayout> descriptor_set_layouts_;
	SlotPool<vk::DescriptorSet> descriptor_sets_;
	SlotPool<vk::Pipeline> pipelines_;
	SlotPool<vk::PipelineLayout> pipeline_layouts_;
	SlotPool<vulkan::SurfaceStorage> surfaces_;
	
public:
	Mutex allocation_mutex_;
private:
	Mutex transfer_mutex_;
	Vector<vulkan::ImageTransferStorage> image_transfers_;
	
	
	Vector<vk::Queue> graphics_queue_;
	Vector<vk::Queue> compute_queue_;
	Vector<vk::Queue> transfer_queue_;
	std::atomic_uint64_t transfer_queue_to_use = 0;

	struct DeadCommandBuffer {
		vk::CommandBuffer command_buffer;
		vk::Fence fence;
	};
	Vector<SharedPtr<DeadCommandBuffer>> dead_command_buffers_;
	
	// All pools must be accessed in a synchronized fashion,
	vk::CommandPool command_pool_;
	
	Vector<vk::CommandPool> transfer_command_pools_; //< To clean up.
	inline static thread_local VkCommandPool transfer_command_pool_ = VK_NULL_HANDLE;
	
	vk::DescriptorPool descriptor_pool_;
	
	VmaAllocator allocator_;
	u32 graphics_queue_family_index_;
	u32 compute_queue_family_index_;
	u32 transfer_queue_family_index_;
	u32 current_graphics_queue = 0;
	bool deleted_ = false;
};

// Verify that VulkanGraphicsDriverBackend satisfies the GraphicsBackend concept
//static_assert(Backend<VkGraphicsBackend>, 
//              "VulkanGraphicsDriverBackend must satisfy GraphicsBackend concept");
