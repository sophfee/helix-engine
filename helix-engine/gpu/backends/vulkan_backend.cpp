// ReSharper disable CppClangTidyClangDiagnosticMissingDesignatedFieldInitializers
// ReSharper disable CppClangTidyClangDiagnosticMissingFieldInitializers
// ReSharper disable CppClangTidyMiscUseAnonymousNamespace
// ReSharper disable CppTooWideScopeInitStatement
// ReSharper disable CppVariableCanBeMadeConstexpr
#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vma/vk_mem_alloc.hpp>

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"
#include "detail/vulkan_enum_conversion.hpp"
#include "ecs/transform.h"
#include "glfw/glfw3.h"
#include "gpu/window.hpp"

using detail::has_flag;
using namespace gfx;

namespace {
	void apply_label(const char* name, const VkObjectType object_type, const uint64_t object_handle) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = object_type,
			.objectHandle = object_handle,
			.pObjectName = name
		};
		VkGraphicsDriverBackend* vk = (VkGraphicsDriverBackend*)GraphicsSystem::get_driver();
		vk->vkSetDebugUtilsObjectName(vk->get_device(), &name_info);
	}

	[[maybe_unused]] void apply_label(VkGraphicsDriverBackend* vk, const char* name, const VkObjectType object_type, const uint64_t object_handle) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = object_type,
			.objectHandle = object_handle,
			.pObjectName = name
		};
		vk->vkSetDebugUtilsObjectName(vk->get_device(), &name_info);
	}
	
	template <typename T>
	[[maybe_unused]] void apply_label(VkGraphicsDriverBackend* vk, const char* name, const VkObjectType object, T object_handle) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = object,
			.objectHandle = reinterpret_cast<uint64_t>(object_handle),
			.pObjectName = name
		};
		vk->vkSetDebugUtilsObjectName(vk->get_device(), &name_info);
	}
	
	template <typename T>
	[[maybe_unused]] void apply_label(VkGraphicsDriverBackend* vk, const std::string& name, const VkObjectType object, T object_handle) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = object,
			.objectHandle = reinterpret_cast<uint64_t>(object_handle),
			.pObjectName = name.c_str()
		};
		vk->vkSetDebugUtilsObjectName(vk->get_device(), &name_info);
	}
	
	template <typename T, VkObjectType E>
	void apply_label(VkGraphicsDriverBackend* vk, const std::string& name, T object_handle) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = E,
			.objectHandle = reinterpret_cast<uint64_t>((typename T::CType)object_handle),
			.pObjectName = name.c_str()
		};
		vk->vkSetDebugUtilsObjectName(vk->get_device(), &name_info);
	}
	
	template <typename T>
	void apply_label(VkGraphicsDriverBackend* vk, const std::string& name, T object_handle) {
	}
	
#define APPLY_LABEL_SPECIALIZATION(TYPE, VK_TYPE) \
	template <> void apply_label<TYPE>(VkGraphicsDriverBackend* vk, const std::string& name, TYPE object_handle) { \
		apply_label<TYPE, VK_TYPE>(vk, name, object_handle); \
	}
	
	APPLY_LABEL_SPECIALIZATION(vk::Device, VK_OBJECT_TYPE_DEVICE)
	APPLY_LABEL_SPECIALIZATION(vk::Buffer, VK_OBJECT_TYPE_BUFFER)
	APPLY_LABEL_SPECIALIZATION(vk::Image, VK_OBJECT_TYPE_IMAGE)
	APPLY_LABEL_SPECIALIZATION(vk::ImageView, VK_OBJECT_TYPE_IMAGE_VIEW)
	APPLY_LABEL_SPECIALIZATION(vk::Sampler, VK_OBJECT_TYPE_SAMPLER)
	APPLY_LABEL_SPECIALIZATION(vk::ShaderModule, VK_OBJECT_TYPE_SHADER_MODULE)
	APPLY_LABEL_SPECIALIZATION(vk::Pipeline, VK_OBJECT_TYPE_PIPELINE)
	APPLY_LABEL_SPECIALIZATION(vk::PipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT)
	APPLY_LABEL_SPECIALIZATION(vk::DescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT)
	APPLY_LABEL_SPECIALIZATION(vk::DescriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET)
	APPLY_LABEL_SPECIALIZATION(vk::CommandPool, VK_OBJECT_TYPE_COMMAND_POOL)
	APPLY_LABEL_SPECIALIZATION(vk::CommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER)
	APPLY_LABEL_SPECIALIZATION(vk::Fence, VK_OBJECT_TYPE_FENCE)
	APPLY_LABEL_SPECIALIZATION(vk::Semaphore, VK_OBJECT_TYPE_SEMAPHORE)
	APPLY_LABEL_SPECIALIZATION(vk::SwapchainKHR, VK_OBJECT_TYPE_SWAPCHAIN_KHR)
	APPLY_LABEL_SPECIALIZATION(vk::SurfaceKHR, VK_OBJECT_TYPE_SURFACE_KHR)
	APPLY_LABEL_SPECIALIZATION(vk::Queue, VK_OBJECT_TYPE_QUEUE)
	APPLY_LABEL_SPECIALIZATION(vk::ShaderEXT, VK_OBJECT_TYPE_SHADER_EXT)
	APPLY_LABEL_SPECIALIZATION(vk::AccelerationStructureKHR, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR)
	APPLY_LABEL_SPECIALIZATION(vk::DebugUtilsMessengerEXT, VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT)
	APPLY_LABEL_SPECIALIZATION(vk::DebugReportCallbackEXT, VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT)
	
#undef APPLY_LABEL_SPECIALIZATION
}

#ifdef _DEBUG

VkBool32 vulkan::vkDebugMessengerCallback(
	const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	const VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData
) {
	std::string message;
	bool interrupt = false;
	
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		message += "[VERBOSE] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		message += "[INFO] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		message += "[WARNING] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		message += "[ERROR] ";
		interrupt = true;
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		message += "[GENERAL] ";
	}
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		message += "[VALIDATION] ";
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		message += "[PERFORMANCE] ";
	}
	
	message += pCallbackData->pMessage;
	std::cout << message << '\n';
	
	if (interrupt)
		__debugbreak();

	return false;
}
#endif

void vkResultCheckInner(VkResult result, const char *file, const int line, const char *msg) {
	char message[4096] = {};
	
	int err = sprintf_s(message, 4096, "[%s:%d] Vulkan error: %s", file, line, msg);
	
	vk::detail::resultCheck(static_cast<vk::Result>(result), message);
}

VkGraphicsDriverBackend::VkGraphicsDriverBackend() : debug_messenger_(nullptr), allocator_(nullptr), graphics_queue_family_index_(0),
										 compute_queue_family_index_(0), transfer_queue_family_index_(0) {
	//initialize();
}

VkGraphicsDriverBackend::~VkGraphicsDriverBackend() {
	shutdown();
}

bool VkGraphicsDriverBackend::initialize() {
	
	if (instance_ != VK_NULL_HANDLE) return true;
	
	create_instance();
	request_adapter();
	create_device_and_queues();
	create_allocator();
	create_default_pools();
	create_default_images();
	create_default_sampler();

	return true;
}

void VkGraphicsDriverBackend::shutdown() {
	for (const std::pair surface : surfaces_)
		if (surface.second != VK_NULL_HANDLE)
			destroy_surface(surface.first);
	
	wait_for_idle();
	
	destroy_image_view(default_normal_image_view);
	destroy_image_view(default_orm_image_view);
	destroy_image(default_normal_image);
	destroy_image(default_orm_image);
	destroy_sampler(default_sampler_);
	
	for (vulkan::ImageTransferStorage& transfer_storage : image_transfers_) {
		if (transfer_storage.fence == VK_NULL_HANDLE) continue;
		vk::Result result = device_.waitForFences(transfer_storage.fence, vk::True, std::numeric_limits<u64>::max());
		assert(result == vk::Result::eSuccess && "Failed to resolve an image transfer fence");
		device_.destroyFence(transfer_storage.fence);
		transfer_storage.fence = VK_NULL_HANDLE;
	}

	buffers_.clear();
	image_views_.clear();
	images_.clear();
	samplers_.clear();
	
	for (const std::pair fence : fences_) {
		if (*fence.second != VK_NULL_HANDLE) {
			VkFence fence_c = *fence.second;
			const VkResult result = vkWaitForFences(device_, 1, &fence_c, VK_TRUE, 10000);
			if (result == VK_TIMEOUT)
				helix_print("Warning: Fence wait timed out during shutdown. This may indicate a problem with the application.\n");
			else if (result != VK_SUCCESS)
				vkCheck(result, "Failed to wait for fence during shutdown"); //< Send to the normal error reporting pipeline
			vkDestroyFence(device_, fence_c, nullptr);
		}
	}
	fences_.clear();
	
	for (const std::pair slot : shader_modules_) {
		device_.destroyShaderModule(slot.second->shader_module);
		if (slot.second->shader_ext != VK_NULL_HANDLE)
			vkDestroyShader(device_, slot.second->shader_ext, nullptr);
	}
	shader_modules_.clear();
	
	for (const std::pair slot : descriptor_set_layouts_) {
		device_.destroyDescriptorSetLayout(*slot.second);
	}
	
	for (const std::pair slot : pipeline_layouts_) {
		device_.destroyPipelineLayout(*slot.second);
	}
	pipeline_layouts_.clear();
	
	for (const std::pair pipeline : pipelines_)
		device_.destroyPipeline(*pipeline.second);
	pipelines_.clear();
	
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
	for (const std::pair slot : descriptor_sets_)
		device_.freeDescriptorSets(descriptor_pool_, { slot.second->descriptor_set });
	descriptor_sets_.clear();
	
	for (const std::pair slot : command_pools_)
		device_.destroyCommandPool(*slot.second);
	command_pools_.clear();
	
	for (auto* window : windows_)
		if (window != nullptr)
			window->dispose();
	windows_.clear();
	
	device_.destroyCommandPool(command_pool_);
	
	{
		std::scoped_lock lock(transfer_mutex_);
		for (const vk::CommandPool pool : transfer_command_pools_) {
			device_.destroyCommandPool(pool);
		}
	}
	transfer_command_pools_.clear();
	device_.destroyCommandPool(transfer_command_pool_);
	device_.destroyDescriptorPool(descriptor_pool_);

	vmaDestroyAllocator(allocator_);
	device_.destroy();
	
#ifdef _DEBUG
	vkDestroyDebugUtilsMessenger(instance_, debug_messenger_, nullptr);
#endif
	
	instance_.destroy();
}

void VkGraphicsDriverBackend::yield_for_commands() {
	//for (auto &q : graphics_queue_)
	//	q.waitIdle();
	//for (auto &q : compute_queue_)
	//	q.waitIdle();
	//for (auto &q : transfer_queue_)
	//	q.waitIdle();
	wait_for_idle();
}

void VkGraphicsDriverBackend::initialize_im_gui() {
#ifdef _DEBUG

	
	ImGui_ImplVulkan_InitInfo imgui_impl_vulkan_init_info{
		.ApiVersion = 0,
		.Instance = instance_,
		.PhysicalDevice = adapter_,
		.Device = device_,
		.QueueFamily = graphics_queue_family_index_,
		.Queue = graphics_queue_[0],
		.DescriptorPoolSize = 1000,
		.MinImageCount = 2,
		.ImageCount = 2,
		.PipelineCache = VK_NULL_HANDLE,
		.UseDynamicRendering = true
	};
	
	assert(ImGui_ImplVulkan_Init(&imgui_impl_vulkan_init_info));
	
	Vector<VkFormat> color_attachment_formats{
		VK_FORMAT_R8G8B8A8_SRGB
	};
	const VkFormat depth_attachment_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	const VkPipelineRenderingCreateInfo rendering_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
		.pColorAttachmentFormats = color_attachment_formats.data(),
		.depthAttachmentFormat = depth_attachment_format
	};

	const ImGui_ImplVulkan_PipelineInfo imgui_impl_vulkan_pipeline_info{
		.PipelineRenderingCreateInfo = rendering_create_info
	};
	
	ImGui_ImplVulkan_CreateMainPipeline(&imgui_impl_vulkan_pipeline_info);
	
	
#endif
}

RID VkGraphicsDriverBackend::create_buffer(const BufferDescriptor &desc) {
	const vk::BufferCreateInfo buffer_create_info = vk::BufferCreateInfo()
		.setSize(desc.size)
		.setUsage(vk::detail::convert_hpp(desc.usage))
		.setSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndices({});
	
	const vma::AllocationCreateInfo allocation_info = vma::AllocationCreateInfo()
		.setFlags((vma::AllocationCreateFlags)vk::detail::convert(desc.allocation_hints.value_or(AllocationHint::eDedicated)))
		.setUsage((vma::MemoryUsage)vk::detail::convert(desc.memory_usage.value_or(MemoryUsage::eAuto)))
		.setPool(nullptr)
		.setPUserData(nullptr)
		.setPriority(0.0f);
	
	vk::Buffer buffer;
	vma::Allocation allocation;
	{
		std::scoped_lock lock(allocation_mutex_);
		const std::pair<vma::Allocation, vk::Buffer> result = allocator_.createBuffer(buffer_create_info, allocation_info);
		allocation = result.first;
		buffer = result.second;
	}

	SlotPool<vulkan::BufferStorage>::Handle handle = buffers_.emplace(vulkan::BufferStorage{
		.buffer = buffer,
		.allocation = allocation,
		.is_allocated = true
	});
	
#ifdef _DEBUG
	
	if (desc.label.has_value()) {
		set_buffer_name(handle, desc.label.value().c_str());
	}

#endif
	
	return handle;
}

void VkGraphicsDriverBackend::destroy_buffer(const RID id) {
	if (id.lower <= 0) return;
	const vulkan::BufferStorage *storage = buffers_.get(id);
	if (storage && storage->is_allocated) vmaDestroyBuffer(allocator_, storage->buffer, storage->allocation);
}

void VkGraphicsDriverBackend::set_buffer_name(const RID buffer_rid, const char *name) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaSetAllocationName(allocator_, allocation, name);
	apply_label(this, name, get_buffer(buffer_rid));
}

void * VkGraphicsDriverBackend::map_buffer(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	void *mapped_data;
	const VkResult result = vmaMapMemory(allocator_, allocation, &mapped_data);
	vkCheck(result, "Failed to map buffer memory");
	return mapped_data;
}

void VkGraphicsDriverBackend::unmap_buffer(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaUnmapMemory(allocator_, allocation);
}

void * VkGraphicsDriverBackend::get_mapped_data(const RID id) {
	const VmaAllocationInfo allocation_info = get_buffer_allocation_info(id);
	return allocation_info.pMappedData;
}

GpuDeviceAddress VkGraphicsDriverBackend::get_buffer_virtual_address(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id);
	const VkBufferDeviceAddressInfo buffer_address_info = VkBufferDeviceAddressInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.pNext = nullptr,
		.buffer = storage->buffer
	};
	return vkGetBufferDeviceAddress(device_, &buffer_address_info);
}

void VkGraphicsDriverBackend::flush_buffer(const RID buffer_rid, const ivec2 range) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaFlushAllocation(allocator_, allocation, range.x, range.y == -1 ? VK_WHOLE_SIZE : range.y);
}

RID VkGraphicsDriverBackend::create_image() {
	return images_.emplace(vulkan::ImageStorage{
		.image = VK_NULL_HANDLE,
		.allocation = VK_NULL_HANDLE,
		.format = gfx::Format::eUndefined,
		.layout = ImageLayout::eUndefined,
		.access = Access::eNone,
		.stage = PipelineStage::eNone
	});
}

RID VkGraphicsDriverBackend::create_image(const ImageDescriptor &desc) {
	
	RID image = create_image();
	create_image(image, desc);
	
	return image;
}

void VkGraphicsDriverBackend::create_image(const RID image_rid, const ImageDescriptor &desc) {
	
	vk::ImageCreateFlags flags;
	
	if (desc.type == ImageType::eCube) {
		flags |= vk::ImageCreateFlagBits::eCubeCompatible;
	}
	const vk::ImageCreateInfo image_create_info = vk::ImageCreateInfo()
		.setFlags(flags)
		.setImageType(vk::detail::convert_hpp(desc.type))
		.setFormat(vk::detail::convert_hpp(desc.format))
		.setExtent({ desc.size.x, desc.size.y, desc.size.z })
		.setMipLevels(std::max(desc.mip_levels, 1u))
		.setArrayLayers(std::max(desc.array_layers, 1u))
		.setSamples(vk::detail::convert_hpp(desc.samples))
		.setTiling(vk::ImageTiling::eOptimal)
		.setUsage(vk::detail::convert_hpp(desc.usage))
		.setSharingMode(vk::SharingMode::eExclusive)
		.setInitialLayout(vk::ImageLayout::eUndefined);
	
	vma::AllocationCreateInfo allocation_info = vma::AllocationCreateInfo()
		.setFlags((vma::AllocationCreateFlags)vk::detail::convert(desc.allocation_hints))
		.setUsage((vma::MemoryUsage)vk::detail::convert(desc.memory_usage));

	vulkan::ImageStorage &storage = get_image_storage(image_rid);
	
	{
		std::scoped_lock l(allocation_mutex_);
		std::pair<vma::Allocation, vk::Image> result = allocator_.createImage(image_create_info, allocation_info);
		storage.allocation = result.first;
		storage.image = result.second;
	}
	
	storage.size = desc.size;
	storage.format = desc.format;
	storage.level_count = std::max(desc.mip_levels, 1u);
	storage.layer_count = std::max(desc.array_layers, 1u);
	storage.layout = gfx::ImageLayout::eUndefined;
	storage.access = Access::eNone;
	storage.stage = PipelineStage::eNone;
	storage.image_type = desc.type;
	
	if (desc.label.has_value()) set_image_name(image_rid, desc.label.value().c_str());
	
}

void VkGraphicsDriverBackend::destroy_image(const RID id) {
	if (!id.valid()) return;
	const vulkan::ImageStorage &storage = get_image_storage(id);
	
	for (SlotPool<vulkan::ImageViewStorage>::Handle view : storage.associated_views)
		destroy_image_view(view);

	allocator_.destroyImage(storage.image, storage.allocation);
	assert(images_.erase(id));
}

void VkGraphicsDriverBackend::set_image_name(const RID handle, const char *name) {
	vulkan::ImageStorage &storage = get_image_storage(handle);
#ifdef _DEBUG
	storage.name = name;
	
	apply_label(this, name, storage.image);
	
#endif
	vmaSetAllocationName(allocator_, storage.allocation, name);
}

size_t VkGraphicsDriverBackend::get_associated_image_views(const RID image_rid, Vector<RID> &image_views_out) {
	const Vector<SlotPool<vulkan::ImageViewStorage>::Handle> image_views = get_image_storage(image_rid).associated_views;
	for (auto image_view : image_views)
		image_views_out.push_back(image_view);
	return image_views.size();
}

RID VkGraphicsDriverBackend::get_associated_image_view(const RID image_rid, const int index) {
	return get_image_storage(image_rid).associated_views[index];
}

bool VkGraphicsDriverBackend::is_image_valid(const RID image_rid) {
	const vulkan::ImageStorage *storage = images_.get(image_rid);
	return storage != nullptr && storage->image != VK_NULL_HANDLE;
}

VkFence VkGraphicsDriverBackend::load_image_from_buffer(const RID image_rid, const RID buffer_rid, const Vector<VkBufferImageCopy2> &copy) {
	vk::Fence fence;
	vk::CommandBuffer command;
	
	const vulkan::ImageStorage& storage = get_image_storage(image_rid);
	const vk::Image image = get_image(image_rid);
	const vk::Buffer buffer = get_buffer(buffer_rid);
	{
		fence = device_.createFence(vk::FenceCreateInfo());
		
#ifdef _DEBUG
		const std::string label_name = "Transfer Op: I[" + std::to_string(image_rid.upper) + ":" + std::to_string(image_rid.lower) + "] << B[" + std::to_string(buffer_rid.upper) + ":" + std::to_string(buffer_rid.lower) + "]";
		apply_label(this, label_name, fence);
#endif
	}

	{
		std::scoped_lock lock(transfer_mutex_);
		if (transfer_command_pool_ == VK_NULL_HANDLE)
			transfer_command_pools_.emplace_back(device_.createCommandPool(vk::CommandPoolCreateInfo()
				.setFlags(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
				.setQueueFamilyIndex(transfer_queue_family_index_)));
		
		command = device_.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
			.setCommandPool(transfer_command_pool_)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1))[0];
	}

	command.begin(vk::CommandBufferBeginInfo()
	   .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));

	{
		const vk::ImageMemoryBarrier2 transfer_barrier = vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
			.setSrcAccessMask(vk::AccessFlagBits2::eNone)
			.setDstStageMask(vk::PipelineStageFlagBits2::eTransfer)
			.setDstAccessMask(vk::AccessFlagBits2::eTransferWrite)
			.setOldLayout(vk::ImageLayout::eUndefined)
			.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
			.setImage(image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(std::max(storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1));
		
		command.pipelineBarrier2(vk::DependencyInfo()
			.setImageMemoryBarriers({transfer_barrier}));
	}
	command.copyBufferToImage2(vk::CopyBufferToImageInfo2()
		.setSrcBuffer(buffer)
		.setDstImage(image)
		.setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
		.setRegionCount(static_cast<uint32_t>(copy.size()))
		.setPRegions((const vk::BufferImageCopy2*)copy.data()));
	
	{
		const vk::ImageMemoryBarrier2 transfer_barrier = vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
			.setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
			.setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader)
			.setDstAccessMask(vk::AccessFlagBits2::eShaderRead)
			.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
			.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setSrcQueueFamilyIndex(transfer_queue_family_index_)
			.setDstQueueFamilyIndex(graphics_queue_family_index_)
			.setImage(image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(std::max(storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1));
		
		command.pipelineBarrier2(vk::DependencyInfo()
			.setImageMemoryBarriers({transfer_barrier}));
	}
	command.end();
	
	VkCommandBufferSubmitInfo command_buffer_submit_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.pNext = nullptr,
		.commandBuffer = command,
		.deviceMask = 0
	};
	
	const VkSubmitInfo2 submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &command_buffer_submit_info,
	};
	
	std::future boink = std::async([&]{
		std::scoped_lock lock(transfer_mutex_);
		const uint64_t index = transfer_queue_to_use++ % transfer_queue_.size();
		vkCheck(vkQueueSubmit2(transfer_queue_[index], 1, &submit_info, fence), "Failed to submit transfer command buffer");
		image_transfers_.push_back(vulkan::ImageTransferStorage{
			.fence = fence,
			.command_buffer = command
		});
	});
	boink.get();
	
	return fence; // used to check
}

VkFence VkGraphicsDriverBackend::load_image_from_buffer(const RID image_rid, const RID buffer_rid, VkBufferImageCopy2 &copy) {
	return load_image_from_buffer(image_rid, buffer_rid, Vector{ copy });
}

vk::Image VkGraphicsDriverBackend::get_image(const RID id) const {
	try {
		const vulkan::ImageStorage *storage = images_.get(id);
		return storage->image;
	}
	catch ([[maybe_unused]] const std::exception &e) {
		return VK_NULL_HANDLE;
	}
}

const vulkan::ImageStorage &VkGraphicsDriverBackend::get_image_storage(const RID id) const {
	const vulkan::ImageStorage *storage = images_.get(id);
	return *storage;
}

vulkan::ImageStorage &VkGraphicsDriverBackend::get_image_storage(const RID id) {
	vulkan::ImageStorage *storage = images_.get(id);
	return *storage;
}

RID VkGraphicsDriverBackend::create_image_view(const ImageViewDescriptor &desc) {
	vulkan::ImageStorage &storage = get_image_storage(desc.image);
	const vk::Image image = storage.image;

	const vk::ImageUsageFlags usage_flags = vk::detail::convert_hpp(desc.usage);
	const vk::ImageViewUsageCreateInfo image_view_usage_create_info = vk::ImageViewUsageCreateInfo()
		.setUsage(usage_flags);
	
	ImageViewType view_type = desc.type;
	if (view_type == ImageViewType::eAuto) {
		switch (storage.image_type) {
		case ImageType::e1D:
			view_type = ImageViewType::e1D;
			break;
		case ImageType::e2D:
			view_type = ImageViewType::e2D;
			break;
		case ImageType::e3D:
			view_type = ImageViewType::e3D;
			break;
		case ImageType::eCube:
			view_type = ImageViewType::eCube;
			break;
		}
	}
	
	gfx::Format format = desc.format;
	if (format == gfx::Format::eUndefined) 
		format = storage.format;
	
	const vk::ImageViewCreateInfo image_view_create_info = vk::ImageViewCreateInfo()
		.setPNext((u32)usage_flags != 0 ? &image_view_usage_create_info : nullptr) //< Only set pNext if usageFlags is not 0, which is when no usage is specified. Vulkan Spec doesn't like when there is nothing proper sent.
		.setImage(image)
		.setViewType(vk::detail::convert_hpp(view_type))
		.setFormat(vk::detail::convert_hpp(format))
		.setComponents(vk::ComponentMapping()
			.setR(vk::detail::convert_hpp(Swizzle::eIdentity))
			.setG(vk::detail::convert_hpp(Swizzle::eIdentity))
			.setB(vk::detail::convert_hpp(Swizzle::eIdentity))
			.setA(vk::detail::convert_hpp(Swizzle::eIdentity)))
		.setSubresourceRange(vk::detail::convert(desc.subresource));

	const vk::ImageView image_view = device_.createImageView(image_view_create_info);
	
#ifdef _DEBUG
	if (desc.label.has_value())
		apply_label(this, desc.label.value(), image_view);
#endif

	const SlotPool<vulkan::ImageViewStorage>::Handle handle = image_views_.emplace(vulkan::ImageViewStorage{image_view, desc.image});
	storage.associated_views.push_back(handle);
	return handle;
}

void VkGraphicsDriverBackend::destroy_image_view(const RID id) {
	if (!id.valid()) return;
	if (!image_views_.contains(id)) return;
	const vk::ImageView image_view = get_image_view(id);
	if (image_view == VK_NULL_HANDLE) return;
	device_.destroyImageView(image_view);
	assert(image_views_.erase(id));
}

bool VkGraphicsDriverBackend::is_image_view_valid(const RID image_view_rid) {
	if (!image_view_rid.valid()) return false;
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(image_view_rid);
	return image_view_storage != nullptr && image_view_storage->image_view != VK_NULL_HANDLE;
}

vk::ImageView VkGraphicsDriverBackend::get_image_view(const RID id) const {
	if (!id.valid()) return VK_NULL_HANDLE;
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(id);
	if (image_view_storage == nullptr) return VK_NULL_HANDLE;
	return image_view_storage->image_view;
}

RID VkGraphicsDriverBackend::get_source_from_view(const RID image_view_rid) const {
	if (!image_view_rid.valid()) return RID{};
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(image_view_rid);
	if (image_view_storage == nullptr) return RID{};
	return image_view_storage->image;
}

const vulkan::ImageViewStorage & VkGraphicsDriverBackend::get_image_view_storage(const RID id) const {
	if (!id.valid()) throw std::runtime_error("Invalid RID");
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(id);
	if (image_view_storage == nullptr) throw std::runtime_error("Image view storage not found");
	return *image_view_storage;
}

vulkan::ImageViewStorage & VkGraphicsDriverBackend::get_image_view_storage(const RID id) {
	if (!id.valid()) throw std::runtime_error("Invalid RID");
	vulkan::ImageViewStorage* image_view_storage = image_views_.get(id);
	if (image_view_storage == nullptr) throw std::runtime_error("Image view storage not found");
	return *image_view_storage;
}

RID VkGraphicsDriverBackend::create_sampler(const SamplerDescriptor &desc) {
	const VkSamplerCreateInfo sampler_create_info{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = vk::detail::convert(desc.mag_filter.value_or(Filter::eLinear)),
		.minFilter = vk::detail::convert(desc.min_filter.value_or(Filter::eLinear)),
		.mipmapMode = vk::detail::convert(desc.mipmap_mode.value_or(MipmapFilter::eLinear)),
		.addressModeU = vk::detail::convert(desc.address_mode_u.value_or(AddressMode::eRepeat)),
		.addressModeV = vk::detail::convert(desc.address_mode_v.value_or(AddressMode::eRepeat)),
		.addressModeW = vk::detail::convert(desc.address_mode_w.value_or(AddressMode::eRepeat)),
		.mipLodBias = desc.mip_lod_bias,
		.anisotropyEnable = desc.max_anisotropy > 1.0f ? VK_TRUE : VK_FALSE,
		.maxAnisotropy = desc.max_anisotropy,
		.compareEnable = desc.compare_op.has_value() ? VK_TRUE : VK_FALSE,
		.compareOp = vk::detail::convert(desc.compare_op.value_or(CompareOp::eAlways)),
		.minLod = desc.min_lod,
		.maxLod = desc.max_lod,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};

	VkSampler sampler;
	vkCheck(vkCreateSampler(device_, &sampler_create_info, nullptr, &sampler), "Failed to create sampler");
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SAMPLER,
			.objectHandle = reinterpret_cast<uint64_t>(sampler),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif
return samplers_.emplace(sampler);
}

void VkGraphicsDriverBackend::destroy_sampler(const RID sampler) {
	if (!sampler.valid()) return;
	const vk::Sampler vk_sampler = get_sampler(sampler);
	device_.destroySampler(vk_sampler);
	assert(samplers_.erase(sampler));
}

RID VkGraphicsDriverBackend::get_default_sampler() const {
	return default_sampler_;
}

vk::Sampler VkGraphicsDriverBackend::get_sampler(const RID id) const {
	const vk::Sampler* sampler = samplers_.get(id);
	if (sampler == nullptr) return VK_NULL_HANDLE;
	return *sampler;
}

RID VkGraphicsDriverBackend::create_bind_group_layout(const BindGroupLayoutDescriptor &bind_group_layout_descriptor) {
	Vector<VkDescriptorSetLayoutBinding> bindings(bind_group_layout_descriptor.entries.size());
	Vector<VkDescriptorBindingFlags> binding_flags(bind_group_layout_descriptor.entries.size());
	for (std::size_t i = 0; i < bind_group_layout_descriptor.entries.size(); ++i)
	{
		const BindGroupLayoutEntryDescriptor &desc = bind_group_layout_descriptor.entries[i];
		bindings[i] = VkDescriptorSetLayoutBinding{
			.binding = desc.binding,
			.descriptorType = vk::detail::convert(desc.type),
			.descriptorCount = desc.count.value_or(1),
			.stageFlags = vk::detail::convert(desc.visibility),
			.pImmutableSamplers = nullptr
		};
		binding_flags[i] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
	}
	const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.pNext = nullptr,
		.bindingCount = static_cast<uint32_t>(bind_group_layout_descriptor.entries.size()),
		.pBindingFlags = binding_flags.data()
	};

	const VkDescriptorSetLayoutCreateInfo create_info = VkDescriptorSetLayoutCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = &binding_flags_create_info,
		.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VkDescriptorSetLayout layout;
	vkCheck(vkCreateDescriptorSetLayout(device_, &create_info, nullptr, &layout), "Failed to create descriptor set layout");
	
#ifdef _DEBUG
	if (bind_group_layout_descriptor.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
			.objectHandle = reinterpret_cast<uint64_t>(layout),
			.pObjectName = bind_group_layout_descriptor.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif
	return descriptor_set_layouts_.emplace(layout);
}

void VkGraphicsDriverBackend::destroy_bind_group_layout(const RID id) {
	const vk::DescriptorSetLayout layout = get_bind_group_layout(id);
	vkDestroyDescriptorSetLayout(device_, layout, nullptr);
	assert(descriptor_set_layouts_.erase(id));
}

vk::DescriptorSetLayout VkGraphicsDriverBackend::get_bind_group_layout(const RID id) const {
	const vk::DescriptorSetLayout* layout = descriptor_set_layouts_.get(id);
	return *layout;
}

RID VkGraphicsDriverBackend::create_bind_group(const BindGroupDescriptor &desc) {
	VkDescriptorSetLayout layout = get_bind_group_layout(desc.layout);
	const VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptor_pool_,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout
	};
	VkDescriptorSet descriptor_set;
	vkCheck(vkAllocateDescriptorSets(device_, &descriptor_set_allocate_info, &descriptor_set), "Failed to allocate descriptor set");
	
#ifdef _DEBUG
	if (desc.label.has_value()) { apply_label(this, desc.label.value(), descriptor_set); }
#endif

	const SlotPool<vulkan::DescriptorSetStorage>::Handle bind_group = descriptor_sets_.emplace(vulkan::DescriptorSetStorage{
		.descriptor_set = descriptor_set,
		.layout = desc.layout
	});
	
	// If entries are specified, then we can write to the Descriptor Set
	if (desc.entries.empty())
		return bind_group;
	
	update_bind_group(bind_group, desc.entries);
	return bind_group;
}

void VkGraphicsDriverBackend::destroy_bind_group(const RID id) {
	if (!id.valid()) return;
	const VkDescriptorSet descriptor_set = get_bind_group(id);
	vkCheck(vkFreeDescriptorSets(device_, descriptor_pool_, 1, &descriptor_set), "Failed to free descriptor set");
	assert(descriptor_sets_.erase(id));
}

void VkGraphicsDriverBackend::update_bind_group(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) {
	vulkan::DescriptorSetStorage &bind_group_storage = get_bind_group_storage(bind_group_rid);
	
	std::size_t image_load_store_index = 0llu;
	bind_group_storage.image_load_stores.clear();
	bind_group_storage.image_load_stores.resize(128);
	
	std::size_t sampled_images_index = 0llu;
	bind_group_storage.sampled_images.clear();
	bind_group_storage.sampled_images.resize(128);
	
	const VkDescriptorSet descriptor_set = bind_group_storage.descriptor_set;
	
	Vector<VkDescriptorImageInfo> image_infos(128);
	std::size_t image_info_index = 0;
	
	Vector<VkDescriptorBufferInfo> buffer_infos(entries.size());
	std::size_t buffer_info_index = 0;
	
	Vector<VkWriteDescriptorSet> write_descriptor_sets;
	
	u32 binding = 0;
	for (const BindGroupEntryDescriptor &entry : entries) {
		VkWriteDescriptorSet write_descriptor_set;
		switch (entry.resource.type) {
		case BindingType::eUniformBuffer:
		case BindingType::eStorageBuffer: {
			const auto buffer_binding = std::get<BindingResource::BufferBinding>(entry.resource.binding);
			const VkDescriptorBufferInfo buffer_info = {
				.buffer = get_buffer(buffer_binding.buffer),
				.offset = buffer_binding.offset,
				.range = buffer_binding.size
			};
			buffer_infos[buffer_info_index] = buffer_info;
			
			write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding.value_or(binding),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = nullptr,
				.pBufferInfo = &buffer_infos[buffer_info_index],
				.pTexelBufferView = nullptr
			};
			write_descriptor_sets.push_back(write_descriptor_set);
			buffer_info_index++;
			break;
		}
		case BindingType::eSampler: {
			const BindingResource::SamplerBinding sampler = std::get<BindingResource::SamplerBinding>(
				entry.resource.binding
			);
			const VkDescriptorImageInfo sampler_info = {
				.sampler = get_sampler(sampler.sampler),
				.imageView = VK_NULL_HANDLE,
				.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
			};
			image_infos[image_info_index] = sampler_info;

			write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding.value_or(binding),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos[image_info_index],
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};

			write_descriptor_sets.push_back(write_descriptor_set);
			image_info_index++;
			break;
		}
		case BindingType::eSampledImage: {
			if (entry.resource.binding.index() == 5) { // vector
				const Vector<BindingResource::ImageBinding> &image_binding = std::get<Vector<BindingResource::ImageBinding>>(entry.resource.binding);
				Vector<VkDescriptorImageInfo> image_bindings(image_binding.size());
				for (std::size_t i = 0; i < image_binding.size(); ++i) {
					image_bindings[i] = {
						.imageView = get_image_view(image_binding[i].image_view),
						.imageLayout = vk::detail::convert(image_binding[i].layout)
					};
					bind_group_storage.sampled_images[sampled_images_index + i] = {
						.image_view = image_binding[i].image_view,
						.image_layout = image_binding[i].layout
					};
					image_infos[image_info_index + i] = image_bindings[i];
				}

				write_descriptor_set = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = descriptor_set,
					.dstBinding = entry.binding.value_or(binding),
					.dstArrayElement = 0,
					.descriptorCount = static_cast<uint32_t>(image_bindings.size()),
					.descriptorType = vk::detail::convert(entry.resource.type),
					.pImageInfo = &image_infos[image_info_index],
					.pBufferInfo = nullptr,
					.pTexelBufferView = nullptr
				};
			
				write_descriptor_sets.push_back(write_descriptor_set);
				image_info_index += image_binding.size();
				sampled_images_index += image_binding.size();
			}
			else {
				const BindingResource::ImageBinding image_binding = std::get<BindingResource::ImageBinding>(entry.resource.binding);
				const VkDescriptorImageInfo image_info{
					.imageView = get_image_view(image_binding.image_view),
					.imageLayout = vk::detail::convert(image_binding.layout)
				};
				image_infos[image_info_index] = image_info;
				bind_group_storage.sampled_images[sampled_images_index] = {
					.image_view = image_binding.image_view,
					.image_layout = image_binding.layout
				};

				write_descriptor_set = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = descriptor_set,
					.dstBinding = entry.binding.value_or(binding),
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = vk::detail::convert(entry.resource.type),
					.pImageInfo = &image_infos[image_info_index],
					.pBufferInfo = nullptr,
					.pTexelBufferView = nullptr
				};
			
				write_descriptor_sets.push_back(write_descriptor_set);
				++image_info_index;
				++sampled_images_index;
			}
			break;
		}
		case BindingType::eStorageImage: {
			const BindingResource::ImageBinding image_binding = std::get<BindingResource::ImageBinding>(entry.resource.binding);
			const VkDescriptorImageInfo image_info{
				.imageView = get_image_view(image_binding.image_view),
				.imageLayout = vk::detail::convert(image_binding.layout)
			};
			image_infos[image_info_index] = image_info;
			bind_group_storage.image_load_stores[image_load_store_index] = {
				image_binding.image_view,
				image_binding.layout
			};

			write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding.value_or(binding),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos[image_info_index],
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			
			write_descriptor_sets.push_back(write_descriptor_set);
			++image_info_index;
			++image_load_store_index;
			break;
		}
		case BindingType::eImageSampler: {
			const auto combined_binding = std::get<BindingResource::CombinedImageSampler>(entry.resource.binding);
			const VkDescriptorImageInfo combined_info = {
				.sampler = get_sampler(combined_binding.sampler),
				.imageView = get_image_view(combined_binding.image_view),
				.imageLayout = vk::detail::convert(combined_binding.layout)
			};
			image_infos[image_info_index] = combined_info;
			bind_group_storage.sampled_images[sampled_images_index] = {
				combined_binding.image_view,
				combined_binding.layout
			};
			
			write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding.value_or(binding),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos[image_info_index],
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			write_descriptor_sets.push_back(write_descriptor_set);
			++image_info_index;
			++sampled_images_index;
			break;
		}
		}
		binding = entry.binding.value_or(binding + 1);
	}

	vkUpdateDescriptorSets(
		device_, 
		static_cast<uint32_t>(write_descriptor_sets.size()), 
		write_descriptor_sets.data(), 
		0, 
		nullptr
	);
}

/**
 * \brief Equivalent to vkCmdBindDescriptorSets[2]. Bind groups are an agnostic term for DescriptorSets & similar functions in other render apis
 * \param command_rid 
 * \param pipeline_layout_rid 
 * \param index 
 * \param bind_group_rid
 * \param stage 
 */
void VkGraphicsDriverBackend::set_bind_group(const RID command_rid, const RID pipeline_layout_rid, const u32 index,
									   const RID bind_group_rid, const ShaderStage stage) {
	
	VkDescriptorSet descriptor_set = get_bind_group(bind_group_rid);

	const VkBindDescriptorSetsInfo bind_descriptor_sets_info{
		.sType = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
		.pNext = nullptr,
		.stageFlags = vk::detail::convert(stage),
		.layout = get_pipeline_layout(pipeline_layout_rid),
		.firstSet = index,
		.descriptorSetCount = 1,
		.pDescriptorSets = &descriptor_set,
		.dynamicOffsetCount = 0,
		.pDynamicOffsets = nullptr,
	};
	
	vkCmdBindDescriptorSets2(get_command_buffer(command_rid), &bind_descriptor_sets_info);
}

void VkGraphicsDriverBackend::set_bind_groups(const RID command_rid, const RID pipeline_layout_rid, const u32 first_index, const Vector<RID> bind_groups, const ShaderStage stage) {
	
	Vector<VkDescriptorSet> descriptor_sets(bind_groups.size());
	
	std::size_t images = 0;
	for (const RID bind_group_rid : bind_groups) {
		const vulkan::DescriptorSetStorage &bind_group_storage = get_bind_group_storage(bind_group_rid);
		images += bind_group_storage.image_load_stores.size();
		images += bind_group_storage.sampled_images.size();
	}
	Vector<ImageTransitionDescriptor> image_transitions(images);
	
	std::size_t image_transition_index = 0;
	
	for (std::size_t i = 0; i < bind_groups.size(); ++i) {
		vulkan::DescriptorSetStorage &bind_group_storage = get_bind_group_storage(bind_groups[i]);
		descriptor_sets[i] = bind_group_storage.descriptor_set;
		
		for (const auto &image_load_store : bind_group_storage.image_load_stores) {
			vulkan::ImageStorage &image_storage = get_image_storage(get_source_from_view(image_load_store.image_view));
			const VkImageMemoryBarrier2 image_transition{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.pNext = nullptr,
				.srcStageMask = vk::detail::convert(image_storage.stage),
				.srcAccessMask = vk::detail::convert(image_storage.access),
				.dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
				.dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT,
				.oldLayout = vk::detail::convert(image_storage.layout),
				.newLayout = vk::detail::convert(image_load_store.image_layout),
				.image = image_storage.image,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = std::max(image_storage.level_count, 1u),
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};
			image_storage.layout = image_load_store.image_layout;
			image_storage.access = Access::eShaderWrite | Access::eShaderRead;
			//image_transitions[image_transition_index] = image_transition;
			//++image_transition_index;
		}
		
		for (const auto &sampled_image : bind_group_storage.sampled_images) {
			using namespace vk::detail;
			vulkan::ImageViewStorage &image_view_storage = get_image_view_storage(sampled_image.image_view);
			vulkan::ImageStorage &image_storage = get_image_storage(image_view_storage.image);

			const gfx::ImageLayout old_layout = image_storage.layout;
			const BitFlag<gfx::Access> current_access = image_storage.access;
			const BitFlag<gfx::PipelineStage> current_stage = image_storage.stage;
			image_transitions[image_transition_index] = ImageTransitionDescriptor{
				.image = image_view_storage.image,
				.src = {
					.layout = old_layout,
					.access = current_access,
					.stage = current_stage
				},
				.dst = {
					.layout = sampled_image.image_layout,
					.access = Access::eShaderSampledRead,
					.stage = PipelineStage::eAllGraphics
				},
				.subresource = ImageSubresourceDescriptor{
					.aspect_mask = format_to_aspect(image_storage.format),
					.base_mip_level = 0,
					.level_count = image_storage.level_count,
					.base_array_layer = 0,
					.layer_count = image_storage.layer_count
				}
			};
			++image_transition_index;
		}
	}
	
	transition(command_rid, image_transitions);
	
	const VkBindDescriptorSetsInfo bind_descriptor_sets_info{
		.sType = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
		.pNext = nullptr,
		.stageFlags = vk::detail::convert(stage),
		.layout = get_pipeline_layout(pipeline_layout_rid),
		.firstSet = first_index,
		.descriptorSetCount = static_cast<uint32_t>(descriptor_sets.size()),
		.pDescriptorSets = descriptor_sets.data(),
		.dynamicOffsetCount = 0,
		.pDynamicOffsets = nullptr,
	};

	vkCmdBindDescriptorSets2(get_command_buffer(command_rid), &bind_descriptor_sets_info);
}

vk::DescriptorSet VkGraphicsDriverBackend::get_bind_group(const RID id) const {
	const vulkan::DescriptorSetStorage* storage = descriptor_sets_.get(id);
	return storage->descriptor_set;
}

const vulkan::DescriptorSetStorage & VkGraphicsDriverBackend::get_bind_group_storage(const RID id) const {
	const vulkan::DescriptorSetStorage* storage = descriptor_sets_.get(id);
	return *storage;
}

vulkan::DescriptorSetStorage & VkGraphicsDriverBackend::get_bind_group_storage(const RID id) {
	vulkan::DescriptorSetStorage* storage = descriptor_sets_.get(id);
	return *storage;
}

RID VkGraphicsDriverBackend::create_shader(const SpirvDescriptor &spirv_descriptor) {
	const VkShaderModuleCreateInfo shader_module_create_info = VkShaderModuleCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr, .flags = 0,
		.codeSize = spirv_descriptor.code_size,
		.pCode = spirv_descriptor.code
	};

	VkShaderModule shader_module;
	vkCheck(vkCreateShaderModule(device_, &shader_module_create_info, nullptr, &shader_module), "Failed to create shader module");
#ifdef _DEBUG
	if (spirv_descriptor.label.has_value()) {
		apply_label(this, spirv_descriptor.label.value(), shader_module);
	}
#endif
	
	return shader_modules_.emplace(vulkan::ShaderStorage{shader_module, VK_NULL_HANDLE});
}

void VkGraphicsDriverBackend::destroy_shader(const RID id) {
	const vk::ShaderModule shader_module = get_shader_module(id);
	vkDestroyShaderModule(device_, shader_module, nullptr);
	assert(shader_modules_.erase(id));
}

vk::ShaderModule VkGraphicsDriverBackend::get_shader_module(const RID id) const {
	const vulkan::ShaderStorage* storage = shader_modules_.get(id);
	return storage->shader_module;
}

vk::ShaderEXT VkGraphicsDriverBackend::get_shader(const RID id) const {
	const vulkan::ShaderStorage* storage = shader_modules_.get(id);
	return storage->shader_ext;
}

namespace detail {
	static gfx::Format get_format_from_color_space(const Vector<vk::SurfaceFormatKHR> &surface_formats, vk::ColorSpaceKHR const &colorSpace) {
		assert(!surface_formats.empty() && "Surface formats must not be empty");
		for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
			if (surface_format.colorSpace == colorSpace)
				return vk::detail::revert(static_cast<VkFormat>(surface_format.format));
	
		return vk::detail::revert(static_cast<VkFormat>(surface_formats[0].format));
	}

	static vk::ColorSpaceKHR get_color_space_from_format(const Vector<vk::SurfaceFormatKHR> &surface_formats, vk::Format const &format) {
		assert(!surface_formats.empty() && "Surface formats must not be empty");
		for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
			if (surface_format.format == format)
				return surface_format.colorSpace;
	
		return surface_formats[0].colorSpace;
	}

	[[maybe_unused]] static vk::Format optimal_depth_formats[] = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD16UnormS8Uint
	};

	constexpr std::array<std::tuple<vk::PresentModeKHR, i32>, 7> present_mode_rankings = {
		std::make_tuple(vk::PresentModeKHR::eImmediate, 10),
		std::make_tuple(vk::PresentModeKHR::eMailbox, 100),
		std::make_tuple(vk::PresentModeKHR::eFifo, 20),
		std::make_tuple(vk::PresentModeKHR::eFifoRelaxed, 25)
	};
}

RID VkGraphicsDriverBackend::create_fence(const Optional<String> &label, const bool signaled) {
	const VkFenceCreateInfo fence_create_info = VkFenceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
	};
	
	VkFence fence;
	vkCheck(vkCreateFence(device_, &fence_create_info, nullptr, &fence), "Failed to create fence");
	
#ifdef _DEBUG
	if (label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_FENCE,
			.objectHandle = reinterpret_cast<uint64_t>(fence),
			.pObjectName = label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif
	
	return fences_.emplace(fence);
}

void VkGraphicsDriverBackend::destroy_fence(const RID fence_rid) {
	const vk::Fence fence = get_fence(fence_rid);
	vkDestroyFence(device_, fence, nullptr);
	assert(fences_.erase(fence_rid));
}

vk::Fence VkGraphicsDriverBackend::get_fence(const RID id) const {
	const vk::Fence* fence = fences_.get(id);
	return *fence;
}

RID VkGraphicsDriverBackend::create_semaphore(const SemaphoreType semaphore_type, const Optional<String> &label) {
	const VkSemaphoreTypeCreateInfo semaphore_type_create_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
		.pNext = nullptr,
		.semaphoreType = semaphore_type == SemaphoreType::eBinary ? VK_SEMAPHORE_TYPE_BINARY : VK_SEMAPHORE_TYPE_TIMELINE,
		.initialValue = 0u
	};
	
	const VkSemaphoreCreateInfo semaphore_create_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = &semaphore_type_create_info,
	};

	VkSemaphore semaphore;
	vkCheck(vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &semaphore), "Failed to create semaphore");

#ifdef _DEBUG
	if (label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SEMAPHORE,
			.objectHandle = reinterpret_cast<uint64_t>(semaphore),
			.pObjectName = label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif

	return semaphores_.emplace(semaphore);
}

void VkGraphicsDriverBackend::destroy_semaphore(const RID semaphore_rid) {
	const vk::Semaphore semaphore = get_semaphore(semaphore_rid);
	vkDestroySemaphore(device_, semaphore, nullptr);
	assert(semaphores_.erase(semaphore_rid));
}

vk::Semaphore VkGraphicsDriverBackend::get_semaphore(const RID id) const {
	const vk::Semaphore* semaphore = semaphores_.get(id);
	return *semaphore;
}

vk::Buffer VkGraphicsDriverBackend::get_buffer(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id);
	return storage->buffer;
}

VmaAllocation VkGraphicsDriverBackend::get_buffer_allocation(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id);
	return storage->allocation;
}

VmaAllocationInfo VkGraphicsDriverBackend::get_buffer_allocation_info(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id);
	VmaAllocationInfo allocation_info;
	vmaGetAllocationInfo(allocator_, storage->allocation, &allocation_info);
	return allocation_info;
}

RID VkGraphicsDriverBackend::create_surface(IWindow *window, const SurfaceDescriptor &desc) {
	switch (window->get_driver()) {
	case WindowDriver::eSdl2:
		return create_surface_sdl2(dynamic_cast<SDL2Window *>(window), desc);
	case WindowDriver::eGlfw3:
		return create_surface_glfw3(dynamic_cast<GLFW3Window *>(window), desc);
	}
	throw std::exception("Unsupported window driver");
}

RID VkGraphicsDriverBackend::create_surface_universal(IWindow *window, const VkSurfaceKHR surface, const SurfaceDescriptor &desc) {
#ifdef _DEBUG
	if (desc.label.has_value())
		apply_label(this, desc.label.value(), surface);
#endif
	
	assert(surface != VK_NULL_HANDLE && "Failed to create window surface");
	const ivec2 size = window->get_size();
	VkExtent2D image_extent = {
		.width = static_cast<uint32_t>(size.x),
		.height = static_cast<uint32_t>(size.y)
	};

	const vk::CommandBufferAllocateInfo command_buffer_allocate_info = vk::CommandBufferAllocateInfo()
		.setCommandPool(command_pool_)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount(frames_in_flight);
	
	Vector<vk::CommandBuffer> allocation = device_.allocateCommandBuffers(command_buffer_allocate_info);
	Array<RID, frames_in_flight> command_buffers;
	const Array<RID, frames_in_flight> render_finished_semaphores;
	const Array<RID, frames_in_flight> image_available_semaphores;
	const Array<RID, frames_in_flight> graphics_fences;
	
	for (size_t i = 0; i < allocation.size(); ++i)
		command_buffers[i] = command_buffers_.emplace(allocation[i]);

	// Upload half complete Surface then call update_surface_configuration to create swapchain,
	// update_surface_configuration can be used 
	const SlotPool<vulkan::SurfaceStorage>::Handle resource = surfaces_.emplace(vulkan::SurfaceStorage{
		.surface = surface,
		.swapchain = VK_NULL_HANDLE,
		.window = window,
		.swapchain_images = {},
		.swapchain_image_views = {},
		.graphics_command_buffers = command_buffers,
		.render_finished_semaphores = render_finished_semaphores,
		.image_available_semaphores = image_available_semaphores,
		.graphics_fences = graphics_fences,
		.frame_index = 0,
		.image_index = 0
	});
	
	update_surface_configuration(resource, desc);
	return resource;
}

RID VkGraphicsDriverBackend::create_surface_sdl2(SDL2Window *window, const SurfaceDescriptor &desc) {
	SDL_Window* sdl_window = window->get_sdl2_window();
	VkSurfaceKHR surface;
	assert(SDL_Vulkan_CreateSurface(sdl_window, instance_, &surface)&&"Failed to create window surface");
	return create_surface_universal(window, surface, desc);
}

RID VkGraphicsDriverBackend::create_surface_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc) {
	GLFWwindow* glfw_window = window->get_glfw3_window();
	VkSurfaceKHR surface;
	vkCheck(glfwCreateWindowSurface(instance_, glfw_window, nullptr, &surface), "Failed to create a surface from GLFW3");
	return create_surface_universal(window, surface, desc);
}

Vector<gfx::Format> VkGraphicsDriverBackend::get_surface_formats(const RID surface_rid) {
	return {}; // TODO
}

gfx::Format VkGraphicsDriverBackend::get_surface_color_format(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.color_format;
}

RID VkGraphicsDriverBackend::get_active_image(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_images[storage.image_index];
}

RID VkGraphicsDriverBackend::get_active_image_view(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_image_views[storage.image_index];
}

void VkGraphicsDriverBackend::update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) {
	
	device_.waitIdle();
	
	vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	vk::SurfaceKHR surface_khr = storage.surface;

	std::vector<vk::SurfaceFormatKHR> surface_formats = adapter_.getSurfaceFormatsKHR(surface_khr);
	
	const ivec2 size = storage.window->get_size();
	VkExtent2D image_extent = {
		.width = static_cast<uint32_t>(size.x),
		.height = static_cast<uint32_t>(size.y)
	};
	
	vk::Format color_format;
	vk::ColorSpaceKHR color_space;
	vk::PresentModeKHR present_mode;

	Optional<PresentMethod> target_present_mode = desc.present_method;
	Optional<gfx::Format> target_color_format = desc.format;
	Optional<ColorSpace> target_color_space = desc.color_space;
	
	const vk::SurfaceCapabilitiesKHR surface_capabilities = adapter_.getSurfaceCapabilitiesKHR(surface_khr);
	storage.extent = vk::Extent2D(surface_capabilities.currentExtent.width,
	                              surface_capabilities.currentExtent.height);
	
	if (!target_present_mode.has_value())
		target_present_mode = PresentMethod::eMailbox;
	
	// Choose a presentation mode.
	i32 current_presentation_mode_score = 20;
	if (!target_present_mode.has_value()) {
		const std::vector<vk::PresentModeKHR> present_modes = adapter_.getSurfacePresentModesKHR(surface_khr);
		present_mode = vk::PresentModeKHR::eMailbox;

		for (vk::PresentModeKHR mode : present_modes)
			if (mode == vk::PresentModeKHR::eMailbox)
				present_mode = mode;
	}
	else {
		present_mode = static_cast<vk::PresentModeKHR>(vk::detail::convert(target_present_mode.value()));
	}

	// Choose a Format + Colorspace
	if (target_color_space.has_value()) {
		color_space = static_cast<vk::ColorSpaceKHR>(target_color_space.value());
		color_format = static_cast<vk::Format>(
			vk::detail::convert(target_color_format.value_or(detail::get_format_from_color_space(surface_formats, color_space)))
		);
	}
	else if (target_color_format.has_value()) {
		color_format = static_cast<vk::Format>(target_color_format.value());
		color_space = detail::get_color_space_from_format(surface_formats, color_format);
	}
	else {
		// If nothing is specified, just fallback to some safe options.
		color_format = vk::Format::eB8G8R8A8Srgb;
		color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
	}
	VkSwapchainCreateInfoKHR swapchain_create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = storage.surface,
		.minImageCount = frames_in_flight,
		.imageFormat = vk::detail::convert(target_color_format.value_or(gfx::Format::eRgba8Srgb)),
		.imageColorSpace = vk::detail::convert(target_color_space.value_or(ColorSpace::eSrgbNonLinear)),
		.imageExtent = image_extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::detail::convert(desc.usage.value_or(ImageUsage::eColorAttachment)),
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = vk::detail::convert(desc.composite_alpha.value_or(CompositeAlpha::eOpaque)),
		.presentMode = vk::detail::convert((PresentMethod::eFifo)), // desc.present_method.value_or
		.oldSwapchain = storage.swapchain
	};
	storage.color_format = vk::detail::revert(static_cast<VkFormat>(color_format));
	
	for (const RID fence_rid : storage.graphics_fences) {
		if (!fence_rid.valid()) continue;
		vk::Fence fence = get_fence(fence_rid);
		vk::detail::resultCheck(device_.waitForFences(fence, vk::True, UINT64_MAX), "Failed to wait for fence");
	}
	
	if (storage.swapchain != VK_NULL_HANDLE) {
		
		for (const RID image_view_rid : storage.swapchain_image_views)
			destroy_image_view(image_view_rid);
		device_.destroySwapchainKHR(storage.swapchain);
	}
	vk::SwapchainKHR swapchain = device_.createSwapchainKHR(swapchain_create_info);
	
#ifdef _DEBUG
	if (desc.label.has_value())
		apply_label(this, desc.label.value(), swapchain);
#endif
	
	const Vector<vk::Image> swapchain_images = device_.getSwapchainImagesKHR(swapchain);
	Vector<RID> images; // (swapchain_images.size());
	size_t tick = 0;
	for (const vk::Image& image : swapchain_images) {
		apply_label(this, "Swapchain Image " + std::to_string(tick++), image);
		
		const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
			.image = image,
			.allocation = VK_NULL_HANDLE,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb)
		});
		images.push_back(handle);
	}
	
	Vector<RID> views(images.size());
	for (size_t i = 0; i < images.size(); ++i) {
		const RID image_rid = images[i];
		ImageViewDescriptor imageViewDescriptor{
			.image = image_rid,
			.type = ImageViewType::e2D,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb),
			.usage = ImageUsage::eColorAttachment,
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = Aspect::eColor,
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}
		};
		views[i] = create_image_view(imageViewDescriptor);
	}
	Array<RID, frames_in_flight> render_finished_semaphores;
	Array<RID, frames_in_flight> image_available_semaphores;
	Array<RID, frames_in_flight> graphics_fences;
	for (size_t i = 0; i < frames_in_flight; ++i) {
		render_finished_semaphores[i] = create_semaphore(SemaphoreType::eBinary, "Render Finished Semaphore" + std::to_string(i));
		image_available_semaphores[i] = create_semaphore(SemaphoreType::eBinary, "Image Available Semaphore" + std::to_string(i));
		graphics_fences[i] = create_fence("Graphics Fence" + std::to_string(i), true);
	}
	storage.render_finished_semaphores = render_finished_semaphores;
	storage.image_available_semaphores = image_available_semaphores;
	storage.graphics_fences = graphics_fences;
	
	storage.swapchain = swapchain;
	storage.swapchain_images = images;
	storage.swapchain_image_views = views;
	storage.color_format = target_color_format.value_or(gfx::Format::eRgba8Srgb);
}

void VkGraphicsDriverBackend::destroy_surface(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);

	if (storage.swapchain != VK_NULL_HANDLE) {
		for (const RID image_view_rid : storage.swapchain_image_views)
			destroy_image_view(image_view_rid);
		for (u32 i = 0; i < storage.graphics_command_buffers.size(); ++i) {
			
			vk::Fence fence = get_fence(storage.graphics_fences[i]);
			vk::detail::resultCheck(device_.waitForFences(fence, vk::True, UINT64_MAX), "Failed to wait for fence");
			
			destroy_semaphore(storage.render_finished_semaphores[i]);
			destroy_semaphore(storage.image_available_semaphores[i]);
			destroy_fence(storage.graphics_fences[i]);
		}
		device_.destroySwapchainKHR(storage.swapchain);
	}
	instance_.destroySurfaceKHR(storage.surface);
	assert(surfaces_.erase(surface_rid));
}

const vulkan::SurfaceStorage & VkGraphicsDriverBackend::get_surface_storage(const RID id) const {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id);
	return *storage;
}

vulkan::SurfaceStorage & VkGraphicsDriverBackend::get_surface_storage(const RID id) {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id);
	return *const_cast<vulkan::SurfaceStorage*>(storage);
}

RID VkGraphicsDriverBackend::create_pipeline_layout(const PipelineLayoutDescriptor &desc) {
	auto pipeline_layout_create_info = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount(static_cast<uint32_t>(desc.bind_group_layouts.size()))
		.setPushConstantRangeCount(static_cast<uint32_t>(desc.push_constants.size()));
	
	Vector<vk::DescriptorSetLayout> descriptor_set_layouts(desc.bind_group_layouts.size());
	for (size_t i = 0; i < desc.bind_group_layouts.size(); ++i) {
		const vk::DescriptorSetLayout layout = get_bind_group_layout(desc.bind_group_layouts[i]);
		descriptor_set_layouts[i] = layout;
	}
	pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
	
	Vector<vk::PushConstantRange> push_constant_ranges(desc.push_constants.size());
	for (size_t i = 0; i < desc.push_constants.size(); ++i) {
		const PushConstantRangeDescriptor &push_constant_range = desc.push_constants[i];
		const auto range = vk::PushConstantRange()
			.setStageFlags(vk::detail::convert_hpp(push_constant_range.visibility))
			.setOffset(push_constant_range.offset)
			.setSize(push_constant_range.size);
		push_constant_ranges[i] = range;
	}
	pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();
	
	vk::PipelineLayout pipeline_layout = device_.createPipelineLayout(pipeline_layout_create_info);
	
#ifdef _DEBUG
	if (desc.label.has_value())
		apply_label(this, desc.label.value(), pipeline_layout);
#endif
	
	return pipeline_layouts_.emplace(pipeline_layout);
}

void VkGraphicsDriverBackend::destroy_pipeline_layout(const RID pipeline_layout_rid) {
	const vk::PipelineLayout pipeline_layout = get_pipeline_layout(pipeline_layout_rid);
	device_.destroyPipelineLayout(pipeline_layout);
	assert(pipeline_layouts_.erase(pipeline_layout_rid));
}

vk::PipelineLayout VkGraphicsDriverBackend::get_pipeline_layout(const RID rid) {
	const vk::PipelineLayout* pipeline_layout = pipeline_layouts_.get(rid);
	return *pipeline_layout;
}

RID VkGraphicsDriverBackend::create_pipeline(const GraphicsPipelineDescriptor &desc) {
	
	Vector<vk::PipelineShaderStageCreateInfo> stages; // `(desc.stages.size());
	for (const auto& stage_desc : desc.stages)
		stages.push_back(vk::PipelineShaderStageCreateInfo()
			.setStage(vk::detail::convert2_hpp(stage_desc.stage))
			.setModule(get_shader_module(stage_desc.shader))
			.setPName(stage_desc.entry_point.c_str()));

	Vector<vk::VertexInputBindingDescription> binding_descriptions; // (desc.vertex_input.bindings.size());
	for (const auto& binding_desc : desc.vertex_input.bindings)
		binding_descriptions.push_back(vk::VertexInputBindingDescription()
			.setBinding(binding_desc.binding)
			.setStride(binding_desc.stride)
			.setInputRate(vk::detail::convert_hpp(binding_desc.input_rate)));

	Vector<vk::VertexInputAttributeDescription> attribute_descriptions; // (desc.vertex_input.attributes.size());
	for (const auto& attribute_desc : desc.vertex_input.attributes)
		attribute_descriptions.push_back(vk::VertexInputAttributeDescription()
			.setLocation(attribute_desc.location)
			.setBinding(attribute_desc.binding)
			.setFormat(vk::detail::convert_hpp(attribute_desc.format))
			.setOffset(attribute_desc.offset));
	
	auto vertex_input_states = vk::PipelineVertexInputStateCreateInfo()
		.setVertexBindingDescriptions(binding_descriptions)
		.setVertexAttributeDescriptions(attribute_descriptions);
	
	auto input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(vk::detail::convert_hpp(desc.input_assembly.primitive_topology))
		.setPrimitiveRestartEnable(desc.input_assembly.primitive_restart_enable ? VK_TRUE : VK_FALSE);
	
	Vector<vk::Viewport> viewports(desc.viewport.viewports.size());
	for (auto i = 0; i < desc.viewport.viewports.size(); ++i) {
		const Viewport viewport_desc = desc.viewport.viewports[i];
		viewports[i] = vk::Viewport()
			.setX(viewport_desc.x)
			.setY(viewport_desc.y)
			.setWidth(viewport_desc.width)
			.setHeight(viewport_desc.height)
			.setMinDepth(viewport_desc.min_depth)
			.setMaxDepth(viewport_desc.max_depth);
	}
	
	Vector<vk::Rect2D> scissors(desc.viewport.scissors.size());
	for (auto i = 0; i < desc.viewport.scissors.size(); ++i) {
		const Rect2D scissor_desc = desc.viewport.scissors[i];
		scissors[i] = vk::Rect2D()
			.setOffset(vk::Offset2D()
				.setX(scissor_desc.offset.x)
				.setY(scissor_desc.offset.y))
			.setExtent(vk::Extent2D()
				.setWidth(scissor_desc.extent.width)
				.setHeight(scissor_desc.extent.height));
	}
	
	auto viewport_state_create_info = vk::PipelineViewportStateCreateInfo()
		.setViewports(viewports)
		.setScissors(scissors);
	
	auto rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(desc.rasterization.depth_clamp_enable ? VK_TRUE : VK_FALSE)
		.setRasterizerDiscardEnable(desc.rasterization.rasterizer_discard_enable ? VK_TRUE : VK_FALSE)
		.setPolygonMode(vk::detail::convert_hpp(desc.rasterization.polygon_mode))
		.setCullMode(vk::detail::convert_hpp(desc.rasterization.cull_mode))
		.setFrontFace(vk::detail::convert_hpp(desc.rasterization.front_face))
		.setDepthBiasEnable(desc.rasterization.depth_bias_enable ? VK_TRUE : VK_FALSE)
		.setDepthBiasConstantFactor(desc.rasterization.depth_bias_constant_factor)
		.setDepthBiasClamp(desc.rasterization.depth_bias_clamp)
		.setDepthBiasSlopeFactor(desc.rasterization.depth_bias_slope_factor)
		.setLineWidth(desc.rasterization.line_width);
	
	auto multi_sample_state_create_info = vk::PipelineMultisampleStateCreateInfo()
		.setRasterizationSamples(vk::detail::convert_hpp(desc.multisample.rasterization_samples))
		.setSampleShadingEnable(desc.multisample.sample_shading_enable ? VK_TRUE : VK_FALSE)
		.setMinSampleShading(desc.multisample.min_sample_shading)
		.setPSampleMask(nullptr)            //< Not implemented
		.setAlphaToCoverageEnable(VK_FALSE) //< Not implemented
		.setAlphaToOneEnable(VK_FALSE);       //< Not implemented
	
	auto depth_stencil_state_create_info = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(desc.depth_stencil.depth_test ? VK_TRUE : VK_FALSE)
		.setDepthWriteEnable(desc.depth_stencil.depth_write ? VK_TRUE : VK_FALSE)
		.setDepthCompareOp(vk::detail::convert_hpp(desc.depth_stencil.depth_compare_op))
		.setDepthBoundsTestEnable(desc.depth_stencil.depth_bounds_test ? VK_TRUE : VK_FALSE)
		.setStencilTestEnable(desc.depth_stencil.stencil_test ? VK_TRUE : VK_FALSE)
		.setFront(vk::StencilOpState()
			.setFailOp(vk::detail::convert_hpp(desc.depth_stencil.front.fail_op))
			.setPassOp(vk::detail::convert_hpp(desc.depth_stencil.front.pass_op))
			.setDepthFailOp(vk::detail::convert_hpp(desc.depth_stencil.front.depth_fail_op))
			.setCompareOp(vk::detail::convert_hpp(desc.depth_stencil.front.compare_op))
			.setCompareMask(desc.depth_stencil.front.compare_mask)
			.setWriteMask(desc.depth_stencil.front.write_mask)
			.setReference(desc.depth_stencil.front.reference))
		.setBack(vk::StencilOpState()
			.setFailOp(vk::detail::convert_hpp(desc.depth_stencil.back.fail_op))
			.setPassOp(vk::detail::convert_hpp(desc.depth_stencil.back.pass_op))
			.setDepthFailOp(vk::detail::convert_hpp(desc.depth_stencil.back.depth_fail_op))
			.setCompareOp(vk::detail::convert_hpp(desc.depth_stencil.back.compare_op))
			.setCompareMask(desc.depth_stencil.back.compare_mask)
			.setWriteMask(desc.depth_stencil.back.write_mask)
			.setReference(desc.depth_stencil.back.reference))
		.setMinDepthBounds(desc.depth_stencil.min_depth_bounds)
		.setMaxDepthBounds(desc.depth_stencil.max_depth_bounds);

	auto color_blend_attachment_state = vk::PipelineColorBlendAttachmentState()
		.setBlendEnable(false)
		.setSrcColorBlendFactor(vk::detail::convert_hpp(desc.blend.src_color_blend_factor))
		.setDstColorBlendFactor(vk::detail::convert_hpp(desc.blend.dst_color_blend_factor))
		.setColorBlendOp(vk::detail::convert_hpp(desc.blend.color_blend_op))
		.setSrcAlphaBlendFactor(vk::detail::convert_hpp(desc.blend.src_alpha_blend_factor))
		.setDstAlphaBlendFactor(vk::detail::convert_hpp(desc.blend.dst_alpha_blend_factor))
		.setAlphaBlendOp(vk::detail::convert_hpp(desc.blend.alpha_blend_op))
		.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	
	auto  color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo()
		.setAttachments({color_blend_attachment_state});
	
	Vector<vk::DynamicState> dynamic_states;//(desc.dynamic_states.size());
	for (const DynamicState &state : desc.dynamic_states) {
		dynamic_states.push_back(vk::detail::convert_hpp(state));
	}
	
	auto dynamic_state_create_info = vk::PipelineDynamicStateCreateInfo()
		.setDynamicStates(dynamic_states);
	
	Vector<vk::Format> color_attachment_formats;//(desc.rendering.color_formats.size());
	for (const auto& color_format : desc.rendering.color_formats)
		color_attachment_formats.push_back(vk::detail::convert_hpp(color_format));

	vk::Format depth_attachment_format = vk::detail::convert_hpp(desc.rendering.depth_format.value_or(gfx::Format::eUndefined));
	vk::Format stencil_attachment_format = vk::detail::convert_hpp(desc.rendering.stencil_format.value_or(gfx::Format::eUndefined));
	
	auto rendering_create_info = vk::PipelineRenderingCreateInfo()
		.setColorAttachmentFormats(color_attachment_formats)
		.setDepthAttachmentFormat(depth_attachment_format)
		.setStencilAttachmentFormat(stencil_attachment_format);
	
	vk::GraphicsPipelineCreateInfo pipeline_create_info = vk::GraphicsPipelineCreateInfo()
		.setPNext(&rendering_create_info)
		.setStages(stages)
		.setPVertexInputState(&vertex_input_states)
		.setPInputAssemblyState(&input_assembly_state_create_info)
		.setPTessellationState(nullptr) //< Not implemented for now.
		.setPViewportState(&viewport_state_create_info)
		.setPRasterizationState(&rasterization_state_create_info)
		.setPMultisampleState(&multi_sample_state_create_info)
		.setPDepthStencilState(&depth_stencil_state_create_info)
		.setPColorBlendState(&color_blend_state_create_info)
		.setPDynamicState(&dynamic_state_create_info)
		.setLayout(get_pipeline_layout(desc.layout));

	vk::ResultValue<vk::Pipeline> pipeline_result = device_.createGraphicsPipeline(nullptr, pipeline_create_info);
	vk::detail::resultCheck(pipeline_result.result, "Failed to create graphics pipeline");
	vk::Pipeline pipeline = pipeline_result.value;
	
	return pipelines_.emplace(pipeline);
}

RID VkGraphicsDriverBackend::create_pipeline(const gfx::ComputePipelineDescriptor &desc) {
	VkComputePipelineCreateInfo compute_pipeline_create_info{
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = vk::detail::convert2(desc.stage.stage),
			.module = get_shader_module(desc.stage.shader),
			.pName = desc.stage.entry_point.c_str()
		},
		.layout = get_pipeline_layout(desc.layout),
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	VkPipeline pipeline;
	vkCheck(vkCreateComputePipelines(device_, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &pipeline), "Failed to create compute pipeline");
	return pipelines_.emplace(pipeline);
}

void VkGraphicsDriverBackend::bind_pipeline(const RID pipeline, const RID cmd_rid, const PipelineBindPoint bind_point) {
	const VkCommandBuffer command_buffer = get_command_buffer(cmd_rid);
	const VkPipeline vk_pipeline = get_pipeline(pipeline);
	vkCmdBindPipeline(command_buffer, vk::detail::convert(bind_point), vk_pipeline);
}

void VkGraphicsDriverBackend::destroy_pipeline(const RID pipeline_rid) {
	const vk::Pipeline pipeline = get_pipeline(pipeline_rid);
	vkDestroyPipeline(device_, pipeline, nullptr);
	assert(pipelines_.erase(pipeline_rid));
}

vk::Pipeline VkGraphicsDriverBackend::get_pipeline(const RID id) const {
	const vk::Pipeline* pipeline = pipelines_.get(id);
	return *pipeline;
}

RID VkGraphicsDriverBackend::begin(const RID surface_rid) {
	using namespace vk::detail;
	vulkan::SurfaceStorage& surface_storage = get_surface_storage(surface_rid);
	const vk::CommandBuffer command = get_command_buffer(surface_storage.graphics_command_buffers[surface_storage.frame_index]);
	const vk::Fence fence = get_fence(surface_storage.graphics_fences[surface_storage.frame_index]);
	const vk::SwapchainKHR swapchain = surface_storage.swapchain;

	switch (vk::Result result = device_.waitForFences(fence, VK_TRUE, UINT64_MAX)) {
	case vk::Result::eSuccess:
		break;
	case vk::Result::eErrorDeviceLost:
		throw std::runtime_error("Device lost while waiting for fence");
		break;
	case vk::Result::eErrorOutOfDateKHR:
		update_surface_configuration(surface_rid, SurfaceDescriptor{
			.label = "i have been recreated and i have forgotten my own name.",
			.size = surface_storage.window->get_size(),
			.minimum_images = frames_in_flight,
			.format = surface_storage.color_format,
			.usage = ImageUsage::eColorAttachment,
			.present_method = PresentMethod::eFifo,
			.color_space = ColorSpace::eSrgbNonLinear
		});
		break;
	default:
		resultCheck(result, "Failed to wait for fence");
		break;
	}
	
	device_.resetFences({fence});
	
	const vk::Semaphore image_available_semaphore = get_semaphore(surface_storage.image_available_semaphores[surface_storage.frame_index]);
	uint32_t* image_index_ptr = &surface_storage.image_index;
	
	vk::AcquireNextImageInfoKHR acquire_info = vk::AcquireNextImageInfoKHR()
		.setSwapchain(swapchain)
		.setTimeout(UINT64_MAX)
		.setSemaphore(image_available_semaphore)
		.setFence(nullptr)
		.setDeviceMask(1);

	const vk::ResultValue<u32> image_index = device_.acquireNextImage2KHR(acquire_info);
	*image_index_ptr = image_index.value;
	
	switch (image_index.result) {  // NOLINT(clang-diagnostic-switch-enum)
	case vk::Result::eErrorOutOfDateKHR:
	case vk::Result::eSuboptimalKHR:
		update_surface_configuration(surface_rid, SurfaceDescriptor{
			.label = "i have been recreated and i have forgotten my own name.",
			.size = surface_storage.window->get_size(),
			.minimum_images = frames_in_flight,
			.format = surface_storage.color_format,
			.usage = ImageUsage::eColorAttachment,
			.present_method = PresentMethod::eFifo,
			.color_space = ColorSpace::eSrgbNonLinear
		});
		break;
	case vk::Result::eSuccess:
		break;
	default:
		resultCheck(image_index.result, "Failed to present swapchain image");
		break;
	}
	
	command.reset();
	
	constexpr vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	
	command.begin(begin_info);
	
	return surface_storage.graphics_command_buffers[surface_storage.frame_index];
}

RID VkGraphicsDriverBackend::begin(QueueFamilyType queue_family) {
	return 0;
}

u32 VkGraphicsDriverBackend::get_frame_index(const RID surface_rid) {
	const vulkan::SurfaceStorage& surface_storage = get_surface_storage(surface_rid);
	return surface_storage.frame_index;
}

void VkGraphicsDriverBackend::begin_rendering(const RID command_rid, const RenderingDescriptor &rendering_descriptor) {
	vk::CommandBuffer command_buffer = get_command_buffer(command_rid);
	Vector<vk::RenderingAttachmentInfo> color_attachments;//(rendering_descriptor.color_attachments.size());
	Vector<vk::ImageMemoryBarrier2> transitions;//(rendering_descriptor.color_attachments.size() + 1); // +2 for depth and stencil
	Vector<RID> color_attachment_image_views;
	
	for (std::size_t i = 0; i < rendering_descriptor.color_attachments.size(); ++i) {
		RenderingAttachmentDescriptor color_attachment = rendering_descriptor.color_attachments[i];
		
		color_attachment_image_views.push_back(color_attachment.image_view);
		
		vulkan::ImageViewStorage* image_view_storage = image_views_.get(color_attachment.image_view);
		vulkan::ImageStorage& image_storage = get_image_storage(image_view_storage->image);
		
		vk::ImageView image_view = image_view_storage->image_view;
		
		assert(image_view != VK_NULL_HANDLE && "Invalid image view");
		
		vk::ImageLayout layout = vk::ImageLayout::eAttachmentOptimal; //vk::detail::convert(ca.layout.value_or(ImageLayout::eColorAttachmentOptimal));
		vk::AttachmentLoadOp load_op = vk::detail::convert_hpp(color_attachment.load_op);
		vk::AttachmentStoreOp store_op = vk::detail::convert_hpp(color_attachment.store_op);
		
		ClearColorValue clear = color_attachment.clear_color.value_or(ClearColorValue{.float32{0.0f, 0.0f, 0.0f, 1.0f}});
		
		color_attachments.push_back(vk::RenderingAttachmentInfo()
			.setImageView(image_view)
			.setImageLayout(layout)
			.setLoadOp(load_op)
			.setStoreOp(store_op)
			.setClearValue(vk::ClearValue()
				.setColor(vk::ClearColorValue()
					.setFloat32({clear.float32[0], clear.float32[1], clear.float32[2], clear.float32[3]}))));
		
		transitions.push_back(vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::detail::convert_hpp(image_storage.stage))
			.setSrcAccessMask(vk::detail::convert_hpp(image_storage.access))
			.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
			.setOldLayout(vk::detail::convert_hpp(image_storage.layout))
			.setNewLayout(layout)
			.setImage(image_storage.image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(std::max(image_storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1)));
		
		image_storage.layout = color_attachment.layout.value_or(ImageLayout::eColorAttachment);
		image_storage.stage = PipelineStage::eColorAttachmentOutput;
		image_storage.access = Access::eColorAttachmentWrite;
	}
	
	vk::RenderingAttachmentInfo depth_attachment;
	RID depth_attachment_image_view = 0;
	
	if (rendering_descriptor.depth_attachment.has_value()) {
		RenderingAttachmentDescriptor depth_attachment_descriptor = rendering_descriptor.depth_attachment.value();
		depth_attachment_image_view = depth_attachment_descriptor.image_view;
		vulkan::ImageViewStorage* depth_image_view_storage = image_views_.get(depth_attachment_descriptor.image_view);
		vulkan::ImageStorage& depth_image_storage = get_image_storage(depth_image_view_storage->image);
		
		vk::ImageLayout depth_image_layout = vk::detail::convert_hpp(depth_attachment_descriptor.layout.value_or(ImageLayout::eDepthStencilAttachment));
		
		ClearDepthStencilValue original_clear_depth_stencil_value = depth_attachment_descriptor.clear_depth_stencil.value_or(ClearDepthStencilValue{1.0f, 0});
		vk::ClearDepthStencilValue clear_depth_stencil_value = vk::ClearDepthStencilValue()
			.setDepth(original_clear_depth_stencil_value.depth)
			.setStencil(original_clear_depth_stencil_value.stencil);
		
		assert(depth_image_layout != vk::ImageLayout::eUndefined);
		
		depth_attachment = vk::RenderingAttachmentInfo()
			.setImageView(depth_image_view_storage->image_view)
			.setImageLayout(depth_image_layout)
			.setLoadOp(vk::detail::convert_hpp(depth_attachment_descriptor.load_op))
			.setStoreOp(vk::detail::convert_hpp(depth_attachment_descriptor.store_op))
			.setClearValue(vk::ClearValue()
				.setDepthStencil(clear_depth_stencil_value));
		
		transitions.push_back(vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::detail::convert_hpp(depth_image_storage.stage))
			.setSrcAccessMask(vk::detail::convert_hpp(depth_image_storage.access))
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eUndefined) // don't care //vk::detail::convert_hpp(depth_image_storage.layout))
			.setNewLayout(depth_image_layout)
			.setImage(depth_image_storage.image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::detail::convert_hpp(format_to_aspect(depth_image_storage.format)))
				.setBaseMipLevel(0)
				.setLevelCount(std::max(depth_image_storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(std::max(depth_image_storage.layer_count, 1u))));
		
		depth_image_storage.layout = depth_attachment_descriptor.layout.value_or(ImageLayout::eDepthStencilAttachment);
		depth_image_storage.access = Access::eDepthStencilAttachmentWrite;
		depth_image_storage.stage = PipelineStage::eEarlyFragmentTests;
	}
	
	vk::RenderingInfo rendering_info = vk::RenderingInfo()
		.setRenderArea(vk::Rect2D()
			.setOffset(vk::Offset2D()
				.setX(rendering_descriptor.render_area.offset.x)
				.setY(rendering_descriptor.render_area.offset.y))
			.setExtent(vk::Extent2D()
				.setWidth(rendering_descriptor.render_area.extent.width)
				.setHeight(rendering_descriptor.render_area.extent.height)))
		.setLayerCount(rendering_descriptor.layer_count)
		.setViewMask(0)
		.setColorAttachments(color_attachments)
		.setPDepthAttachment(&depth_attachment)
		.setPStencilAttachment(nullptr);
	
	vk::DependencyInfo dependency_info = vk::DependencyInfo()
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion)
		.setImageMemoryBarriers(transitions);
	
	assert(!transitions.empty());
	
	command_buffer.pipelineBarrier2(dependency_info);
	command_buffer.beginRendering(rendering_info);
	
	command_buffer.setViewport(0, {
		vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth(static_cast<f32>(rendering_descriptor.render_area.extent.width))
			.setHeight(static_cast<f32>(rendering_descriptor.render_area.extent.height))
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f)
	});
	
	command_buffer.setScissor(0, {
		vk::Rect2D()
			.setOffset(vk::Offset2D{rendering_descriptor.render_area.offset.x, rendering_descriptor.render_area.offset.y})
			.setExtent(vk::Extent2D()
				.setWidth(rendering_descriptor.render_area.extent.width)
				.setHeight(rendering_descriptor.render_area.extent.height)
			)
	});
	
	//if (rendering_descriptor.color_attachments.empty()) __debugbreak(); // breakpoint
	
	vulkan::CommandBufferStorage& command_buffer_storage = get_command_buffer_storage(command_rid);
	command_buffer_storage.rendering_descriptor = rendering_descriptor;
	
	command_buffer_storage.rendering = true;
	command_buffer_storage.color_attachments_image_views = color_attachment_image_views;
	command_buffer_storage.depth_attachment_image_view = depth_attachment_image_view;
}

uint32_t VkGraphicsDriverBackend::begin_rendering(const RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) {
	vulkan::CommandBufferStorage &command_storage = get_command_buffer_storage(command_rid);
	command_storage.connected_surface = surface_rid;
	
	begin_rendering(command_rid, RenderingDescriptor{
		.color_attachments = {
			RenderingAttachmentDescriptor{
				.image_view = get_surface_storage(surface_rid).swapchain_image_views[get_surface_storage(surface_rid).image_index],
				.layout = ImageLayout::eColorAttachment,
				.load_op = LoadOp::eClear,
				.store_op = StoreOp::eStore,
				.clear_color = ClearColorValue{.float32{0.0f, 0.0f, 0.0f, 1.0f}}
			}
		},
		.depth_attachment = depth_image_view.valid() ? Optional(RenderingAttachmentDescriptor{
			.image_view = depth_image_view,
			.layout = ImageLayout::eDepthStencilAttachment,
			.load_op = LoadOp::eClear,
			.store_op = StoreOp::eDontCare,
			.clear_depth_stencil = ClearDepthStencilValue{1.0f, 0}
		}) : std::nullopt,
		.render_area = Rect2D{
			.offset = {0, 0},
			.extent = get_surface_storage(surface_rid).window->get_extent_2d()
		}
	});
	
	command_storage.command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, get_pipeline(pipeline_rid));
	return 0;
}

void VkGraphicsDriverBackend::finish_rendering(const RID command_rid, const bool for_presenting) {
	vulkan::CommandBufferStorage &command_storage = get_command_buffer_storage(command_rid);
	const vk::CommandBuffer command_buffer = command_storage.command_buffer;
	command_buffer.endRendering();
	
	const ImageLayout target_color_layout = command_storage.connected_surface.has_value() ? ImageLayout::ePresent : ImageLayout::eAttachment;
	
	Vector<vk::ImageMemoryBarrier2> image_transitions;
	
	for (const RID color_attachment_image_view : command_storage.color_attachments_image_views) {
		const vulkan::ImageViewStorage *image_view_storage = image_views_.get(color_attachment_image_view);
		vulkan::ImageStorage &image_storage = get_image_storage(image_view_storage->image);

		vk::ImageMemoryBarrier2 transition_descriptor = vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::detail::convert_hpp(image_storage.stage))
			.setSrcAccessMask(vk::detail::convert_hpp(image_storage.access))
			.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits2::eNone)
			.setOldLayout(vk::detail::convert_hpp(image_storage.layout))
			.setNewLayout(for_presenting ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eColorAttachmentOptimal)
			.setImage(image_storage.image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(std::max(image_storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1));
		
		image_transitions.push_back(transition_descriptor);

		image_storage.layout = for_presenting ? ImageLayout::ePresent : ImageLayout::eColorAttachment;
		image_storage.access = Access::eNone;
		image_storage.stage = PipelineStage::eColorAttachmentOutput;
	}
	const vulkan::ImageViewStorage &depth_image_view_storage = get_image_view_storage(command_storage.depth_attachment_image_view);
	const vulkan::ImageStorage &depth_image_storage = get_image_storage(depth_image_view_storage.image);
	
	if (has_stencil(depth_image_storage.format)) {
		image_transitions.push_back(vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::detail::convert_hpp(depth_image_storage.stage))
			.setSrcAccessMask(vk::detail::convert_hpp(depth_image_storage.access))
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eNone)
			.setOldLayout(vk::detail::convert_hpp(depth_image_storage.layout))
			.setNewLayout(vk::detail::convert_hpp(command_storage.rendering_descriptor.depth_attachment.value().layout.value_or(ImageLayout::eAttachment)))
			.setImage(depth_image_storage.image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::detail::convert_hpp(format_to_aspect(depth_image_storage.format)))
				.setBaseMipLevel(0)
				.setLevelCount(std::max(depth_image_storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1)));
	}
	else {
		image_transitions.push_back(vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::detail::convert_hpp(depth_image_storage.stage))
			.setSrcAccessMask(vk::detail::convert_hpp(depth_image_storage.access))
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eNone)
			.setOldLayout(vk::detail::convert_hpp(depth_image_storage.layout))
			.setNewLayout(vk::ImageLayout::eDepthReadOnlyOptimal)
			.setImage(depth_image_storage.image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::detail::convert_hpp(format_to_aspect(depth_image_storage.format)))
				.setBaseMipLevel(0)
				.setLevelCount(std::max(depth_image_storage.level_count, 1u))
				.setBaseArrayLayer(0)
				.setLayerCount(1)));
	}

	const vk::DependencyInfo dependency_info = vk::DependencyInfo()
		.setImageMemoryBarriers(image_transitions);
	
	// std::cout << "Finishing rendering for command buffer " << command_rid << " with " << image_transitions.size() << " image transitions." << std::endl;
	command_buffer.pipelineBarrier2(dependency_info);
	
	command_storage.rendering = false;
	command_storage.color_attachments_image_views.clear();
	command_storage.depth_attachment_image_view = 0;
}

void VkGraphicsDriverBackend::finish(const RID command_rid) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkCheck(vkEndCommandBuffer(command), "Failed to end command buffer recording");
}

void VkGraphicsDriverBackend::submit(const RID command_rid) {
	const vulkan::CommandBufferStorage& command_storage = get_command_buffer_storage(command_rid);
	
	VkSemaphoreSubmitInfo wait_semaphore_submit_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.pNext = nullptr,
		.semaphore = VK_NULL_HANDLE,
		.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
	};
	VkSemaphoreSubmitInfo signal_semaphore_submit_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.pNext = nullptr,
		.semaphore = VK_NULL_HANDLE,
		.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
	};
	VkFence fence = VK_NULL_HANDLE;
	
	if (command_storage.connected_surface.has_value()) {
		const vulkan::SurfaceStorage &surface_storage = get_surface_storage(command_storage.connected_surface.value());
	
		const VkSemaphore image_available_semaphore = get_semaphore(surface_storage.image_available_semaphores[surface_storage.frame_index]);
		const VkSemaphore render_finished_semaphore = get_semaphore(surface_storage.render_finished_semaphores[surface_storage.image_index]);
		wait_semaphore_submit_info.semaphore = image_available_semaphore;
		signal_semaphore_submit_info.semaphore = render_finished_semaphore;
		fence = get_fence(surface_storage.graphics_fences[surface_storage.frame_index]);
	}
	
	VkCommandBufferSubmitInfo command_buffer_submit_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = command_storage.command_buffer
	};

	const VkSubmitInfo2 submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = 1,
		.pWaitSemaphoreInfos = &wait_semaphore_submit_info,
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &command_buffer_submit_info,
		.signalSemaphoreInfoCount = 1,
		.pSignalSemaphoreInfos = &signal_semaphore_submit_info
	};

	vkCheck(vkQueueSubmit2(graphics_queue_[current_graphics_queue], 1, &submit_info, fence),"Failed to submit command buffer");
}

void VkGraphicsDriverBackend::present(const RID surface_rid) {
	vulkan::SurfaceStorage& surface_storage = get_surface_storage(surface_rid);
	const VkSemaphore render_finished_semaphore = get_semaphore(surface_storage.render_finished_semaphores[surface_storage.image_index]);
	
	VkSwapchainKHR swapchain = surface_storage.swapchain;

	const VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &render_finished_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &surface_storage.image_index
	};
	
	VkResult result = vkQueuePresentKHR(graphics_queue_[current_graphics_queue], &present_info);
	
	switch (result) {
	case VK_ERROR_OUT_OF_DATE_KHR:
	case VK_SUBOPTIMAL_KHR:
		update_surface_configuration(surface_rid, SurfaceDescriptor{
			.label = "i have been recreated and i have forgotten my own name.",
			.size = surface_storage.window->get_size(),
			.minimum_images = frames_in_flight,
			.format = surface_storage.color_format,
			.usage = ImageUsage::eColorAttachment,
			.present_method = PresentMethod::eMailbox,
			.color_space = ColorSpace::eSrgbNonLinear
		});
		break;
	case VK_SUCCESS:
		break;
	default:
		vkCheck(result, "Failed to present swapchain image");
		break;
	}
	
	surface_storage.frame_index = (surface_storage.frame_index + 1) % frames_in_flight;
	current_graphics_queue = (current_graphics_queue + 1) % graphics_queue_.size();
	
	get_command_buffer_storage(surface_storage.graphics_command_buffers[surface_storage.frame_index]).connected_surface = std::nullopt;
}

void VkGraphicsDriverBackend::push_label(const RID command_rid, const String &label) {
	const VkDebugUtilsLabelEXT label_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pNext = nullptr,
		.pLabelName = label.c_str(),
		.color = {0.0f, 0.0f, 0.0f, 0.0f}
	};
	vkCmdBeginDebugUtilsLabel(get_command_buffer(command_rid), &label_info);
}

void VkGraphicsDriverBackend::pop_label(const RID command_rid) {
	vkCmdEndDebugUtilsLabel(get_command_buffer(command_rid));
}

void VkGraphicsDriverBackend::bind_shader(const RID command_rid, RID shader_rid, ShaderStage stage) {
	bind_shader(command_rid, { shader_rid }, { stage });
}

void VkGraphicsDriverBackend::bind_shader(const RID command_rid, const Vector<RID> shader_rids, const Vector<ShaderStage> stages) {
	const VkCommandBuffer command_buffer = get_command_buffer(command_rid);
	Vector<VkShaderEXT> shader_objects;
	Vector<VkShaderStageFlagBits> shader_flags;
	
	for (size_t i = 0; i < shader_rids.size(); ++i) {
		const RID shader_rid = shader_rids[i];
		const ShaderStage stage = stages[i];
		const VkShaderEXT shader_object = get_shader(shader_rid);
		shader_objects.push_back(shader_object);
		shader_flags.push_back(vk::detail::convert2(stage));
	}

	vkCmdBindShaders(command_buffer, static_cast<uint32_t>(shader_objects.size()), shader_flags.data(), shader_objects.data());
}

void VkGraphicsDriverBackend::bind_shader(const RID command_rid, const Vector<BindShaderDescriptor> shader_descriptors) {
	const VkCommandBuffer command_buffer = get_command_buffer(command_rid);
	Vector<VkShaderEXT> shader_objects;
	Vector<VkShaderStageFlagBits> shader_flags;
	
	for (const BindShaderDescriptor &descriptor : shader_descriptors) {
		const RID shader_rid = descriptor.shader;
		const ShaderStage stage = descriptor.stage;
		const VkShaderEXT shader_object = get_shader(shader_rid);
		shader_objects.push_back(shader_object);
		shader_flags.push_back(vk::detail::convert2(stage));
	}
	
	vkCmdBindShaders(command_buffer, static_cast<uint32_t>(shader_objects.size()), shader_flags.data(), shader_objects.data());
}

void VkGraphicsDriverBackend::transition(const RID command_rid, const ImageTransitionDescriptor &descriptor) {
	transition(command_rid, Vector{ descriptor });
}

void VkGraphicsDriverBackend::transition(const RID command_rid, const RID image, const ImageLayout layout, BitFlag<Access> access, BitFlag<PipelineStage> stage, ImageSubresourceDescriptor subresource) {
	vulkan::ImageStorage &image_storage = get_image_storage(image); // Ensure the image exists
	
	transition(command_rid, { 
	    ImageTransitionDescriptor{
	        .image = image,
	        .src = {
		    	.layout = image_storage.layout,
		    	.access = image_storage.access,
		    	.stage = image_storage.stage
	        },
	        .dst = {
		        .layout = layout,
		        .access = access,
		        .stage = stage
	        },
	        .subresource = subresource
	    }
	});
	
	image_storage.layout = layout;
	image_storage.access = access;
	image_storage.stage = stage;
}

void VkGraphicsDriverBackend::transition(const RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	Vector<VkImageMemoryBarrier2> barriers;
	for (const ImageTransitionDescriptor &descriptor : descriptors) {
		const VkImage image = get_image(descriptor.image);

		uint32_t src_queue_family = VK_QUEUE_FAMILY_IGNORED;
		if (descriptor.src.queue_family.has_value())
			src_queue_family = queue_family(descriptor.src.queue_family.value());

		uint32_t dst_queue_family = VK_QUEUE_FAMILY_IGNORED;
		if (descriptor.dst.queue_family.has_value())
			dst_queue_family = queue_family(descriptor.dst.queue_family.value());
		
		if (descriptor.src.layout == descriptor.dst.layout &&
			descriptor.src.access == descriptor.dst.access &&
			descriptor.src.stage == descriptor.dst.stage) {
			__debugbreak();
		}

		VkImageMemoryBarrier2 barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = vk::detail::convert(descriptor.src.stage),
			.srcAccessMask = vk::detail::convert(descriptor.src.access),
			.dstStageMask = vk::detail::convert(descriptor.dst.stage),
			.dstAccessMask = vk::detail::convert(descriptor.dst.access),
			.oldLayout = vk::detail::convert(descriptor.src.layout),
			.newLayout = vk::detail::convert(descriptor.dst.layout),
			.srcQueueFamilyIndex = src_queue_family,
			.dstQueueFamilyIndex = dst_queue_family,
			.image = image,
			.subresourceRange = vk::detail::convert(descriptor.subresource.value_or(ImageSubresourceDescriptor{
				.aspect_mask = Aspect::eColor,
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}))
		};
		barriers.push_back(barrier);
		
		vulkan::ImageStorage &image_storage = get_image_storage(descriptor.image);
		//printf("Transitioned image \"%s\" from layout \"%s\" to layout \"%s\"\n", image_storage.name.c_str(), gfx::to_string(descriptor.src.layout), gfx::to_string(descriptor.dst.layout));
		
		image_storage.layout = descriptor.dst.layout;
		image_storage.access = descriptor.dst.access;
		image_storage.stage = descriptor.dst.stage;
	}
	const VkDependencyInfo dependency_info{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
		.pImageMemoryBarriers = barriers.data(),
	};
	vkCmdPipelineBarrier2(command, &dependency_info);
}

void VkGraphicsDriverBackend::bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) {
	bind_vertex_buffers(command_rid, { desc });
}

void VkGraphicsDriverBackend::bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	
	Vector<VkBuffer> buffers;     //(desc.size());
	Vector<VkDeviceSize> offsets; //(desc.size());
	for (const VertexBufferDescriptor &buffer_desc : desc) {
		buffers.push_back(get_buffer(buffer_desc.buffer));
		offsets.push_back(buffer_desc.offset);
	}
	vkCmdBindVertexBuffers(command, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VkGraphicsDriverBackend::bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkCmdBindIndexBuffer(command, get_buffer(desc.buffer), desc.offset, vk::detail::convert(desc.index_type));
}

void VkGraphicsDriverBackend::DrawIndexed(const RID command_rid, const std::uint32_t first_index, const std::uint32_t index_count) {
	draw_indexed(command_rid, index_count, 1, first_index, 0, 0);
}

void VkGraphicsDriverBackend::draw_indexed(const RID command_rid, const std::uint32_t index_count, const std::uint32_t instance_count, const std::uint32_t first_index, const std::int32_t vertex_offset, const std::uint32_t first_instance) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void VkGraphicsDriverBackend::draw_indexed_indirect(const RID command_rid, const RID buffer, const u64 buffer_offset, const RID count_buffer, const u64 count_buffer_offset, const u32 max_draw_count, const u32 stride) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	const VkBuffer vk_buffer = get_buffer(buffer);
	if (count_buffer.valid()) {
		const VkBuffer vk_count_buffer = get_buffer(count_buffer);
		vkCmdDrawIndexedIndirectCount(commandBuffer, vk_buffer, buffer_offset, vk_count_buffer, count_buffer_offset, max_draw_count, stride);
	} else {
		vkCmdDrawIndexedIndirect(commandBuffer, vk_buffer, buffer_offset, max_draw_count, stride);
	}
}

void VkGraphicsDriverBackend::set_depth_test_enable(const RID command_rid, const bool enable) {
	vkCmdSetDepthTestEnable(get_command_buffer(command_rid), enable ? VK_TRUE : VK_FALSE);
}

void VkGraphicsDriverBackend::set_depth_write_enable(const RID command_rid, const bool enable) {
	vkCmdSetDepthWriteEnable(get_command_buffer(command_rid), enable ? VK_TRUE : VK_FALSE);
}

void VkGraphicsDriverBackend::dispatch(const RID command_rid, const uvec3 groups) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDispatch(commandBuffer, groups.x, groups.y, groups.z);
}

void VkGraphicsDriverBackend::dispatch(const RID command_rid, const u32 groups_x, const u32 groups_y, const u32 groups_z) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDispatch(commandBuffer, groups_x, groups_y, groups_z);
}

void VkGraphicsDriverBackend::push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) {
	vkCmdPushConstants(get_command_buffer(command_rid), get_pipeline_layout(pipeline_layout_rid), 
	                   vk::detail::convert(BitFlag(descriptor.visibility)), descriptor.offset, descriptor.size, data);
}

void VkGraphicsDriverBackend::dispatch_mesh(const RID command_rid, const uvec3 groups) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawMeshTasks(commandBuffer, groups.x, groups.y, groups.z);
}

void VkGraphicsDriverBackend::dispatch_mesh(const RID command_rid, const u32 groups_x, const u32 groups_y, const u32 groups_z) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawMeshTasks(commandBuffer, groups_x, groups_y, groups_z);
}

vk::CommandBuffer VkGraphicsDriverBackend::get_command_buffer(const RID id) const {
	const vulkan::CommandBufferStorage* storage = command_buffers_.get(id);
	return storage->command_buffer;
}

vulkan::CommandBufferStorage & VkGraphicsDriverBackend::get_command_buffer_storage(const RID id) {
	vulkan::CommandBufferStorage* storage = command_buffers_.get(id);
	return *storage;
}

const vulkan::CommandBufferStorage & VkGraphicsDriverBackend::get_command_buffer_storage(const RID id) const {
	const vulkan::CommandBufferStorage* storage = command_buffers_.get(id);
	return *storage;
}

uint32_t VkGraphicsDriverBackend::queue_family(const QueueFamilyType queue_family) const {
	switch (queue_family) {
	case QueueFamilyType::eGraphics:
		return graphics_queue_family_index_;
	case QueueFamilyType::eCompute:
		return compute_queue_family_index_;
	case QueueFamilyType::eTransfer:
		return transfer_queue_family_index_;
	default:
		throw std::runtime_error("Invalid queue family type");
	}
}

void VkGraphicsDriverBackend::prune_dead_objects() {
}

void VkGraphicsDriverBackend::wait_for_idle() {
	vkDeviceWaitIdle(device_);
}

RID VkGraphicsDriverBackend::get_default_normal_image() {
	return default_normal_image;
}

RID VkGraphicsDriverBackend::get_default_normal_image_view() const {
	return default_normal_image_view;
}

RID VkGraphicsDriverBackend::get_default_orm_image() {
	return default_orm_image;
}

RID VkGraphicsDriverBackend::get_default_orm_image_view() const {
	return default_orm_image_view;
}

#ifdef _DEBUG

void VkGraphicsDriverBackend::imgui_draw_buffer_resource_info(const RID buffer) {
	vulkan::BufferStorage* buffer_storage = buffers_.get(buffer);
	
	VmaAllocationInfo2 allocation_info2;
	vmaGetAllocationInfo2(allocator_, buffer_storage->allocation, &allocation_info2);
	
	ImGui::SeparatorText("Buffer Resource Info (vulkan)");
	ImGui::Text("Buffer RID: %u:%u", buffer.upper, buffer.lower);
	ImGui::Text("Buffer Size: %llu bytes", allocation_info2.blockSize);
	ImGui::Text("Dedicated: %s", allocation_info2.dedicatedMemory ? "Yes" : "No");
	ImGui::Text("Memory Type Index: %u", allocation_info2.allocationInfo.memoryType);
}

#endif

namespace std {
	template <>
	struct hash<vk::CommandPool> {
		static size_t _Do_hash(const vk::CommandPool& _Keyval) noexcept {
			return (size_t)(VkCommandPool)_Keyval;
		}
		_NODISCARD _STATIC_CALL_OPERATOR size_t operator()(const vk::CommandPool _Keyval) _CONST_CALL_OPERATOR noexcept {
			return _Do_hash(_Keyval);
		}
	};
}

void VkGraphicsDriverBackend::prune() {
	std::scoped_lock lock(transfer_mutex_);
	std::vector<vk::Fence> dead_fences;
	std::unordered_map<vk::CommandPool, std::vector<vk::CommandBuffer>> dead_command_buffers;
	for (auto &transfer : image_transfers_) {
		switch (const vk::Result result = device_.waitForFences(transfer.fence, vk::True, std::numeric_limits<u64>::max())) {
		case vk::Result::eTimeout:
			continue;
		case vk::Result::eSuccess:
			// Thread local command pools... look into how well this would work with transient command buffers.
			if (!dead_command_buffers.contains(transfer.owning_pool))
				dead_command_buffers[transfer.owning_pool] = {};
			dead_command_buffers[transfer.owning_pool].push_back(transfer.command_buffer);
			device_.destroyFence(transfer.fence);
			break;
		default:
			break;
		}
	}
	for (auto &dead_commands : dead_command_buffers)
		device_.freeCommandBuffers(dead_commands.first, dead_commands.second);
}

void VkGraphicsDriverBackend::create_instance() {
	uint32_t extension_count = 0;
	const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

	Vector<const char*> extensions(extension_count);
	for (uint32_t u = 0u; u < extension_count; u++)
		extensions[u] = required_instance_extensions[u];
	
#ifdef _DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	Vector enables = {
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
		VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
		VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
	};
	
	VkValidationFeaturesEXT validationFeaturesExt {
		.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
		.pNext = nullptr,
		.enabledValidationFeatureCount = static_cast<uint32_t>(enables.size()),
		.pEnabledValidationFeatures = enables.data(),
		.disabledValidationFeatureCount = 0,
		.pDisabledValidationFeatures = nullptr
	};
	
#endif
	
	Vector<const char*> layers = {
#ifdef _DEBUG
		"VK_LAYER_KHRONOS_validation"
#endif
	};
	
	
	constexpr auto applicationInfo = vk::ApplicationInfo()
	    .setApiVersion(VK_API_VERSION_1_4)
	    .setApplicationVersion(1)
	    .setPApplicationName("Generic Application")
	    .setEngineVersion(1)
	    .setPEngineName("Helix");

	const auto instanceCreateInfo = vk::InstanceCreateInfo()
	    .setPApplicationInfo(&applicationInfo)
	    .setPEnabledExtensionNames(extensions)
	    .setPEnabledLayerNames(layers);
	
	std::string extension_list;
	
	for (std::uint32_t i = 0; i < extension_count; ++i) {
		extension_list += extensions[i];
		if (i < extension_count - 1) {
			extension_list += ", ";
		}
	}
	
	helix_print("Vulkan: The instance extensions in use are {} ({})", extension_list.c_str(), extension_count);
	
	instance_ = vk::createInstance(instanceCreateInfo);

	load_instance_extension_functions();
	
	ext.vkCmdDrawMeshTasks = (PFN_vkCmdDrawMeshTasksEXT)vkGetInstanceProcAddr(instance_, "vkCmdDrawMeshTasksEXT");
	ext.vkCmdDrawMeshTasksIndirect = (PFN_vkCmdDrawMeshTasksIndirectEXT)vkGetInstanceProcAddr(instance_, "vkCmdDrawMeshTasksIndirectEXT");
	
#ifdef _DEBUG
	const VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = vulkan::vkDebugMessengerCallback,
		.pUserData = this
	};
	vkCheck(vkCreateDebugUtilsMessenger(
		        instance_, &debug_utils_messenger_create_info,
		        nullptr, &debug_messenger_
	        ), "Failed to create debug utils messenger");
	
#endif
}

void VkGraphicsDriverBackend::request_adapter() {
	const Vector<vk::PhysicalDevice> physical_devices = instance_.enumeratePhysicalDevices();
	
	vk::PhysicalDevice chosen_adapter;
	f64 chosen_adapter_score = 0.0f;
	
	for (vk::PhysicalDevice physical_device : physical_devices) {
		f64 adapter_score = 0.0f;
		const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
		
		adapter_score += static_cast<f64>(properties.limits.maxImageDimension2D);
		
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			adapter_score *= 100000.0f;
		}
		
		if (adapter_score > chosen_adapter_score) {
			chosen_adapter = physical_device;
			chosen_adapter_score = adapter_score;
		}
	}
	
	adapter_ = chosen_adapter;
}

void VkGraphicsDriverBackend::create_device_and_queues() {
	const std::vector<vk::QueueFamilyProperties> queue_families = adapter_.getQueueFamilyProperties();
	
	bool found_graphics_queue = false;
	bool found_compute_queue = false;
	bool found_transfer_queue = false;
	
	for (uint32_t queue_index = 0; queue_index < queue_families.size(); queue_index++) {
		vk::QueueFamilyProperties properties = queue_families[queue_index];
		
		if (!found_graphics_queue && properties.queueFlags & vk::QueueFlagBits::eGraphics) {
			found_graphics_queue = true;
			graphics_queue_family_index_ = queue_index;
			graphics_queue_.resize(properties.queueCount);
			continue;
		}
		
		if (!found_compute_queue && properties.queueFlags & vk::QueueFlagBits::eCompute) {
			found_compute_queue = true;
			compute_queue_family_index_ = queue_index;
			compute_queue_.resize(properties.queueCount);
			continue;
		}
		
		auto dedicated_transfer_queue_flags = vk::QueueFlagBits::eTransfer & ~vk::QueueFlagBits::eGraphics & ~vk::QueueFlagBits::eCompute;
		if (!found_transfer_queue && properties.queueFlags & dedicated_transfer_queue_flags) {
			found_transfer_queue = true;
			transfer_queue_family_index_ = queue_index;
			transfer_queue_.resize(properties.queueCount);
			continue;
		}
		
		if (found_graphics_queue && found_compute_queue && found_transfer_queue)
			break;
	}

	constexpr float queue_priority = 0.5000f;
	
	auto device_features_11 = vk::PhysicalDeviceVulkan11Features()
		.setShaderDrawParameters(true);
	
	auto device_features_12 = vk::PhysicalDeviceVulkan12Features()
		.setPNext(&device_features_11)
	    .setBufferDeviceAddress(true)
	    .setDescriptorBindingSampledImageUpdateAfterBind(true)
		.setDescriptorBindingStorageImageUpdateAfterBind(true)
	    .setDescriptorBindingUpdateUnusedWhilePending(true)
	    .setDescriptorBindingVariableDescriptorCount(true)
	    .setDescriptorIndexing(true)
		.setDrawIndirectCount(true)
	    .setRuntimeDescriptorArray(true)
	    .setScalarBlockLayout(true)
	    .setShaderInt8(true)
	    .setShaderSampledImageArrayNonUniformIndexing(true);

	auto device_features_13 = vk::PhysicalDeviceVulkan13Features()
	    .setPNext(&device_features_12)
	    .setDynamicRendering(true)
	    .setMaintenance4(true)
	.setShaderDemoteToHelperInvocation(true)
	    .setSynchronization2(true);

	auto device_features_14 = vk::PhysicalDeviceVulkan14Features()
	    .setPNext(&device_features_13)
	    .setPipelineRobustness(true)
	    .setPushDescriptor(true)
	    .setMaintenance5(true)
	    .setMaintenance6(true);
	
	auto mesh_shader_features = vk::PhysicalDeviceMeshShaderFeaturesEXT()
	    .setPNext(&device_features_14)
	    .setMeshShaderQueries(true)
	    .setMeshShader(true)
	    .setTaskShader(true);
	
	auto robustness_2 = vk::PhysicalDeviceRobustness2FeaturesEXT()
		.setPNext(&mesh_shader_features)
		.setNullDescriptor(true);
	
	auto atomic_float_1_features = vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT()
		.setPNext(&robustness_2)
		.setShaderBufferFloat32Atomics(true)
		.setShaderBufferFloat32AtomicAdd(true)
		.setShaderSharedFloat32Atomics(true)
		.setShaderSharedFloat32AtomicAdd(true)
		.setShaderBufferFloat64Atomics(true)
		.setShaderBufferFloat64AtomicAdd(true)
		.setShaderSharedFloat64Atomics(true)
		.setShaderSharedFloat64AtomicAdd(true);
	
	auto device_features = vk::PhysicalDeviceFeatures()
		.setSamplerAnisotropy(true)
		.setFragmentStoresAndAtomics(true);
	
	
	Vector<f32> graphics_priorities(graphics_queue_.size(), queue_priority);
	Vector<f32> compute_priorities(compute_queue_.size(), queue_priority);
	Vector<f32> transfer_priorities(transfer_queue_.size(), queue_priority);
	
	Vector queue_create_info = {
		vk::DeviceQueueCreateInfo()
			.setQueueFamilyIndex(graphics_queue_family_index_)
			.setQueueCount(static_cast<uint32_t>(graphics_queue_.capacity()))
			.setQueuePriorities(graphics_priorities),
		vk::DeviceQueueCreateInfo()
	    	.setQueueFamilyIndex(compute_queue_family_index_)
	    	.setQueueCount(static_cast<uint32_t>(compute_queue_.capacity()))
	    	.setQueuePriorities(compute_priorities),
		vk::DeviceQueueCreateInfo()
	   		.setQueueFamilyIndex(transfer_queue_family_index_)
	   		.setQueueCount(static_cast<uint32_t>(transfer_queue_.capacity()))
	   		.setQueuePriorities(transfer_priorities)
	};
	
	// bool ngq = queue_create_info[0].queueCount == graphics_queue_.size();
	// bool ncq = queue_create_info[1].queueCount == compute_queue_.size();
	// bool ntq = queue_create_info[2].queueCount == transfer_queue_.size();
	queue_create_info[0].setQueueCount(static_cast<uint32_t>(graphics_queue_.size()));
	queue_create_info[1].setQueueCount(static_cast<uint32_t>(compute_queue_.size()));
	queue_create_info[2].setQueueCount(static_cast<uint32_t>(transfer_queue_.size()));

	std::vector device_extensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
		VK_EXT_MESH_SHADER_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
		VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
	};

	const vk::DeviceCreateInfo &device_create_info = vk::DeviceCreateInfo()
	    .setPNext(&atomic_float_1_features)
	    .setPEnabledExtensionNames(device_extensions)
	    .setQueueCreateInfos(queue_create_info)
	    .setPEnabledFeatures(&device_features);
	
	device_ = adapter_.createDevice(device_create_info);
	
	load_device_extension_functions();
	
	for (std::uint32_t index = 0; index < graphics_queue_.size(); ++index) {
		graphics_queue_[index] = device_.getQueue(graphics_queue_family_index_, index);
#ifdef _DEBUG
		apply_label(this, "Graphics Queue #" + std::to_string(index), graphics_queue_[index]);
#endif
	}
	for (std::uint32_t index = 0; index < compute_queue_.size(); ++index) {
		compute_queue_[index] = device_.getQueue(compute_queue_family_index_, index);
#ifdef _DEBUG
		apply_label(this, "Compute Queue #" + std::to_string(index), compute_queue_[index]);
#endif
	}
	for (std::uint32_t index = 0; index < transfer_queue_.size(); ++index) {
		transfer_queue_[index] = device_.getQueue(transfer_queue_family_index_, index);
#ifdef _DEBUG
		apply_label(this, "Transfer Queue #" + std::to_string(index), transfer_queue_[index]);
#endif
	}
}

void VkGraphicsDriverBackend::create_allocator() {
	const vma::VulkanFunctions vulkanFunctions = vma::VulkanFunctions()
		.setVkGetInstanceProcAddr(vkGetInstanceProcAddr)
		.setVkGetDeviceProcAddr(vkGetDeviceProcAddr)
		.setVkGetPhysicalDeviceProperties(vkGetPhysicalDeviceProperties)
		.setVkGetPhysicalDeviceMemoryProperties(vkGetPhysicalDeviceMemoryProperties)
		.setVkAllocateMemory(vkAllocateMemory)
		.setVkFreeMemory(vkFreeMemory)
		.setVkMapMemory(vkMapMemory)
		.setVkUnmapMemory(vkUnmapMemory)
		.setVkFlushMappedMemoryRanges(vkFlushMappedMemoryRanges)
		.setVkInvalidateMappedMemoryRanges(vkInvalidateMappedMemoryRanges)
		.setVkBindBufferMemory(vkBindBufferMemory)
		.setVkBindImageMemory(vkBindImageMemory)
		.setVkGetBufferMemoryRequirements(vkGetBufferMemoryRequirements)
		.setVkGetImageMemoryRequirements(vkGetImageMemoryRequirements)
		.setVkCreateBuffer(vkCreateBuffer)
		.setVkDestroyBuffer(vkDestroyBuffer)
		.setVkCreateImage(vkCreateImage)
		.setVkDestroyImage(vkDestroyImage)
		.setVkCmdCopyBuffer(vkCmdCopyBuffer)
		.setVkGetDeviceBufferMemoryRequirements(vkGetDeviceBufferMemoryRequirements)
		.setVkGetDeviceImageMemoryRequirements(vkGetDeviceImageMemoryRequirements)
		.setVkGetMemoryWin32HandleKHR(nullptr);

	const vma::AllocatorCreateInfo allocator_create_info = vma::AllocatorCreateInfo()
		.setFlags(vma::AllocatorCreateFlagBits::eBufferDeviceAddress)
		.setPhysicalDevice(adapter_)
		.setDevice(device_)
		.setPVulkanFunctions(&vulkanFunctions)
		.setInstance(instance_);
	
	allocator_ = vma::createAllocator(allocator_create_info);
	//vkCheck(result, "Failed to create Vulkan Memory Allocator");
}

void VkGraphicsDriverBackend::create_default_pools() {

	vk::DescriptorPoolSize pool_sizes[] = {
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eSampler)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eSampledImage)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eStorageImage)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformTexelBuffer)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eStorageTexelBuffer)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eStorageBuffer)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBufferDynamic)
			.setDescriptorCount(1000),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eStorageBufferDynamic)
			.setDescriptorCount(1000),
	};
	
	const vk::DescriptorPoolCreateInfo descriptor_pool_create_info = vk::DescriptorPoolCreateInfo()
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
		.setPoolSizes(pool_sizes)
		.setMaxSets(1000);
	
	descriptor_pool_ = device_.createDescriptorPool(descriptor_pool_create_info);

	using enum vk::CommandPoolCreateFlagBits;
	const vk::CommandPoolCreateInfo transfer_command_pool_create_info = vk::CommandPoolCreateInfo()
	    .setFlags(eTransient)
	    .setQueueFamilyIndex(transfer_queue_family_index_);
	
	transfer_command_pool_ = device_.createCommandPool(transfer_command_pool_create_info);

	const VkCommandPoolCreateInfo graphics_command_pool_create_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = graphics_queue_family_index_
	};
	
	vkCheck(vkCreateCommandPool(device_, &graphics_command_pool_create_info, nullptr, (VkCommandPool*)&command_pool_), "Failed to create graphics command pool");
}

void VkGraphicsDriverBackend::create_default_images() {
	ImageDescriptor normal_descriptor{
		.label = "Default Normal",
		.format = gfx::Format::eRgba8Unorm,
		.usage = ImageUsage::eSampled | ImageUsage::eTransferDst,
		.samples = SampleCount::e1,
		.size = uint3(1, 1, 1),
		.array_layers = 1,
		.mip_levels = 1
	};
	default_normal_image = create_image(normal_descriptor);
	
	ImageDescriptor orm_descriptor{
		.label = "Default ORM",
		.format = gfx::Format::eRgba8Unorm,
		.usage = ImageUsage::eSampled | ImageUsage::eTransferDst,
		.samples = SampleCount::e1,
		.size = uint3(1, 1, 1),
		.array_layers = 1,
		.mip_levels = 1
	};
	default_orm_image = create_image(orm_descriptor);
	
	BufferDescriptor staging_descriptor{
		.label = "Staging Buffer",
		.size = 8,
		.usage = BitFlag(BufferUsage::eTransferSrc),
		.memory_usage = BitFlag(MemoryUsage::eCpuToGpu),
		.allocation_hints = BitFlag(AllocationHint::eMapped) | AllocationHint::eHostSequentialWrite | AllocationHint::eAllowTransferInstead
	};
	RID staging = create_buffer(staging_descriptor);
	
	u8* data = (u8*)get_mapped_data(staging);
	data[0] = 0x80;
	data[1] = 0x80;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0x00;
	data[5] = 0xA0;
	data[6] = 0x00;
	data[7] = 0xFF;
	
	VkBufferImageCopy2 copy{
		.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
		.bufferOffset = 0,
		.bufferRowLength = 1,
		.bufferImageHeight = 1,
		.imageSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageOffset = VkOffset3D{ 0, 0, 0 },
		.imageExtent = VkExtent3D{ 1, 1, 1 }
	};
	
	RID images[] = { default_normal_image, default_orm_image };
	
	for (RID image : images) {
		VkFence fence = load_image_from_buffer(image, staging, copy);
		vkCheck(vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fences on default image creation");
		vkDestroyFence(device_, fence, nullptr);
		
		for (auto& transfer : image_transfers_)
			if (transfer.fence == fence) {
				transfer.fence = VK_NULL_HANDLE;
				break;
			}

		copy.bufferOffset += 4;
		flush_buffer(staging, {0, 4});
	}
	
	ImageViewDescriptor normal_view_descriptor{
		.label = "Default Normal View",
		.image = default_normal_image,
		.type = ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = Aspect::eColor,
			.base_mip_level = 0,
			.level_count = 1,
			.base_array_layer = 0,
			.layer_count = 1
		}
	};
	default_normal_image_view = create_image_view(normal_view_descriptor);
	ImageViewDescriptor orm_view_descriptor{
		.label = "Default ORM View",
		.image = default_orm_image,
		.type = ImageViewType::e2D,
		.format = gfx::Format::eRgba8Unorm,
		.subresource = ImageSubresourceDescriptor{
			.aspect_mask = Aspect::eColor,
			.base_mip_level = 0,
			.level_count = 1,
			.base_array_layer = 0,
			.layer_count = 1
		}
	};
	default_orm_image_view = create_image_view(orm_view_descriptor);
	destroy_buffer(staging);
}

void VkGraphicsDriverBackend::create_default_sampler() {
	const SamplerDescriptor sampler_descriptor{
		.label = "Sampler",
		.mag_filter = Filter::eLinear,
		.min_filter = Filter::eLinear,
		.mipmap_mode = MipmapFilter::eLinear,
		.address_mode_u = AddressMode::eRepeat,
		.address_mode_v = AddressMode::eRepeat,
		.address_mode_w = AddressMode::eRepeat,
		.compare_op = CompareOp::eAlways,
		.enable_anisotropy = true,
		.mip_lod_bias = 0.0f,
		.max_anisotropy = 16.0f,
		.min_lod = 0.0f,
		.max_lod = 1.0f
	};
	default_sampler_ = create_sampler(sampler_descriptor);
}

void VkGraphicsDriverBackend::dispose() {
	shutdown();
}

bool VkGraphicsDriverBackend::disposed() const {
	return instance_ == VK_NULL_HANDLE && device_ == VK_NULL_HANDLE && allocator_ == VK_NULL_HANDLE;
}

void VkGraphicsDriverBackend::load_instance_extension_functions() {
	ext.vkCreateDebugUtilsMessengerEXT 	= (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
	ext.vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
	ext.vkCmdBeginDebugUtilsLabelEXT 	= (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdBeginDebugUtilsLabelEXT");
	ext.vkCmdInsertDebugUtilsLabelEXT 	= (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdInsertDebugUtilsLabelEXT");
	ext.vkCmdEndDebugUtilsLabelEXT 		= (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdEndDebugUtilsLabelEXT");
	ext.vkQueueBeginDebugUtilsLabelEXT 	= (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueBeginDebugUtilsLabelEXT");
	ext.vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueInsertDebugUtilsLabelEXT");
	ext.vkQueueEndDebugUtilsLabelEXT 	= (PFN_vkQueueEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueEndDebugUtilsLabelEXT");
	ext.vkSetDebugUtilsObjectNameEXT 	= (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance_, "vkSetDebugUtilsObjectNameEXT");
}

void VkGraphicsDriverBackend::load_device_extension_functions() {
	ext.vkCreateShaders		= (PFN_vkCreateShadersEXT)vkGetDeviceProcAddr(device_, "vkCreateShadersEXT");
	ext.vkCmdBindShaders	= (PFN_vkCmdBindShadersEXT)vkGetDeviceProcAddr(device_, "vkCmdBindShadersEXT");
}

// ReSharper disable CppMemberFunctionMayBeConst
VkResult VkGraphicsDriverBackend::vkCreateDebugUtilsMessenger(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT*pMessenger) {
	return ext.vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

void VkGraphicsDriverBackend::vkDestroyDebugUtilsMessenger(const VkInstance instance, const VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator) {
	ext.vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

void VkGraphicsDriverBackend::vkCmdBeginDebugUtilsLabel(const VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

void VkGraphicsDriverBackend::vkCmdInsertDebugUtilsLabel(const VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

void VkGraphicsDriverBackend::vkCmdEndDebugUtilsLabel(const VkCommandBuffer commandBuffer) {
	ext.vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}

void VkGraphicsDriverBackend::vkQueueBeginDebugUtilsLabel(const VkQueue queue, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}

void VkGraphicsDriverBackend::vkQueueInsertDebugUtilsLabel(const VkQueue queue, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}

void VkGraphicsDriverBackend::vkQueueEndDebugUtilsLabel(const VkQueue queue) {
	ext.vkQueueEndDebugUtilsLabelEXT(queue);
}

VkResult VkGraphicsDriverBackend::vkSetDebugUtilsObjectName(const VkDevice device, const VkDebugUtilsObjectNameInfoEXT *pNameInfo) {
	return ext.vkSetDebugUtilsObjectNameEXT(device, pNameInfo);
}

void VkGraphicsDriverBackend::vkCmdDrawMeshTasks(const VkCommandBuffer commandBuffer, const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ) {
	ext.vkCmdDrawMeshTasks(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VkGraphicsDriverBackend::vkCmdDrawMeshTasksIndirect(const VkCommandBuffer commandBuffer, const VkBuffer buffer, const VkDeviceSize offset,
                                                   const uint32_t drawCount, const uint32_t stride) {
	ext.vkCmdDrawMeshTasksIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

VkResult VkGraphicsDriverBackend::vkCreateShaders(const VkDevice device, const uint32_t createInfoCount,
                                            const VkShaderCreateInfoEXT *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkShaderEXT*pShaders) {
	return ext.vkCreateShaders(device, createInfoCount, pCreateInfos, pAllocator, pShaders);
}

void VkGraphicsDriverBackend::vkDestroyShader(const VkDevice device, const VkShaderEXT shader, const VkAllocationCallbacks *pAllocator) {
	ext.vkDestroyShader(device, shader, pAllocator);
}

void VkGraphicsDriverBackend::vkCmdBindShaders(const VkCommandBuffer commandBuffer, const uint32_t stageCount, const VkShaderStageFlagBits *pStages, const VkShaderEXT *pShaders) {
	ext.vkCmdBindShaders(commandBuffer, stageCount, pStages, pShaders);
}
// ReSharper restore CppMemberFunctionMayBeConst