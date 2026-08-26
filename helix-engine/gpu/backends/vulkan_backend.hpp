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
	bool Init() override;
	void Stop() override;
	void YieldForAllCommands() override;
	
	// fence
	RID CreateFence(const Optional<String> &label = std::nullopt, bool signaled = false) override;
	void DestroyFence(RID fence_rid) override;
	[[nodiscard]] vk::Fence GetFence(RID id) const;
	
	// semaphore
	RID CreateSemaphore(const gfx::SemaphoreType semaphore_type = gfx::SemaphoreType::eBinary, const Optional<String> &label = std::nullopt) override;
	void DestroySemaphore(RID semaphore_rid) override;
	[[nodiscard]] vk::Semaphore GetSemaphore(RID id) const;
	
	// buffer
	RID CreateBuffer(const BufferDescriptor &desc) override;
	void DestroyBuffer(RID id) override;
	void SetBufferName(RID buffer_rid, const char *name) override;
	[[nodiscard]] void * Map(const RID buffer_rid) override;
	void Unmap(const RID buffer_rid) override;
	[[nodiscard]] void* GetMappedData(RID id) override;
	[[nodiscard]] GpuDeviceAddress GetBufferVirtualAddress(RID id) override;
	void FlushBuffer(RID buffer_rid, ivec2 range) override;
	[[nodiscard]] vk::Buffer GetBuffer(RID id);
	[[nodiscard]] VmaAllocation GetBufferAllocation(RID id);
	[[nodiscard]] VmaAllocationInfo GetBufferAllocationInfo(RID id);
	
	// image
	RID CreateImage() override;
	RID CreateImage(const ImageDescriptor &desc) override;
	void CreateImage(RID image_rid, const ImageDescriptor &desc) override;
	void DestroyImage(RID id) override;
	void SetImageName(RID handle, const char* name) override;
	[[nodiscard]] bool IsImageValid(RID image_rid) override;
	VkFence LoadImageFromBuffer(RID image_rid, RID buffer_rid, const Vector<VkBufferImageCopy2> &copy);
	VkFence LoadImageFromBuffer(RID image_rid, RID buffer_rid, VkBufferImageCopy2 &copy);
	[[nodiscard]] vk::Image GetImage(RID id) const;
	[[nodiscard]] const vulkan::ImageStorage& GetImageStorage(RID id) const;
	vulkan::ImageStorage &GetImageStorageMut(RID id);

	// image view
	RID CreateImageView(const ImageViewDescriptor& desc) override;
	void DestroyImageView(RID id) override;
	[[nodiscard]] bool IsImageViewValid(const RID image_view_rid) override;
	[[nodiscard]] vk::ImageView GetImageView(RID id) const;
	
	// sampler
	RID CreateSampler(const SamplerDescriptor &desc) override;
	void DestroySampler(RID sampler) override;
	[[nodiscard]] vk::Sampler GetSampler(const RID id) const;
	
	// bind group layout (descriptor set layout)
	RID CreateBindGroupLayout(const BindGroupLayoutDescriptor &desc) override;
	void DestroyBindGroupLayout(RID id) override;
	[[nodiscard]] vk::DescriptorSetLayout GetBindGroupLayout(RID id) const;
	
	// bind group (descriptor set)
	RID CreateBindGroup(const BindGroupDescriptor &desc) override;
	void DestroyBindGroup(const RID id) override;
	void UpdateBindGroup(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) override;
	void SetBindGroup(const RID command_rid, const RID pipeline_layout_rid, u32 index, const RID bind_group_rid, gfx::ShaderStage stage) override;
	[[nodiscard]] vk::DescriptorSet GetBindGroup(RID id) const;
	
	// shader
	
	[[nodiscard]] RID CreateShader(const SpirvDescriptor &spirv_descriptor) override;
	void DestroyShader(RID id) override;
	[[nodiscard]] vk::ShaderModule GetShaderModule(RID id) const;
	[[nodiscard]] vk::ShaderEXT GetShaderExt(RID id) const;
	
	// surface
	
	[[nodiscard]] RID CreateSurface(IWindow *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID CreateSurfaceUniversal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID CreateSurfaceSDL2(SDL2Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] RID CreateSurfaceGLFW3(GLFW3Window *window, const SurfaceDescriptor &desc) override;
	[[nodiscard]] Vector<gfx::Format> GetSurfaceFormats(const RID surface_rid) override;
	[[nodiscard]] gfx::Format GetSurfaceColorFormat(const RID surface_rid) override;
	[[nodiscard]] RID GetActiveImage(const RID surface_rid) override;
	[[nodiscard]] RID GetActiveImageView(const RID surface_rid) override;
	void UpdateSurfaceConfiguration(const RID surface_rid, const SurfaceDescriptor &desc) override;
	void DestroySurface(const RID surface_rid) override;
	[[nodiscard]] const vulkan::SurfaceStorage& GetSurfaceStorage(RID id) const;
	[[nodiscard]] vulkan::SurfaceStorage& GetSurfaceStorageMut(RID id);
	
	// pipeline layout
	RID CreatePipelineLayout(const PipelineLayoutDescriptor &desc) override;
	void DestroyPipelineLayout(const RID pipeline_layout_rid) override;
	vk::PipelineLayout GetPipelineLayout(RID rid);
	
	// graphics pipeline
	
	RID CreateGraphicsPipeline(const GraphicsPipelineDescriptor &desc) override;
	void BindPipeline(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) override;
	void DestroyPipeline(const RID pipeline_rid) override;
	
	[[nodiscard]] vk::Pipeline GetPipeline(RID id) const;
	
	// commands
	[[nodiscard]] RID Begin(RID surface_rid) override;
	uint32_t BeginRendering(RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) override;
	void FinishRendering(const RID command_rid) const override;
	void Finish(const RID command_rid) const override;
	void Submit(RID surface_rid, RID command_rid) override;
	void Present(RID surface_rid) override;
	void PushLabel(RID command_rid, const String &label) override;
	void PopLabel(RID command_rid) override;
	void BindShader(RID command_rid, RID shader_rid, gfx::ShaderStage stage) override;
	void BindShader(RID command_rid, Vector<RID> shader_rids, Vector<gfx::ShaderStage> stages) override;
	void BindShader(RID command_rid, Vector<BindShaderDescriptor> shader_descriptors) override;
	void Transition(RID command_rid, const ImageTransitionDescriptor &descriptor) override;
	void Transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) override;
	void BindVertexBuffer(const RID command_rid, const VertexBufferDescriptor &desc) override;
	void BindVertexBuffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) override;
	void BindIndexBuffer(const RID command_rid, const IndexBufferDescriptor &desc) override;
	void DrawIndexed(RID command_rid, std::uint32_t first_index, std::uint32_t index_count);
	void DrawIndexedInstanced(RID command_rid, std::uint32_t index_count, std::uint32_t instance_count, std::uint32_t first_index, std::int32_t vertex_offset, std::uint32_t first_instance) override;
	void Dispatch(RID command_rid, uvec3 groups) override;
	void Dispatch(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;void PushConstants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) override;
	void DispatchMesh(RID command_rid, uvec3 groups) override;
	void DispatchMesh(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) override;
	[[nodiscard]] vk::CommandBuffer GetCommandBuffer(RID id) const;
	
	uint32_t QueueFamily(gfx::QueueFamilyType queue_family) const override;
	void PruneDeadObjects();
	
	// Accessors for Vulkan objects
	[[nodiscard]] vk::Instance GetInstance() const { return instance_; }
	[[nodiscard]] vk::PhysicalDevice GetAdapter() const { return adapter_; }
	[[nodiscard]] vk::Device GetDevice() const { return device_; }
	[[nodiscard]] VmaAllocator GetAllocator() const { return allocator_; }
	[[nodiscard]] const std::vector<vk::Queue>& GetGraphicsQueues() const { return graphics_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& GetComputeQueues() const { return compute_queue_; }
	[[nodiscard]] const std::vector<vk::Queue>& GetTransferQueues() const { return transfer_queue_; }
	[[nodiscard]] std::uint32_t GetGraphicsQueueFamily() const { return graphics_queue_family_index_; }
	[[nodiscard]] std::uint32_t GetComputeQueueFamily() const { return compute_queue_family_index_; }
	[[nodiscard]] std::uint32_t GetTransferQueueFamily() const { return transfer_queue_family_index_; }
	[[nodiscard]] vk::CommandPool GetCommandPool() const { return command_pool_; }
	[[nodiscard]] vk::CommandPool GetTransferCommandPool() const { return transfer_command_pool_; }
	[[nodiscard]] vk::DescriptorPool GetDescriptorPool() const { return descriptor_pool_; }
	
	void WaitForDeviceIdle() override;
	
	void prune();
	
private:
	void CreateInstance();
	void RequestAdapter();
	void CreateDeviceAndQueues();
	void CreateAllocator();
	void CreateDefaultPools();

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
	
	void LoadInstanceExtensionFunctions();
	void LoadDeviceExtensionFunctions();
	
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
