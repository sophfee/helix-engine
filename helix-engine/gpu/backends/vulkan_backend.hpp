// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once
#include "graphics_backend_concept.hpp"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

// Forward declarations
class Window;

#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
	printf((format), __VA_ARGS__); \
	printf("\n"); \
} while(false)

#define VMA_LEAK_LOG_FORMAT VMA_DEBUG_LOG_FORMAT

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
		gfx::ImageLayout layout;
		BitFlag<gfx::Access> access;
		BitFlag<gfx::PipelineStage> stage;
		u32 level_count;
	};
	struct ImageViewStorage {
		vk::ImageView image_view;
		RID image;
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
	
	struct CommandBufferStorage {
		vk::CommandBuffer command_buffer;
		Optional<RID> connected_surface;
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
	void yield_for_commands() override;
	
	void initialize_im_gui();
	
	// fence
	RID create_fence(const Optional<String> &label = std::nullopt, bool signaled = false);
	void destroy_fence(RID fence_rid);
	[[nodiscard]] vk::Fence get_fence(RID id) const;
	
	// semaphore
	RID create_semaphore(const gfx::SemaphoreType semaphore_type = gfx::SemaphoreType::eBinary, const Optional<String> &label = std::nullopt);
	void destroy_semaphore(RID semaphore_rid);
	[[nodiscard]] vk::Semaphore get_semaphore(RID id) const;
	
	// buffer
	RID create_buffer(const BufferDescriptor &desc) override;
	void destroy_buffer(RID id) override;
	void set_buffer_name(RID buffer_rid, const char *name) override;
	[[nodiscard]] void * map_buffer(const RID buffer_rid) override;
	void unmap_buffer(const RID buffer_rid) override;
	[[nodiscard]] void* get_mapped_data(RID id) override;
	[[nodiscard]] GpuDeviceAddress get_buffer_virtual_address(RID id) override;
	void flush_buffer(RID buffer_rid, ivec2 range) override;
	[[nodiscard]] vk::Buffer get_buffer(RID id);
	[[nodiscard]] VmaAllocation get_buffer_allocation(RID id);
	[[nodiscard]] VmaAllocationInfo get_buffer_allocation_info(RID id);
	
	// image
	RID create_image() override;
	RID create_image(const ImageDescriptor &desc) override;
	void create_image(RID image_rid, const ImageDescriptor &desc) override;
	void destroy_image(RID id) override;
	void set_image_name(RID handle, const char* name) override;
	[[nodiscard]] bool is_image_valid(RID image_rid) override;
	VkFence load_image_from_buffer(RID image_rid, RID buffer_rid, const Vector<VkBufferImageCopy2> &copy);
	VkFence load_image_from_buffer(RID image_rid, RID buffer_rid, VkBufferImageCopy2 &copy);
	[[nodiscard]] vk::Image get_image(RID id) const;
	[[nodiscard]] const vulkan::ImageStorage& get_image_storage(RID id) const;
	vulkan::ImageStorage &get_image_storage_mutable(RID id);

	// image view
	RID create_image_view(const ImageViewDescriptor& desc) override;
	void destroy_image_view(RID id) override;
	[[nodiscard]] bool is_image_view_valid(const RID image_view_rid) override;
	[[nodiscard]] vk::ImageView get_image_view(RID id) const;
	
	// sampler
	RID create_sampler(const SamplerDescriptor &desc) override;
	void destroy_sampler(RID sampler) override;
	[[nodiscard]] vk::Sampler get_sampler(const RID id) const;
	
	// bind group layout (descriptor set layout)
	RID create_bind_group_layout(const BindGroupLayoutDescriptor &desc) override;
	void destroy_bind_group_layout(RID id) override;
	[[nodiscard]] vk::DescriptorSetLayout get_bind_group_layout(RID id) const;
	
	// bind group (descriptor set)
	RID create_bind_group(const BindGroupDescriptor &desc) override;
	void destroy_bind_group(const RID id) override;
	void update_bind_group(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) override;
	void set_bind_group(const RID command_rid, const RID pipeline_layout_rid, u32 index, const RID bind_group_rid, gfx::ShaderStage stage) override;
	[[nodiscard]] vk::DescriptorSet get_bind_group(RID id) const;
	
	// shader
	
	[[nodiscard]] RID create_shader(const SpirvDescriptor &spirv_descriptor) override;
	void destroy_shader(RID id) override;
	[[nodiscard]] vk::ShaderModule get_shader_module(RID id) const;
	[[nodiscard]] vk::ShaderEXT get_shader(RID id) const;
	
	// surface
	[[nodiscard]] RID create_surface(IWindow *window, const SurfaceDescriptor &desc) override;
private:
	// helpers
	[[nodiscard]] RID create_surface_universal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc);
	[[nodiscard]] RID create_surface_sdl2(SDL2Window *window, const SurfaceDescriptor &desc);
	[[nodiscard]] RID create_surface_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc);
public:
	// more surface
	[[nodiscard]] Vector<gfx::Format> get_surface_formats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format get_surface_color_format(const RID surface_rid) override;
	[[nodiscard]] RID get_active_image(const RID surface_rid) override;
	[[nodiscard]] RID get_active_image_view(const RID surface_rid) override;
	void update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void destroy_surface(const RID surface_rid) override;
	[[nodiscard]] const vulkan::SurfaceStorage& get_surface_storage(RID id) const;
	[[nodiscard]] vulkan::SurfaceStorage& get_surface_storage_mutable(RID id);
	
	// pipeline layout
	RID create_pipeline_layout(const PipelineLayoutDescriptor &desc) override;
	void destroy_pipeline_layout(const RID pipeline_layout_rid) override;
	vk::PipelineLayout get_pipeline_layout(RID rid);
	
	// graphics pipeline
	
	RID create_graphics_pipeline(const GraphicsPipelineDescriptor &desc) override;
	void bind_pipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	void destroy_pipeline(const RID pipeline_rid) override;
	
	[[nodiscard]] vk::Pipeline get_pipeline(RID id) const;
	
	// commands
	[[nodiscard]] RID begin(RID surface_rid) override;
	uint32_t begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) override;
	void finish_rendering(const RID command_rid) override;
	void finish(const RID command_rid) override;
	void submit(RID command_rid) override;
	void present(RID surface_rid) override;
	void push_label(RID command_rid, const String &label) override;
	void pop_label(RID command_rid) override;
	void bind_shader(RID command_rid, RID shader_rid, gfx::ShaderStage stage) override;
	void bind_shader(RID command_rid, Vector<RID> shader_rids, Vector<gfx::ShaderStage> stages) override;
	void bind_shader(RID command_rid, Vector<BindShaderDescriptor> shader_descriptors) override;
	void transition(RID command_rid, const ImageTransitionDescriptor &descriptor);
	void transition(RID command_rid, RID image, gfx::ImageLayout layout, BitFlag<gfx::Access> access, BitFlag<gfx::PipelineStage> stage, ImageSubresourceDescriptor subresource);
	void transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors);
	void bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) override;
	void bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void DrawIndexed(RID command_rid, std::uint32_t first_index, std::uint32_t index_count);
	void draw_indexed(RID command_rid, std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance) override;
	void draw_indexed_indirect(RID command_rid, RID buffer, u64 buffer_offset, RID count_buffer, u64 count_buffer_offset, u32 max_draw_count, u32 stride) override;
	
	void dispatch(RID command_rid, uvec3 groups) override;
	void dispatch(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;void push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) override;
	void dispatch_mesh(RID command_rid, uvec3 groups) override;
	void dispatch_mesh(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;
	[[nodiscard]] vk::CommandBuffer get_command_buffer(RID id) const;
	[[nodiscard]] vulkan::CommandBufferStorage &get_command_buffer_storage(RID id);
	[[nodiscard]] const vulkan::CommandBufferStorage &get_command_buffer_storage(RID id) const;
	
	uint32_t queue_family(gfx::QueueFamilyType queue_family) const override;
	void prune_dead_objects();
	
	// Accessors for Vulkan objects
	[[nodiscard]] vk::Instance get_instance() const { return instance_; }
	[[nodiscard]] vk::PhysicalDevice get_adapter() const { return adapter_; }
	[[nodiscard]] vk::Device get_device() const { return device_; }
	[[nodiscard]] VmaAllocator get_allocator() const { return allocator_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_graphics_queues() const { return graphics_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_compute_queues() const { return compute_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& get_transfer_queues() const { return transfer_queue_; }
	[[nodiscard]] std::uint32_t get_graphics_queue_family() const { return graphics_queue_family_index_; }
	[[nodiscard]] std::uint32_t get_compute_queue_family() const { return compute_queue_family_index_; }
	[[nodiscard]] std::uint32_t get_transfer_queue_family() const { return transfer_queue_family_index_; }
	[[nodiscard]] vk::CommandPool get_command_pool() const { return command_pool_; }
	[[nodiscard]] vk::CommandPool get_transfer_command_pool() const { return transfer_command_pool_; }
	[[nodiscard]] vk::DescriptorPool get_descriptor_pool() const { return descriptor_pool_; }
	
	void wait_for_idle() override;
	
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

public:

private:
#endif
	std::uint64_t allocations_ = 0;
	
	SlotPool<vulkan::BufferStorage> buffers_;
	SlotPool<vulkan::ImageStorage> images_;
	SlotPool<vulkan::ImageViewStorage> image_views_;
	SlotPool<vk::Sampler> samplers_;
	SlotPool<vulkan::ShaderStorage> shader_modules_;
	SlotPool<vk::CommandPool> command_pools_;
	SlotPool<vk::Fence> fences_;
	SlotPool<vk::Semaphore> semaphores_;
	SlotPool<vulkan::CommandBufferStorage> command_buffers_;
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
	bool imgui_has_fully_uploaded = false;
	bool imgui_must_end_full_upload = false;
};

// Verify that VulkanGraphicsDriverBackend satisfies the GraphicsBackend concept
//static_assert(Backend<VkGraphicsBackend>, 
//              "VulkanGraphicsDriverBackend must satisfy GraphicsBackend concept");
