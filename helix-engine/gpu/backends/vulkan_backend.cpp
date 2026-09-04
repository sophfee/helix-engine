// ReSharper disable CppClangTidyClangDiagnosticMissingDesignatedFieldInitializers
// ReSharper disable CppClangTidyClangDiagnosticMissingFieldInitializers
// ReSharper disable CppClangTidyMiscUseAnonymousNamespace
// ReSharper disable CppTooWideScopeInitStatement
// ReSharper disable CppVariableCanBeMadeConstexpr
#include "vulkan_backend.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"
#include "detail/vulkan_enum_conversion.hpp"
#include "ecs/transform.h"
#include "glfw/glfw3.h"
#include "gpu/window.hpp"

using detail::has_flag;
using namespace gfx;

#ifdef _DEBUG

VkBool32 vulkan::vkDebugMessengerCallback(
	const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	const VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData
) {
	std::string message;
	
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		message += "[VERBOSE] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		message += "[INFO] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		message += "[WARNING] ";
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		message += "[ERROR] ";
	
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
	return false;
}

#endif

void vkResultCheckInner(VkResult result, const char *file, const int line, const char *msg) {
	char message[4096] = {};
	
	int err = sprintf_s(message, 4096, "[%s:%d] Vulkan error: %s", file, line, msg);
	
	vk::detail::resultCheck(static_cast<vk::Result>(result), message);
}

VkGraphicsBackend::VkGraphicsBackend() : debug_messenger_(nullptr), allocator_(nullptr), graphics_queue_family_index_(0),
										 compute_queue_family_index_(0), transfer_queue_family_index_(0) {
	//initialize();
}

VkGraphicsBackend::~VkGraphicsBackend() {
	shutdown();
}

bool VkGraphicsBackend::initialize() {
	
	if (instance_ != VK_NULL_HANDLE) return true;
	
	create_instance();
	request_adapter();
	create_device_and_queues();
	create_allocator();
	create_default_pools();

	return true;
}

void VkGraphicsBackend::shutdown() {
	for (const std::pair surface : surfaces_)
		if (surface.second != VK_NULL_HANDLE)
			destroy_surface(surface.first);

	buffers_.clear();
	image_views_.clear();
	images_.clear();
	samplers_.clear();
	
	for (const std::pair fence : fences_) {
		if (*fence.second != VK_NULL_HANDLE) {
			VkFence fence_c = *fence.second;
			const VkResult result = vkWaitForFences(device_, 1, &fence_c, VK_TRUE, 10000);
			if (result == VK_TIMEOUT)
				printf("Warning: Fence wait timed out during shutdown. This may indicate a problem with the application.\n");
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
		device_.freeDescriptorSets(descriptor_pool_, { *slot.second });
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

void VkGraphicsBackend::yield_for_commands() {
	//for (auto &q : graphics_queue_)
	//	q.waitIdle();
	//for (auto &q : compute_queue_)
	//	q.waitIdle();
	//for (auto &q : transfer_queue_)
	//	q.waitIdle();
}

void VkGraphicsBackend::initialize_im_gui() {
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
		VK_FORMAT_R8G8B8A8_UNORM
	};
	VkFormat depth_attachment_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	const VkPipelineRenderingCreateInfo rendering_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
		.pColorAttachmentFormats = color_attachment_formats.data(),
		.depthAttachmentFormat = depth_attachment_format
	};
	
	ImGui_ImplVulkan_PipelineInfo imgui_impl_vulkan_pipeline_info{
		.PipelineRenderingCreateInfo = rendering_create_info
	};
	
	ImGui_ImplVulkan_CreateMainPipeline(&imgui_impl_vulkan_pipeline_info);
	
	
#endif
}

RID VkGraphicsBackend::create_buffer(const BufferDescriptor &desc) {
	const VkBufferCreateInfo buffer_create_info = VkBufferCreateInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = desc.size,
		.usage = vk::detail::convert(desc.usage),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};
	
	const VmaAllocationCreateInfo allocation_info = VmaAllocationCreateInfo{
		.flags = vk::detail::convert(desc.allocation_hints.value_or(AllocationHint::eDedicated)),
		.usage = vk::detail::convert(desc.memory_usage.value_or(MemoryUsage::eAuto)),
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = nullptr,
		.pUserData = nullptr,
		.priority = 0.0f
	};
	
	VkBuffer buffer;
	VmaAllocation allocation;
	
	{
		std::scoped_lock lock(allocation_mutex_);
		vmaCreateBuffer(allocator_, &buffer_create_info, &allocation_info, &buffer, &allocation, nullptr);
	}
#ifdef _DEBUG
	
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_BUFFER,
			.objectHandle = reinterpret_cast<uint64_t>(buffer),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}

#endif

	const auto handle = buffers_.emplace(vulkan::BufferStorage{
		.buffer = buffer,
		.allocation = allocation,
		.is_allocated = true
	});
	RID rid = _make_rid(ResourceKind::eBuffer, handle.slot);
	rid.lower = handle.generation;
	++allocations_;
	return rid;
}

void VkGraphicsBackend::destroy_buffer(const RID id) {
	if (id.lower <= 0) return;
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	if (storage && storage->is_allocated) {
		vmaDestroyBuffer(allocator_, storage->buffer, storage->allocation);
		--allocations_;
	}
}

void VkGraphicsBackend::set_buffer_name(const RID buffer_rid, const char *name) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaSetAllocationName(allocator_, allocation, name);
}

void * VkGraphicsBackend::map_buffer(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	void *mapped_data;
	const VkResult result = vmaMapMemory(allocator_, allocation, &mapped_data);
	vkCheck(result, "Failed to map buffer memory");
	return mapped_data;
}

void VkGraphicsBackend::unmap_buffer(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaUnmapMemory(allocator_, allocation);
}

void * VkGraphicsBackend::get_mapped_data(const RID id) {
	const VmaAllocationInfo allocation_info = get_buffer_allocation_info(id);
	return allocation_info.pMappedData;
}

GpuDeviceAddress VkGraphicsBackend::get_buffer_virtual_address(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	const VkBufferDeviceAddressInfo buffer_address_info = VkBufferDeviceAddressInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.pNext = nullptr,
		.buffer = storage->buffer
	};
	return vkGetBufferDeviceAddress(device_, &buffer_address_info);
}

void VkGraphicsBackend::flush_buffer(const RID buffer_rid, const ivec2 range) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaFlushAllocation(allocator_, allocation, range.x, range.y == -1 ? VK_WHOLE_SIZE : range.y);
}

RID VkGraphicsBackend::create_image() {
	const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
		.image = VK_NULL_HANDLE,
		.allocation = VK_NULL_HANDLE,
		.format = gfx::Format::eUndefined,
		.layout = ImageLayout::eUndefined,
		.access = Access::eNone,
		.stage = PipelineStage::eNone
	});
	RID rid = _make_rid(ResourceKind::eImage, handle.slot);
	rid.lower = handle.generation;
	++allocations_;
	return rid;
}

RID VkGraphicsBackend::create_image(const ImageDescriptor &desc) {
	const VkImageCreateInfo image_create_info = VkImageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = vk::detail::convert(desc.type),
		.format = vk::detail::convert(desc.format),
		.extent = { 
			.width = desc.size.x,
			.height = desc.size.y,
			.depth = desc.size.z
		},
		.mipLevels = std::max(desc.mip_levels, 1u),
		.arrayLayers = std::max(desc.array_layers, 1u),
		.samples = vk::detail::convert(desc.samples.value_or(SampleCount::e1)),
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = vk::detail::convert(desc.usage),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	};
	
	const VmaAllocationCreateInfo allocation_info = VmaAllocationCreateInfo{
		.flags = vk::detail::convert(desc.allocation_hints.value_or(AllocationHint::eDedicated)),
		.usage = vk::detail::convert(desc.memory_usage.value_or(MemoryUsage::eAuto)),
	};

	VkImage image;
	VmaAllocation allocation;
	{
		std::scoped_lock l(allocation_mutex_);
		vmaCreateImage(allocator_, &image_create_info, &allocation_info, &image, &allocation, nullptr);
	}
	
	{
		const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
			.image = image,
			.allocation = allocation,
			.format = desc.format,
			.level_count = std::max(desc.mip_levels, 1u),
		});
		RID rid = _make_rid(ResourceKind::eImage, handle.slot);
		rid.lower = handle.generation;
		++allocations_;
		
		if (desc.label.has_value())
			set_image_name(rid, desc.label.value().c_str());
		
		return rid;
	}
}

void VkGraphicsBackend::create_image(const RID image_rid, const ImageDescriptor &desc) {
	const VkImageCreateInfo image_create_info = VkImageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = vk::detail::convert(desc.type),
		.format = vk::detail::convert(desc.format),
		.extent = { 
			.width = desc.size.x,
			.height = desc.size.y,
			.depth = desc.size.z
		},
		.mipLevels = std::max(desc.mip_levels, 1u),
		.arrayLayers = std::max(desc.array_layers, 1u),
		.samples = vk::detail::convert(desc.samples.value_or(SampleCount::e1)),
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = vk::detail::convert(desc.usage),
		.initialLayout = vk::detail::convert(desc.initial_layout.value_or(ImageLayout::eUndefined))
	};
	
	const VmaAllocationCreateInfo allocation_info = VmaAllocationCreateInfo{
		.flags = vk::detail::convert(desc.allocation_hints.value_or(AllocationHint::eDedicated)),
		.usage = vk::detail::convert(desc.memory_usage.value_or(MemoryUsage::eAuto)),
	};

	vulkan::ImageStorage &storage = get_image_storage_mutable(image_rid);
	VkImage image;
	VmaAllocation allocation;
	{
		std::scoped_lock lock(allocation_mutex_);
		vkCheck(vmaCreateImage(allocator_, &image_create_info, &allocation_info, &image, &allocation, nullptr), "failed to create image");
		storage.image = image;
		storage.allocation = allocation;
		storage.format = desc.format;
	}
}

void VkGraphicsBackend::destroy_image(const RID id) {
	if (!is_valid_rid(id)) return;
	const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	vmaDestroyImage(allocator_, storage->image, storage->allocation);
	assert(images_.erase(id.upper, id.lower));
}

void VkGraphicsBackend::set_image_name(const RID handle, const char *name) {
	const vulkan::ImageStorage storage = get_image_storage(handle);
	vmaSetAllocationName(allocator_, storage.allocation, name);
}

bool VkGraphicsBackend::is_image_valid(const RID image_rid) {
	const vulkan::ImageStorage *storage = images_.get(image_rid.upper, image_rid.lower);
	return storage != nullptr && storage->image != VK_NULL_HANDLE;
}

VkFence VkGraphicsBackend::load_image_from_buffer(RID image_rid, RID buffer_rid, const Vector<VkBufferImageCopy2> &copy) {
	VkFence fence;
	VkCommandBuffer command;
	
	const vulkan::ImageStorage& storage = get_image_storage(image_rid);
	const VkImage image = get_image(image_rid);
	const VkBuffer buffer = get_buffer(buffer_rid);
	{
		const VkFenceCreateInfo fence_create_info{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		vkCheck(vkCreateFence(device_, &fence_create_info, nullptr, &fence), "Failed to create fence for transfer command buffer");
		
#ifdef _DEBUG
		std::string label_name = "Transfer Op: I[" + std::to_string(image_rid.upper) + ":" + std::to_string(image_rid.lower) + "] << B[" + std::to_string(buffer_rid.upper) + ":" + std::to_string(buffer_rid.lower) + "]";
		VkDebugUtilsObjectNameInfoEXT label{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_FENCE,
			.objectHandle = (u64)fence,
			.pObjectName = label_name.c_str()
		};
		vkSetDebugUtilsObjectName(device_, &label);
#endif
	}

	{
		std::scoped_lock lock(transfer_mutex_);
		if (transfer_command_pool_ == VK_NULL_HANDLE) {
			const VkCommandPoolCreateInfo command_pool_create_info{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = transfer_queue_family_index_
			};
			vkCheck(vkCreateCommandPool(device_, &command_pool_create_info, nullptr, &transfer_command_pool_), "Failed to create (thread local) transfer command pool");
			transfer_command_pools_.emplace_back(transfer_command_pool_);
		}
		const VkCommandBufferAllocateInfo command_buffer_allocate_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = transfer_command_pool_,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		vkCheck(vkAllocateCommandBuffers(device_, &command_buffer_allocate_info, &command), "Failed to allocate transfer command buffer");
	}
	
	{
		const VkCommandBufferBeginInfo begin_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			.pInheritanceInfo = nullptr
		};
		vkCheck(vkBeginCommandBuffer(command, &begin_info), "Failed to begin recording for transfer command buffer");
	}
	
	{
		const VkImageMemoryBarrier2 transfer_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange = { // TODO: Add mipmaps. Might need a descriptor for this function? Or maybe add target number of layers/levels into the ImageDescriptor.
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = storage.level_count,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		const VkDependencyInfo dependency_info{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &transfer_barrier
		};
		vkCmdPipelineBarrier2(command, &dependency_info);
	}
	const VkCopyBufferToImageInfo2 copy_info{
		.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
		.pNext = nullptr,
		.srcBuffer = buffer,
		.dstImage = image,
		.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		.regionCount = static_cast<uint32_t>(copy.size()),
		.pRegions = copy.data(),
	};
	vkCmdCopyBufferToImage2(command, &copy_info);
	{
		const VkImageMemoryBarrier2 transfer_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = transfer_queue_family_index_,
			.dstQueueFamilyIndex = graphics_queue_family_index_,
			.image = image,
			.subresourceRange = { // TODO: Add mipmaps. Might need a descriptor for this function? Or maybe add target number of layers/levels into the ImageDescriptor.
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = storage.level_count,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		const VkDependencyInfo dependency_info{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &transfer_barrier
		};
		vkCmdPipelineBarrier2(command, &dependency_info);
	}
	vkCheck(vkEndCommandBuffer(command), "Failed to end recording for transfer command buffer");
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

VkFence VkGraphicsBackend::load_image_from_buffer(const RID image_rid, const RID buffer_rid, VkBufferImageCopy2 &copy) {
	return load_image_from_buffer(image_rid, buffer_rid, Vector{ copy });
}

vk::Image VkGraphicsBackend::get_image(const RID id) const {
	try {
		const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
		return storage->image;
	}
	catch (const std::exception &e) {
		return VK_NULL_HANDLE;
	}
}

const vulkan::ImageStorage &VkGraphicsBackend::get_image_storage(const RID id) const {
	const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	return *storage;
}

vulkan::ImageStorage &VkGraphicsBackend::get_image_storage_mutable(const RID id) {
	vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	return *storage;
}

RID VkGraphicsBackend::create_image_view(const ImageViewDescriptor &desc) {
	const vulkan::ImageStorage &storage = get_image_storage(desc.image);
	const vk::Image image = storage.image;
	
	VkImageUsageFlags usageFlags = 0;
	switch (desc.subresource->aspect_mask) {
		case Aspect::eDepth:
			usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			break;
		default: {
			if (desc.usage.has_value()) {
				usageFlags = vk::detail::convert(desc.usage.value());
				break;
			}
		}
	}

	const VkImageViewUsageCreateInfo image_view_usage_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO,
		.usage = usageFlags
	};
	
	const VkImageViewCreateInfo image_view_create_info = VkImageViewCreateInfo{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = &image_view_usage_create_info,
		.image = image,
		.viewType = vk::detail::convert(desc.type.value_or(ImageViewType::e2D)),
		.format = vk::detail::convert(desc.format.value_or(get_image_storage(desc.image).format)),
		.components = VkComponentMapping{
			.r = vk::detail::convert(Swizzle::eIdentity),
			.g = vk::detail::convert(Swizzle::eIdentity),
			.b = vk::detail::convert(Swizzle::eIdentity),
			.a = vk::detail::convert(Swizzle::eIdentity)
		},
		.subresourceRange = vk::detail::convert(desc.subresource.value_or({}))
	};

	VkImageView image_view;
	vkCreateImageView(device_, &image_view_create_info, nullptr, &image_view);
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_IMAGE_VIEW,
			.objectHandle = reinterpret_cast<uint64_t>(image_view),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif

	const SlotPool<vulkan::ImageViewStorage>::Handle handle = image_views_.emplace(vulkan::ImageViewStorage{image_view, desc.image});
	return handle;
}

void VkGraphicsBackend::destroy_image_view(const RID id) {
	if (!is_valid_rid(id)) return;
	const vk::ImageView image_view = get_image_view(id);
	vkDestroyImageView(device_, image_view, nullptr);
	assert(image_views_.erase(id.upper, id.lower));
}

bool VkGraphicsBackend::is_image_view_valid(const RID image_view_rid) {
	if (!is_valid_rid(image_view_rid)) return false;
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(image_view_rid.upper, image_view_rid.lower);
	return image_view_storage != nullptr && image_view_storage->image_view != VK_NULL_HANDLE;
}

vk::ImageView VkGraphicsBackend::get_image_view(const RID id) const {
	if (!is_valid_rid(id)) return VK_NULL_HANDLE;
	const vulkan::ImageViewStorage* image_view_storage = image_views_.get(id.upper, id.lower);
	if (image_view_storage == nullptr) return VK_NULL_HANDLE;
	return image_view_storage->image_view;
}

RID VkGraphicsBackend::create_sampler(const SamplerDescriptor &desc) {
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

	const auto handle = samplers_.emplace(sampler);
	RID rid = _make_rid(ResourceKind::eSampler, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_sampler(const RID sampler) {
	if (sampler.lower==0) return;
	const vk::Sampler vk_sampler = get_sampler(sampler);
	vkDestroySampler(device_, vk_sampler, nullptr);
	assert(samplers_.erase(sampler.upper, sampler.lower));
}

vk::Sampler VkGraphicsBackend::get_sampler(const RID id) const {
	const vk::Sampler* sampler = samplers_.get(id.upper, id.lower);
	if (sampler == nullptr) return VK_NULL_HANDLE;
	return *sampler;
}

RID VkGraphicsBackend::create_bind_group_layout(const BindGroupLayoutDescriptor &desc) {
	Vector<VkDescriptorSetLayoutBinding> bindings(desc.entries.size());
	Vector<VkDescriptorBindingFlags> binding_flags(desc.entries.size());
	for (std::size_t i = 0; i < desc.entries.size(); ++i)
	{
		bindings[i] = vk::detail::convert(desc.entries[i]);
		binding_flags[i] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
	}
	const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_create_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.pNext = nullptr,
		.bindingCount = static_cast<uint32_t>(desc.entries.size()),
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
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
			.objectHandle = reinterpret_cast<uint64_t>(layout),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif

	const SlotPool<vk::DescriptorSetLayout>::Handle handle = descriptor_set_layouts_.emplace(layout);
	RID rid = _make_rid(ResourceKind::eBindGroupLayout, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_bind_group_layout(const RID id) {
	const vk::DescriptorSetLayout layout = get_bind_group_layout(id);
	vkDestroyDescriptorSetLayout(device_, layout, nullptr);
	assert(descriptor_set_layouts_.erase(id.upper, id.lower));
}

vk::DescriptorSetLayout VkGraphicsBackend::get_bind_group_layout(const RID id) const {
	const vk::DescriptorSetLayout* layout = descriptor_set_layouts_.get(id.upper, id.lower);
	return *layout;
}

RID VkGraphicsBackend::create_bind_group(const BindGroupDescriptor &desc) {
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
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET,
			.objectHandle = reinterpret_cast<uint64_t>(descriptor_set),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif

	const SlotPool<vk::DescriptorSet>::Handle handle = descriptor_sets_.emplace(descriptor_set);
	RID bind_group = _make_rid(ResourceKind::eBindGroup, handle.slot);
	bind_group.lower = handle.generation;
	++allocations_;
	
	// If entries are specified, then we can write to the Descriptor Set
	if (desc.entries.empty())
		return bind_group;
	
	update_bind_group(bind_group, desc.entries);
	
	return bind_group;
}

void VkGraphicsBackend::destroy_bind_group(const RID id) {
	if (!is_valid_rid(id)) return;
	const VkDescriptorSet descriptor_set = get_bind_group(id);
	vkCheck(vkFreeDescriptorSets(device_, descriptor_pool_, 1, &descriptor_set), "Failed to free descriptor set");
	assert(descriptor_sets_.erase(id.upper, id.lower));
}

void VkGraphicsBackend::update_bind_group(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) {
	const VkDescriptorSet descriptor_set = get_bind_group(bind_group_rid);
	
	Vector<VkDescriptorImageInfo> image_infos(entries.size());
	std::size_t image_info_index = 0;
	Vector<VkDescriptorBufferInfo> buffer_infos(entries.size());
	std::size_t buffer_info_index = 0;
	Vector<VkWriteDescriptorSet> write_descriptor_sets;//(entries.size());
	
	for (const BindGroupEntryDescriptor &entry : entries) {
		switch (entry.resource.type) {
		case BindingType::eUniformBuffer:
		case BindingType::eStorageBuffer: {
			auto buffer_binding = std::get<BindingResource::BufferBinding>(entry.resource.binding);
			VkDescriptorBufferInfo buffer_info = {
				.buffer = get_buffer(buffer_binding.buffer),
				.offset = buffer_binding.offset,
				.range = buffer_binding.size
			};
			buffer_infos[buffer_info_index] = buffer_info;
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
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
			VkDescriptorImageInfo sampler_info = {
				.sampler = get_sampler(sampler.sampler),
				.imageView = VK_NULL_HANDLE,
				.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
			};
			image_infos[image_info_index] = sampler_info;
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
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
		case BindingType::eSampledImage:
		case BindingType::eStorageImage: {
			auto image_binding = std::get<BindingResource::ImageBinding>(entry.resource.binding);
			VkDescriptorImageInfo image_info = {
				.imageView = get_image_view(image_binding.image_view),
				.imageLayout = vk::detail::convert(image_binding.layout)
			};
			image_infos[image_info_index] = image_info;
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
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
		case BindingType::eImageSampler: {
			auto combined_binding = std::get<BindingResource::CombinedImageSampler>(entry.resource.binding);
			VkDescriptorImageInfo combined_info = {
				.sampler = get_sampler(combined_binding.sampler),
				.imageView = get_image_view(combined_binding.image_view),
				.imageLayout = vk::detail::convert(combined_binding.layout)
			};
			image_infos[image_info_index] = combined_info;
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
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
		}
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
void VkGraphicsBackend::set_bind_group(const RID command_rid, const RID pipeline_layout_rid, const u32 index,
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

vk::DescriptorSet VkGraphicsBackend::get_bind_group(const RID id) const {
	const vk::DescriptorSet* descriptor_set = descriptor_sets_.get(id.upper, id.lower);
	return *descriptor_set;
}

RID VkGraphicsBackend::create_shader(const SpirvDescriptor &spirv_descriptor) {
	const VkShaderModuleCreateInfo shader_module_create_info = VkShaderModuleCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = spirv_descriptor.code_size,
		.pCode = spirv_descriptor.code
	};

	VkShaderModule shader_module;
	vkCheck(vkCreateShaderModule(device_, &shader_module_create_info, nullptr, &shader_module), "Failed to create shader module");
#ifdef _DEBUG
	if (spirv_descriptor.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SHADER_MODULE,
			.objectHandle = reinterpret_cast<uint64_t>(shader_module),
			.pObjectName = spirv_descriptor.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif

	const SlotPool<vulkan::ShaderStorage>::Handle handle = shader_modules_.emplace(vulkan::ShaderStorage{shader_module, VK_NULL_HANDLE});
	RID rid = _make_rid(ResourceKind::eShaderModule, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_shader(const RID id) {
	const vk::ShaderModule shader_module = get_shader_module(id);
	vkDestroyShaderModule(device_, shader_module, nullptr);
	assert(shader_modules_.erase(id.upper, id.lower));
}

vk::ShaderModule VkGraphicsBackend::get_shader_module(const RID id) const {
	const vulkan::ShaderStorage* storage = shader_modules_.get(id.upper, id.lower);
	return storage->shader_module;
}

vk::ShaderEXT VkGraphicsBackend::get_shader(const RID id) const {
	const vulkan::ShaderStorage* storage = shader_modules_.get(id.upper, id.lower);
	return storage->shader_ext;
}

namespace detail {
	static gfx::Format getFormatFromColorSpace(const Vector<vk::SurfaceFormatKHR> &surface_formats, vk::ColorSpaceKHR const &colorSpace) {
		assert(!surface_formats.empty() && "Surface formats must not be empty");
		for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
			if (surface_format.colorSpace == colorSpace)
				return vk::detail::convert(static_cast<VkFormat>(surface_format.format));
	
		return vk::detail::convert(static_cast<VkFormat>(surface_formats[0].format));
	}

	static vk::ColorSpaceKHR getColorSpaceFromFormat(const Vector<vk::SurfaceFormatKHR> &surface_formats, vk::Format const &format) {
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

RID VkGraphicsBackend::create_surface(IWindow *window, const SurfaceDescriptor &desc) {
	switch (window->get_driver()) {
	case WindowDriver::eSdl2:
		return create_surface_sdl2(dynamic_cast<SDL2Window *>(window), desc);
	case WindowDriver::eGlfw3:
		return create_surface_glfw3(dynamic_cast<GLFW3Window *>(window), desc);
	}
	throw std::exception("Unsupported window driver");
}

RID VkGraphicsBackend::create_surface_universal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc) {
	
#ifdef _DEBUG

	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SURFACE_KHR,
			.objectHandle = reinterpret_cast<uint64_t>(surface),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
	
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
																	  .setCommandBufferCount(2);
	
	Vector<vk::CommandBuffer> allocation = device_.allocateCommandBuffers(command_buffer_allocate_info);
	Array<RID, vulkan::framesInFlight> command_buffers;
	const Array<RID, vulkan::framesInFlight> render_finished_semaphores;
	const Array<RID, vulkan::framesInFlight> image_available_semaphores;
	const Array<RID, vulkan::framesInFlight> graphics_fences;
	
	for (size_t i = 0; i < allocation.size(); ++i) {
		const SlotPool<vulkan::CommandBufferStorage>::Handle command_buffer_handle = command_buffers_.emplace(allocation[i]);
		RID command_buffer_rid = _make_rid(ResourceKind::eCommandBuffer, command_buffer_handle.slot);
		command_buffer_rid.lower = command_buffer_handle.generation;
		command_buffers[i] = command_buffer_rid;
	}

	// Upload half complete Surface then call update_surface_configuration to create swapchain,
	// update_surface_configuration can be used 
	const SlotPool<vulkan::SurfaceStorage>::Handle handle = surfaces_.emplace(vulkan::SurfaceStorage{
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
	
	RID rid = _make_rid(ResourceKind::eSurface, handle.slot);
	rid.lower = handle.generation;
	
	update_surface_configuration(rid, desc);
	return rid;
}

RID VkGraphicsBackend::create_surface_sdl2(SDL2Window *window, const SurfaceDescriptor &desc) {
	SDL_Window* sdl_window = window->get_sdl2_window();
	VkSurfaceKHR surface;
	assert(SDL_Vulkan_CreateSurface(sdl_window, instance_, &surface)&&"Failed to create window surface");
	return create_surface_universal(window, surface, desc);
}

RID VkGraphicsBackend::create_surface_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc) {
	GLFWwindow* glfw_window = window->get_glfw3_window();
	VkSurfaceKHR surface;
	vkCheck(glfwCreateWindowSurface(instance_, glfw_window, nullptr, &surface), "Failed to create a surface from GLFW3");
	return create_surface_universal(window, surface, desc);
}

Vector<gfx::Format> VkGraphicsBackend::get_surface_formats(const RID surface_rid) {
	return {}; // TODO
}

gfx::Format VkGraphicsBackend::get_surface_color_format(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.color_format;
}

RID VkGraphicsBackend::get_active_image(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_images[storage.image_index];
}

RID VkGraphicsBackend::get_active_image_view(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_image_views[storage.image_index];
}

void VkGraphicsBackend::update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) {
	vulkan::SurfaceStorage& storage = get_surface_storage_mutable(surface_rid);
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
			vk::detail::convert(
				target_color_format.value_or(
					detail::getFormatFromColorSpace(surface_formats, color_space)
				)
			)
		);
	}
	else if (target_color_format.has_value()) {
		color_format = static_cast<vk::Format>(target_color_format.value());
		color_space = detail::getColorSpaceFromFormat(surface_formats, color_format);
	}
	else {
		// If nothing is specified, just fallback to some safe options.
		color_format = vk::Format::eB8G8R8A8Srgb;
		color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
	}
	VkSwapchainCreateInfoKHR swapchain_create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = storage.surface,
		.minImageCount = vulkan::framesInFlight,
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
	storage.color_format = vk::detail::convert(static_cast<VkFormat>(color_format));
	if (storage.swapchain != VK_NULL_HANDLE) {
		
		for (const RID image_view_rid : storage.swapchain_image_views)
			destroy_image_view(image_view_rid);
		device_.destroySwapchainKHR(storage.swapchain);
	}
	vk::SwapchainKHR swapchain = device_.createSwapchainKHR(swapchain_create_info);
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SWAPCHAIN_KHR,
			.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkSwapchainKHR>(swapchain)),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
	}
#endif
	
	const Vector<vk::Image> swapchain_images = device_.getSwapchainImagesKHR(swapchain);
	Vector<RID> images; // (swapchain_images.size());
	size_t tick = 0;
	for (const vk::Image& image : swapchain_images) {
		auto name ="Swapchain Image " + std::to_string(tick++);
		VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_IMAGE,
			.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImage>(image)),
			.pObjectName = name.c_str()
		};
		vkSetDebugUtilsObjectName(device_, &name_info);
		
		const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
			.image = image,
			.allocation = VK_NULL_HANDLE,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb)
		});
		RID rid = _make_rid(ResourceKind::eImage, handle.slot);
		rid.lower = handle.generation;
		images.push_back(rid);
	}
	
	Vector<RID> views(images.size());
	for (size_t i = 0; i < images.size(); ++i) {
		const RID image_rid = images[i];
		ImageViewDescriptor imageViewDescriptor{
			.image = image_rid,
			.type = ImageViewType::e2D,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb),
			.usage = ImageUsage::eTransferSrc | ImageUsage::eColorAttachment,
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
	Array<RID, 2> render_finished_semaphores;
	Array<RID, 2> image_available_semaphores;
	Array<RID, 2> graphics_fences;
	for (size_t i = 0; i < vulkan::framesInFlight; ++i) {
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

void VkGraphicsBackend::destroy_surface(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage_mutable(surface_rid);

	if (storage.swapchain != VK_NULL_HANDLE) {
		for (const RID image_view_rid : storage.swapchain_image_views)
			destroy_image_view(image_view_rid);
		for (u32 i = 0; i < storage.graphics_command_buffers.size(); ++i) {
			
			VkFence fence = get_fence(storage.graphics_fences[i]);
			vkCheck(vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
			
			destroy_semaphore(storage.render_finished_semaphores[i]);
			destroy_semaphore(storage.image_available_semaphores[i]);
			destroy_fence(storage.graphics_fences[i]);
		}
		device_.destroySwapchainKHR(storage.swapchain);
	}
	instance_.destroySurfaceKHR(storage.surface);
	assert(surfaces_.erase(surface_rid.upper, surface_rid.lower));
}

const vulkan::SurfaceStorage & VkGraphicsBackend::get_surface_storage(const RID id) const {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id.upper, id.lower);
	return *storage;
}

vulkan::SurfaceStorage & VkGraphicsBackend::get_surface_storage_mutable(const RID id) {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id.upper, id.lower);
	return *const_cast<vulkan::SurfaceStorage*>(storage);
}

RID VkGraphicsBackend::create_pipeline_layout(const PipelineLayoutDescriptor &desc) {
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = static_cast<uint32_t>(desc.bind_group_layouts.size()),
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = static_cast<uint32_t>(desc.push_constants.size()),
		.pPushConstantRanges = nullptr
	};
	
	Vector<VkDescriptorSetLayout> descriptor_set_layouts(desc.bind_group_layouts.size());
	for (size_t i = 0; i < desc.bind_group_layouts.size(); ++i) {
		const vk::DescriptorSetLayout layout = get_bind_group_layout(desc.bind_group_layouts[i]);
		descriptor_set_layouts[i] = layout;
	}
	pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
	
	Vector<VkPushConstantRange> push_constant_ranges(desc.push_constants.size());
	for (size_t i = 0; i < desc.push_constants.size(); ++i) {
		const PushConstantRangeDescriptor &push_constant_range = desc.push_constants[i];
		const VkPushConstantRange range = {
			.stageFlags = vk::detail::convert(push_constant_range.visibility),
			.offset = push_constant_range.offset,
			.size = push_constant_range.size
		};
		push_constant_ranges[i] = range;
	}
	pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();
	
	VkPipelineLayout pipeline_layout;
	vkCreatePipelineLayout(device_, &pipeline_layout_create_info, nullptr, &pipeline_layout);
	const SlotPool<vk::PipelineLayout>::Handle handle = pipeline_layouts_.emplace(pipeline_layout);
	RID rid = _make_rid(ResourceKind::ePipelineLayout, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_pipeline_layout(const RID pipeline_layout_rid) {
	const vk::PipelineLayout pipeline_layout = get_pipeline_layout(pipeline_layout_rid);
	vkDestroyPipelineLayout(device_, pipeline_layout, nullptr);
	assert(pipeline_layouts_.erase(pipeline_layout_rid.upper, pipeline_layout_rid.lower));
}

vk::PipelineLayout VkGraphicsBackend::get_pipeline_layout(const RID rid) {
	const vk::PipelineLayout* pipeline_layout = pipeline_layouts_.get(rid.upper, rid.lower);
	return *pipeline_layout;
}

RID VkGraphicsBackend::create_graphics_pipeline(const GraphicsPipelineDescriptor &desc) {
	
	Vector<VkPipelineShaderStageCreateInfo> stages; // `(desc.stages.size());
	for (const auto& stage_desc : desc.stages) {
		VkPipelineShaderStageCreateInfo stage_create_info = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = vk::detail::convert2(stage_desc.stage),
			.module = get_shader_module(stage_desc.shader),
			.pName = stage_desc.entry_point.c_str()
		};
		stages.push_back(stage_create_info);
	}
	
	Vector<VkVertexInputBindingDescription> binding_descriptions; // (desc.vertex_input.bindings.size());
	for (const auto& binding_desc : desc.vertex_input.bindings) {
		VkVertexInputBindingDescription binding_description = {
			.binding = binding_desc.binding,
			.stride = binding_desc.stride,
			.inputRate = vk::detail::convert(binding_desc.input_rate)
		};
		binding_descriptions.push_back(binding_description);
	}
	
	Vector<VkVertexInputAttributeDescription> attribute_descriptions; // (desc.vertex_input.attributes.size());
	for (const auto& attribute_desc : desc.vertex_input.attributes) {
		VkVertexInputAttributeDescription attribute_description = {
			.location = attribute_desc.location,
			.binding = attribute_desc.binding,
			.format = vk::detail::convert(attribute_desc.format),
			.offset = attribute_desc.offset
		};
		attribute_descriptions.push_back(attribute_description);
	}
	
	VkPipelineVertexInputStateCreateInfo vertex_input_states{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = static_cast<uint32_t>(desc.vertex_input.bindings.size()),
		.pVertexBindingDescriptions = binding_descriptions.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(desc.vertex_input.attributes.size()),
		.pVertexAttributeDescriptions = attribute_descriptions.data()
	};
	
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = vk::detail::convert(desc.input_assembly.primitive_topology),
		.primitiveRestartEnable = desc.input_assembly.primitive_restart_enable ? VK_TRUE : VK_FALSE
	};
	
	Vector<VkViewport> viewports(desc.viewport.viewports.size());
	for (auto i = 0; i < desc.viewport.viewports.size(); ++i) {
		Viewport viewport_desc = desc.viewport.viewports[i];
		VkViewport viewport = {
			.x = viewport_desc.x,
			.y = viewport_desc.y,
			.width = viewport_desc.width,
			.height = viewport_desc.height,
			.minDepth = viewport_desc.min_depth,
			.maxDepth = viewport_desc.max_depth
		};
	}
	
	Vector<VkRect2D> scissors(desc.viewport.scissors.size());
	for (auto i = 0; i < desc.viewport.scissors.size(); ++i) {
		Rect2D scissor_desc = desc.viewport.scissors[i];
		scissors[i] = {
			.offset = {.x = scissor_desc.offset.x, .y = scissor_desc.offset.y },
			.extent = {.width = scissor_desc.extent.width, .height = scissor_desc.extent.height }
		};
	}
	
	VkPipelineViewportStateCreateInfo viewport_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = static_cast<uint32_t>(viewports.size()),
		.pViewports = viewports.data(),
		.scissorCount = static_cast<uint32_t>(scissors.size()),
		.pScissors = scissors.data()
	};
	
	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = desc.rasterization.depth_clamp_enable ? VK_TRUE : VK_FALSE,
		.rasterizerDiscardEnable = desc.rasterization.rasterizer_discard_enable ? VK_TRUE : VK_FALSE,
		.polygonMode = vk::detail::convert(desc.rasterization.polygon_mode),
		.cullMode = vk::detail::convert(desc.rasterization.cull_mode),
		.frontFace = vk::detail::convert(desc.rasterization.front_face),
		.depthBiasEnable = desc.rasterization.depth_bias_enable ? VK_TRUE : VK_FALSE,
		.depthBiasConstantFactor = desc.rasterization.depth_bias_constant_factor,
		.depthBiasClamp = desc.rasterization.depth_bias_clamp,
		.depthBiasSlopeFactor = desc.rasterization.depth_bias_slope_factor,
		.lineWidth = desc.rasterization.line_width
	};
	
	VkPipelineMultisampleStateCreateInfo multi_sample_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = vk::detail::convert(desc.multisample.rasterization_samples),
		.sampleShadingEnable = desc.multisample.sample_shading_enable ? VK_TRUE : VK_FALSE,
		.minSampleShading = desc.multisample.min_sample_shading,
		.pSampleMask = nullptr,            //< Not implemented
		.alphaToCoverageEnable = VK_FALSE, //< Not implemented
		.alphaToOneEnable = VK_FALSE       //< Not implemented
	};
	
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = desc.depth_stencil.depth_test ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = desc.depth_stencil.depth_write ? VK_TRUE : VK_FALSE,
		.depthCompareOp = vk::detail::convert(desc.depth_stencil.depth_compare_op),
		.depthBoundsTestEnable = desc.depth_stencil.depth_bounds_test ? VK_TRUE : VK_FALSE,
		.stencilTestEnable = desc.depth_stencil.stencil_test ? VK_TRUE : VK_FALSE,
		.front = VkStencilOpState{
			.failOp = vk::detail::convert(desc.depth_stencil.front.fail_op),
			.passOp = vk::detail::convert(desc.depth_stencil.front.pass_op),
			.depthFailOp = vk::detail::convert(desc.depth_stencil.front.depth_fail_op),
			.compareOp = vk::detail::convert(desc.depth_stencil.front.compare_op),
			.compareMask = desc.depth_stencil.front.compare_mask,
			.writeMask = desc.depth_stencil.front.write_mask,
			.reference = desc.depth_stencil.front.reference
		},
		.back = VkStencilOpState{
			.failOp = vk::detail::convert(desc.depth_stencil.back.fail_op),
			.passOp = vk::detail::convert(desc.depth_stencil.back.pass_op),
			.depthFailOp = vk::detail::convert(desc.depth_stencil.back.depth_fail_op),
			.compareOp = vk::detail::convert(desc.depth_stencil.back.compare_op),
			.compareMask = desc.depth_stencil.back.compare_mask,
			.writeMask = desc.depth_stencil.back.write_mask,
			.reference = desc.depth_stencil.back.reference
		},
		.minDepthBounds = desc.depth_stencil.min_depth_bounds,
		.maxDepthBounds = desc.depth_stencil.max_depth_bounds
	};
	
	VkPipelineColorBlendAttachmentState color_blend_attachment_state{
		.blendEnable = false,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	
	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment_state
	};
	
	Vector<VkDynamicState> dynamic_states;//(desc.dynamic_states.size());
	for (const DynamicState &state : desc.dynamic_states) {
		dynamic_states.push_back(vk::detail::convert(state));
	}
	
	VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
		.pDynamicStates = dynamic_states.data()
	};
	
	Vector<VkFormat> color_attachment_formats;//(desc.rendering.color_formats.size());
	for (const auto& color_format : desc.rendering.color_formats) {
		color_attachment_formats.push_back(vk::detail::convert(color_format));
		break;
	}
	VkFormat depth_attachment_format = vk::detail::convert(
		desc.rendering.depth_format.value_or(gfx::Format::eUndefined));
	
	VkFormat stencil_attachment_format = vk::detail::convert(
		desc.rendering.stencil_format.value_or(gfx::Format::eUndefined));
	
	VkPipelineRenderingCreateInfo rendering_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
		.pColorAttachmentFormats = color_attachment_formats.data(),
		.depthAttachmentFormat = depth_attachment_format,
		.stencilAttachmentFormat = stencil_attachment_format
	};
	
	VkGraphicsPipelineCreateInfo pipeline_create_info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &rendering_create_info,
		.stageCount = static_cast<uint32_t>(desc.stages.size()),
		.pStages = stages.data(),
		.pVertexInputState = &vertex_input_states,
		.pInputAssemblyState = &input_assembly_state_create_info,
		.pTessellationState = nullptr, //< Not implemented for now.
		.pViewportState = &viewport_state_create_info,
		.pRasterizationState = &rasterization_state_create_info,
		.pMultisampleState = &multi_sample_state_create_info,
		.pDepthStencilState = &depth_stencil_state_create_info,
		.pColorBlendState = &color_blend_state_create_info,
		.pDynamicState = &dynamic_state_create_info,
		.layout = get_pipeline_layout(desc.layout)
	};
	
	VkPipeline pipeline;
	VkResult result = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 
												1, &pipeline_create_info, nullptr, &pipeline);
	vkCheck(result, "Failed to create graphics pipeline");
	
	const SlotPool<vk::Pipeline>::Handle handle = pipelines_.emplace(pipeline);
	RID rid = _make_rid(ResourceKind::ePipeline, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::bind_pipeline(const RID pipeline, const RID cmd_rid, const PipelineBindPoint bind_point) {
	const VkPipeline vk_pipeline = get_pipeline(pipeline);
	const VkCommandBuffer command_buffer = get_command_buffer(cmd_rid);
	vkCmdBindPipeline(command_buffer, vk::detail::convert(bind_point), vk_pipeline);
}

void VkGraphicsBackend::destroy_pipeline(const RID pipeline_rid) {
	const vk::Pipeline pipeline = get_pipeline(pipeline_rid);
	vkDestroyPipeline(device_, pipeline, nullptr);
	assert(pipelines_.erase(pipeline_rid.upper, pipeline_rid.lower));
}

vk::Pipeline VkGraphicsBackend::get_pipeline(const RID id) const {
	const vk::Pipeline* pipeline = pipelines_.get(id.upper, id.lower);
	return *pipeline;
}

RID VkGraphicsBackend::create_fence(const Optional<String> &label, const bool signaled) {
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
	
	const SlotPool<vk::Fence>::Handle handle = fences_.emplace(fence);
	RID rid = _make_rid(ResourceKind::eFence, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_fence(const RID fence_rid) {
	const vk::Fence fence = get_fence(fence_rid);
	vkDestroyFence(device_, fence, nullptr);
	assert(fences_.erase(fence_rid.upper, fence_rid.lower));
}

vk::Fence VkGraphicsBackend::get_fence(const RID id) const {
	const vk::Fence* fence = fences_.get(id.upper, id.lower);
	return *fence;
}

RID VkGraphicsBackend::create_semaphore(const SemaphoreType semaphore_type, const Optional<String> &label) {
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

	const SlotPool<vk::Semaphore>::Handle handle = semaphores_.emplace(semaphore);
	RID rid = _make_rid(ResourceKind::eSemaphore, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::destroy_semaphore(const RID semaphore_rid) {
	const vk::Semaphore semaphore = get_semaphore(semaphore_rid);
	vkDestroySemaphore(device_, semaphore, nullptr);
	assert(semaphores_.erase(semaphore_rid.upper, semaphore_rid.lower));
}

vk::Semaphore VkGraphicsBackend::get_semaphore(const RID id) const {
	const vk::Semaphore* semaphore = semaphores_.get(id.upper, id.lower);
	return *semaphore;
}

RID VkGraphicsBackend::begin(const RID surface_rid) {
	using namespace vk::detail;
	vulkan::SurfaceStorage& surface_storage = get_surface_storage_mutable(surface_rid);
	const VkCommandBuffer command = get_command_buffer(surface_storage.graphics_command_buffers[surface_storage.frame_index]);
	const VkFence fence = get_fence(surface_storage.graphics_fences[surface_storage.frame_index]);
	const VkSwapchainKHR swapchain = surface_storage.swapchain;
	
	constexpr VkCommandBufferBeginInfo begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};
	
	vkCheck(vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
	vkCheck(vkResetFences(device_, 1, &fence), "Failed to reset fence");
	
	const VkSemaphore image_available_semaphore = get_semaphore(surface_storage.image_available_semaphores[surface_storage.frame_index]);
	uint32_t* image_index_ptr = &surface_storage.image_index;
	vkCheck(vkAcquireNextImageKHR(device_, swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, image_index_ptr), "Failed to acquire next swapchain image!");
	vkCheck(vkResetCommandBuffer(command, 0), "Failed to reset command buffer");
	vkCheck(vkBeginCommandBuffer(command, &begin_info), "Failed to begin command buffer recording");
	
	return surface_storage.graphics_command_buffers[surface_storage.frame_index];
}

uint32_t VkGraphicsBackend::begin_rendering(const RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) {
	using namespace vk::detail;
	const vulkan::SurfaceStorage& surface_storage = get_surface_storage_mutable(surface_rid);
	vulkan::CommandBufferStorage& command_buffer_storage = get_command_buffer_storage(command_rid);
	const VkPipeline pipeline = get_pipeline(pipeline_rid);

	const vulkan::ImageViewStorage* active_image_view_storage = image_views_.get(surface_storage.swapchain_image_views[surface_storage.image_index]);
	const vulkan::ImageViewStorage* depth_image_view_storage = image_views_.get(depth_image_view);
	
	const VkRenderingAttachmentInfo color_attachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO, .pNext = nullptr,
		.imageView = active_image_view_storage->image_view,
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = {
			.color = {
				VkClearColorValue{{0.0f, 0.0f, 0.0f, 1.0f}} 
			}
		}
	};
	
	const VkRenderingAttachmentInfo depth_attachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO, .pNext = nullptr,
		.imageView = depth_image_view_storage->image_view,
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue = { .depthStencil = {.depth = 1.0f, .stencil = 0 } }
	};

	const VkRenderingInfo rendering_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO, .pNext = nullptr, .flags = 0,
		.renderArea = { 
			.offset = {
				.x = 0,
				.y = 0
			},
			.extent = {
				.width = static_cast<u32>(surface_storage.window->get_size().x),
				.height = static_cast<u32>(surface_storage.window->get_size().y)
			}
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment,
		.pDepthAttachment = &depth_attachment,
		.pStencilAttachment = nullptr
	};

	const vulkan::ImageStorage* active_image = images_.get(active_image_view_storage->image);
	const vulkan::ImageStorage* depth_image = images_.get(depth_image_view_storage->image);
	
	transition(command_rid, {
		ImageTransitionDescriptor{
			.image = active_image_view_storage->image,
			.src = {
				.layout = active_image->layout,
				.access = active_image->access,
				.stage = active_image->stage
			},
			.dst = {
				.layout = ImageLayout::eAttachmentOptimal,
				.access = Access::eColorAttachmentWrite,
				.stage = PipelineStage::eColorAttachmentOutput
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = Aspect::eColor,
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}
		},
		ImageTransitionDescriptor{
			.image = depth_image_view_storage->image,
			.src = {
				.layout = depth_image->layout,
				.access = depth_image->access,
				.stage = depth_image->stage
			},
			.dst = {
				.layout = ImageLayout::eAttachmentOptimal,
				.access = Access::eDepthStencilAttachmentWrite,
				.stage = PipelineStage::eEarlyFragmentTests
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = BitFlag(Aspect::eDepth) | BitFlag(Aspect::eStencil),
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}
		}
	});
	
	vkCmdBeginRendering(command_buffer_storage.command_buffer, &rendering_info);

	const VkViewport window_viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(surface_storage.window->get_size().x),
		.height = static_cast<float>(surface_storage.window->get_size().y),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	
	vkCmdSetViewport(command_buffer_storage.command_buffer, 0, 1, &window_viewport);

	const VkRect2D window_scissor{
		.offset = {
			.x = 0,
			.y = 0
		},
		.extent = {
			.width = static_cast<u32>(surface_storage.window->get_size().x),
			.height = static_cast<u32>(surface_storage.window->get_size().y)
		}
	};
	vkCmdSetScissor(command_buffer_storage.command_buffer, 0, 1, &window_scissor);
	
	vkCmdBindPipeline(command_buffer_storage.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	command_buffer_storage.connected_surface = surface_rid;
	
	return surface_storage.image_index;
}

void VkGraphicsBackend::finish_rendering(const RID command_rid) {
	const vulkan::CommandBufferStorage &command_storage = get_command_buffer_storage(command_rid);
	vkCmdEndRendering(command_storage.command_buffer);
	
	// Not all rendering is done onto a surface, so we don't assert for a surface, we just exit early if we don't have one.
	if (!command_storage.connected_surface.has_value())
		return;
	
	const vulkan::SurfaceStorage &surface_storage = get_surface_storage(command_storage.connected_surface.value());
	
	transition(command_rid, {
		ImageTransitionDescriptor{
			.image = surface_storage.swapchain_images[surface_storage.image_index],
			.src = {
				.layout = ImageLayout::eAttachmentOptimal,
				.access = Access::eColorAttachmentWrite,
				.stage = PipelineStage::eColorAttachmentOutput
			},
			.dst = {
				.layout = ImageLayout::ePresent,
				.access = Access::eNone,
				.stage = PipelineStage::eColorAttachmentOutput
			},
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = Aspect::eColor,
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}
		}
	});
}

void VkGraphicsBackend::finish(const RID command_rid) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkCheck(vkEndCommandBuffer(command), "Failed to end command buffer recording");
}

void VkGraphicsBackend::bind_shader(const RID command_rid, RID shader_rid, ShaderStage stage) {
	bind_shader(command_rid, { shader_rid }, { stage });
}

void VkGraphicsBackend::bind_shader(const RID command_rid, const Vector<RID> shader_rids, const Vector<ShaderStage> stages) {
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

void VkGraphicsBackend::bind_shader(const RID command_rid, const Vector<BindShaderDescriptor> shader_descriptors) {
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

void VkGraphicsBackend::transition(const RID command_rid, const ImageTransitionDescriptor &descriptor) {
	transition(command_rid, Vector{ descriptor });
}

void VkGraphicsBackend::transition(const RID command_rid, const RID image, const ImageLayout layout, BitFlag<Access> access, BitFlag<PipelineStage> stage, ImageSubresourceDescriptor subresource) {
	vulkan::ImageStorage &image_storage = get_image_storage_mutable(image); // Ensure the image exists
	
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

void VkGraphicsBackend::transition(const RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) {
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
	}
	const VkDependencyInfo dependency_info{
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
		.pImageMemoryBarriers = barriers.data(),
	};
	vkCmdPipelineBarrier2(command, &dependency_info);

	for (const ImageTransitionDescriptor &descriptor : descriptors) {
		vulkan::ImageStorage &image_storage = get_image_storage_mutable(descriptor.image);
		image_storage.layout = descriptor.dst.layout;
		image_storage.access = descriptor.dst.access;
		image_storage.stage = descriptor.dst.stage;
	}
}

uint32_t VkGraphicsBackend::queue_family(const QueueFamilyType queue_family) const {
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

void VkGraphicsBackend::submit(const RID command_rid) {
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

void VkGraphicsBackend::present(const RID surface_rid) {
	vulkan::SurfaceStorage& surface_storage = get_surface_storage_mutable(surface_rid);
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
	vkCheck(
		vkQueuePresentKHR(graphics_queue_[current_graphics_queue], &present_info),
		"Failed to present swapchain image"
	);
	
	surface_storage.frame_index = (surface_storage.frame_index + 1) % vulkan::framesInFlight;
	current_graphics_queue = (current_graphics_queue + 1) % graphics_queue_.size();
}

void VkGraphicsBackend::push_label(const RID command_rid, const String &label) {
	const VkDebugUtilsLabelEXT label_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pNext = nullptr,
		.pLabelName = label.c_str(),
		.color = {0.0f, 0.0f, 0.0f, 0.0f}
	};
	vkCmdBeginDebugUtilsLabel(get_command_buffer(command_rid), &label_info);
}

void VkGraphicsBackend::pop_label(const RID command_rid) {
	vkCmdEndDebugUtilsLabel(get_command_buffer(command_rid));
}

void VkGraphicsBackend::push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) {
	vkCmdPushConstants(get_command_buffer(command_rid), get_pipeline_layout(pipeline_layout_rid), 
					   vk::detail::convert(BitFlag(descriptor.visibility)), descriptor.offset, descriptor.size, data);
}

vk::CommandBuffer VkGraphicsBackend::get_command_buffer(const RID id) const {
	const vulkan::CommandBufferStorage* storage = command_buffers_.get(id.upper, id.lower);
	return storage->command_buffer;
}

vulkan::CommandBufferStorage & VkGraphicsBackend::get_command_buffer_storage(const RID id) {
	vulkan::CommandBufferStorage* storage = command_buffers_.get(id.upper, id.lower);
	return *storage;
}

const vulkan::CommandBufferStorage & VkGraphicsBackend::get_command_buffer_storage(const RID id) const {
	const vulkan::CommandBufferStorage* storage = command_buffers_.get(id.upper, id.lower);
	return *storage;
}

void VkGraphicsBackend::bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) {
	bind_vertex_buffers(command_rid, { desc });
}

void VkGraphicsBackend::bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	
	Vector<VkBuffer> buffers;     //(desc.size());
	Vector<VkDeviceSize> offsets; //(desc.size());
	for (const VertexBufferDescriptor &buffer_desc : desc) {
		buffers.push_back(get_buffer(buffer_desc.buffer));
		offsets.push_back(buffer_desc.offset);
	}
	vkCmdBindVertexBuffers(command, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VkGraphicsBackend::bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkCmdBindIndexBuffer(command, get_buffer(desc.buffer), desc.offset, vk::detail::convert(desc.index_type));
}

void VkGraphicsBackend::DrawIndexed(const RID command_rid, const std::uint32_t first_index, const std::uint32_t index_count) {
	draw_indexed(command_rid, index_count, 1, first_index, 0, 0);
}

void VkGraphicsBackend::draw_indexed(const RID command_rid, const std::uint32_t index_count, const std::uint32_t instance_count, const std::uint32_t first_index, const std::int32_t vertex_offset, const std::uint32_t first_instance) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawIndexed(commandBuffer, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void VkGraphicsBackend::draw_indexed_indirect(const RID command_rid, const RID buffer, const u64 buffer_offset, const RID count_buffer, const u64 count_buffer_offset, const u32 max_draw_count, const u32 stride) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	const VkBuffer vk_buffer = get_buffer(buffer);
	if (count_buffer.valid()) {
		const VkBuffer vk_count_buffer = get_buffer(count_buffer);
		vkCmdDrawIndexedIndirectCount(commandBuffer, vk_buffer, buffer_offset, vk_count_buffer, count_buffer_offset, max_draw_count, stride);
	} else {
		vkCmdDrawIndexedIndirect(commandBuffer, vk_buffer, buffer_offset, max_draw_count, stride);
	}
}

void VkGraphicsBackend::dispatch(const RID command_rid, const uvec3 groups) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDispatch(commandBuffer, groups.x, groups.y, groups.z);
}

void VkGraphicsBackend::dispatch(const RID command_rid, const u32 groups_x, const u32 groups_y, const u32 groups_z) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDispatch(commandBuffer, groups_x, groups_y, groups_z);
}

void VkGraphicsBackend::dispatch_mesh(const RID command_rid, const uvec3 groups) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawMeshTasks(commandBuffer, groups.x, groups.y, groups.z);
}

void VkGraphicsBackend::dispatch_mesh(const RID command_rid, const u32 groups_x, const u32 groups_y, const u32 groups_z) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawMeshTasks(commandBuffer, groups_x, groups_y, groups_z);
}

void VkGraphicsBackend::prune_dead_objects() {
}

vk::Buffer VkGraphicsBackend::get_buffer(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	return storage->buffer;
}

VmaAllocation VkGraphicsBackend::get_buffer_allocation(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	return storage->allocation;
}

VmaAllocationInfo VkGraphicsBackend::get_buffer_allocation_info(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	VmaAllocationInfo allocation_info;
	vmaGetAllocationInfo(allocator_, storage->allocation, &allocation_info);
	return allocation_info;
}

void VkGraphicsBackend::wait_for_idle() {
	vkDeviceWaitIdle(device_);
}

void VkGraphicsBackend::prune() {
	std::scoped_lock lock(transfer_mutex_);
	for (auto &transfer : image_transfers_)
		switch (const VkResult result = vkWaitForFences(device_, 1, &transfer.fence, VK_TRUE, 1)) {
		case VK_TIMEOUT:
			continue;
		case VK_SUCCESS:
			// Thread local command pools... look into how well this would work with transient command buffers.
			vkFreeCommandBuffers(device_, transfer.owning_pool, 1, &transfer.command_buffer);
			vkDestroyFence(device_, transfer.fence, nullptr);
			break;
		default:
			vkCheck(result, "Failed to wait for fence");
			break;
		}
}

void VkGraphicsBackend::create_instance() {
	uint32_t extension_count = 0;
	const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

	Vector<const char*> extensions(extension_count);
	for (uint32_t u = 0u; u < extension_count; u++)
		extensions[u] = required_instance_extensions[u];
	
#ifdef _DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	Vector enables ={
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
		.setPApplicationName("Standard Application")
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
	
	printf("VK: Extensions present are (%u) %s\n", extension_count, extension_list.c_str());
	
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

void VkGraphicsBackend::request_adapter() {
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

void VkGraphicsBackend::create_device_and_queues() {
	const std::vector<vk::QueueFamilyProperties> queue_families = adapter_.getQueueFamilyProperties();
	
	bool found_graphics_queue = false;
	bool found_compute_queue = false;
	bool found_transfer_queue = false;
	
	for (uint32_t queue_index = 0; queue_index < queue_families.size(); queue_index++)
	{
		vk::QueueFamilyProperties properties = queue_families[queue_index];
		
		if (!found_graphics_queue &&
			properties.queueFlags & vk::QueueFlagBits::eGraphics //&&
			//adapter.getSurfaceSupportKHR(queue_index, surface)
		) {
			found_graphics_queue = true;
			graphics_queue_family_index_ = queue_index;
			graphics_queue_.resize(properties.queueCount);
			continue;
		}
		
		if (!found_compute_queue &&
			properties.queueFlags & vk::QueueFlagBits::eCompute) {
			found_compute_queue = true;
			compute_queue_family_index_ = queue_index;
			compute_queue_.resize(properties.queueCount);
			continue;
		}
		
		auto dedicated_transfer_queue_flags = vk::QueueFlagBits::eTransfer & ~vk::QueueFlagBits::eGraphics & ~vk::QueueFlagBits::eCompute;
		if (!found_transfer_queue &&
			properties.queueFlags & dedicated_transfer_queue_flags ) {
			found_transfer_queue = true;
			transfer_queue_family_index_ = queue_index;
			transfer_queue_.resize(properties.queueCount);
			continue;
		}
		
		if (found_graphics_queue && found_compute_queue && found_transfer_queue)
			break;
	}

	constexpr float queue_priority = 0.5000f;
	
	auto device_features_12 = vk::PhysicalDeviceVulkan12Features()
		.setDescriptorIndexing(true)
		.setShaderSampledImageArrayNonUniformIndexing(true)
		.setBufferDeviceAddress(true)
		.setRuntimeDescriptorArray(true)
		.setDescriptorBindingVariableDescriptorCount(true)
		.setDescriptorBindingSampledImageUpdateAfterBind(true)
		.setShaderInt8(true)
		.setScalarBlockLayout(true)
		.setDescriptorBindingUpdateUnusedWhilePending(true);

	auto device_features_13 = vk::PhysicalDeviceVulkan13Features()
		.setPNext(&device_features_12)
		.setDynamicRendering(true)
		.setSynchronization2(true)
		.setMaintenance4(true);

	auto device_features_14 = vk::PhysicalDeviceVulkan14Features()
		.setPNext(&device_features_13)
		.setPipelineRobustness(true)
		.setPushDescriptor(true)
		.setMaintenance5(true)
		.setMaintenance6(true);
	
	const auto &mesh_shader_features = vk::PhysicalDeviceMeshShaderFeaturesEXT()
		.setPNext(&device_features_14)
		.setPrimitiveFragmentShadingRateMeshShader(true)
		.setMeshShaderQueries(true)
		.setMeshShader(true)
		.setTaskShader(true);
	
	const auto &device_features = vk::PhysicalDeviceFeatures()
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
	
	bool ngq = queue_create_info[0].queueCount == graphics_queue_.size();
	bool ncq = queue_create_info[1].queueCount == compute_queue_.size();
	bool ntq = queue_create_info[2].queueCount == transfer_queue_.size();
	queue_create_info[0].queueCount = graphics_queue_.size();
	queue_create_info[1].queueCount = compute_queue_.size();
	queue_create_info[2].queueCount = transfer_queue_.size();

	std::vector device_extensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
		VK_EXT_MESH_SHADER_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME
	};

	const vk::DeviceCreateInfo &device_create_info = vk::DeviceCreateInfo()
		.setPNext(&mesh_shader_features)
		.setPEnabledExtensionNames(device_extensions)
		.setQueueCreateInfos(queue_create_info)
		.setPEnabledFeatures(&device_features);
	
	device_ = adapter_.createDevice(device_create_info);
	
	load_device_extension_functions();
	
	for (std::uint32_t index = 0; index < graphics_queue_.size(); ++index) {
		graphics_queue_[index] = device_.getQueue(graphics_queue_family_index_, index);
#ifdef _DEBUG
		std::string name = "Graphics Queue #" + std::to_string(index);
		VkDebugUtilsObjectNameInfoEXT queue_name{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_QUEUE,
			.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkQueue>(graphics_queue_[index])),
			.pObjectName = name.c_str()
		};
		vkSetDebugUtilsObjectName(device_, &queue_name);
#endif
	}
	for (std::uint32_t index = 0; index < compute_queue_.size(); ++index) {
		compute_queue_[index] = device_.getQueue(compute_queue_family_index_, index);
#ifdef _DEBUG
		std::string name = "Compute Queue #" + std::to_string(index);
		VkDebugUtilsObjectNameInfoEXT queue_name{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_QUEUE,
			.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkQueue>(compute_queue_[index])),
			.pObjectName = name.c_str()
		};
		vkSetDebugUtilsObjectName(device_, &queue_name);
#endif
	}
	for (std::uint32_t index = 0; index < transfer_queue_.size(); ++index) {
		transfer_queue_[index] = device_.getQueue(transfer_queue_family_index_, index);
#ifdef _DEBUG
		std::string name = "Transfer Queue #" + std::to_string(index);
		VkDebugUtilsObjectNameInfoEXT queue_name{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_QUEUE,
			.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkQueue>(transfer_queue_[index])),
			.pObjectName = name.c_str()
		};
		vkSetDebugUtilsObjectName(device_, &queue_name);
#endif
	}
}

void VkGraphicsBackend::create_allocator() {
	VmaVulkanFunctions vulkanFunctions = {
		.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
		.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
		.vkGetPhysicalDeviceProperties =  vkGetPhysicalDeviceProperties,
		.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
		.vkAllocateMemory = vkAllocateMemory,
		.vkFreeMemory = vkFreeMemory,
		.vkMapMemory = vkMapMemory,
		.vkUnmapMemory = vkUnmapMemory,
		.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
		.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
		.vkBindBufferMemory = vkBindBufferMemory,
		.vkBindImageMemory = vkBindImageMemory,
		.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
		.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
		.vkCreateBuffer = vkCreateBuffer,
		.vkDestroyBuffer = vkDestroyBuffer,
		.vkCreateImage = vkCreateImage,
		.vkDestroyImage = vkDestroyImage,
		.vkCmdCopyBuffer = vkCmdCopyBuffer,
		.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
		.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
		.vkGetMemoryWin32HandleKHR = nullptr
	};

	const VmaAllocatorCreateInfo allocatorCreateInfo = {
		.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
		.physicalDevice = adapter_,
		.device = device_,
		.pVulkanFunctions = &vulkanFunctions,
		.instance = instance_
	};

	const VkResult result = vmaCreateAllocator(
		&allocatorCreateInfo,
		&allocator_
	);
	vkCheck(result, "Failed to create Vulkan Memory Allocator");
}

void VkGraphicsBackend::create_default_pools() {

	const vk::DescriptorPoolCreateInfo descriptor_pool_create_info = vk::DescriptorPoolCreateInfo()
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
		.setMaxSets(100);
	
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

void VkGraphicsBackend::dispose() {
	shutdown();
}

bool VkGraphicsBackend::disposed() const {
	return instance_ == VK_NULL_HANDLE && device_ == VK_NULL_HANDLE && allocator_ == VK_NULL_HANDLE;
}

void VkGraphicsBackend::load_instance_extension_functions() {
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

void VkGraphicsBackend::load_device_extension_functions() {
	ext.vkCreateShaders		= (PFN_vkCreateShadersEXT)vkGetDeviceProcAddr(device_, "vkCreateShadersEXT");
	ext.vkCmdBindShaders	= (PFN_vkCmdBindShadersEXT)vkGetDeviceProcAddr(device_, "vkCmdBindShadersEXT");
}

// ReSharper disable CppMemberFunctionMayBeConst
VkResult VkGraphicsBackend::vkCreateDebugUtilsMessenger(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT*pMessenger) {
	return ext.vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

void VkGraphicsBackend::vkDestroyDebugUtilsMessenger(const VkInstance instance, const VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator) {
	ext.vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

void VkGraphicsBackend::vkCmdBeginDebugUtilsLabel(const VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

void VkGraphicsBackend::vkCmdInsertDebugUtilsLabel(const VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

void VkGraphicsBackend::vkCmdEndDebugUtilsLabel(const VkCommandBuffer commandBuffer) {
	ext.vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}

void VkGraphicsBackend::vkQueueBeginDebugUtilsLabel(const VkQueue queue, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}

void VkGraphicsBackend::vkQueueInsertDebugUtilsLabel(const VkQueue queue, const VkDebugUtilsLabelEXT *pLabelInfo) {
	ext.vkQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}

void VkGraphicsBackend::vkQueueEndDebugUtilsLabel(const VkQueue queue) {
	ext.vkQueueEndDebugUtilsLabelEXT(queue);
}

VkResult VkGraphicsBackend::vkSetDebugUtilsObjectName(const VkDevice device, const VkDebugUtilsObjectNameInfoEXT *pNameInfo) {
	return ext.vkSetDebugUtilsObjectNameEXT(device, pNameInfo);
}

void VkGraphicsBackend::vkCmdDrawMeshTasks(const VkCommandBuffer commandBuffer, const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ) {
	ext.vkCmdDrawMeshTasks(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VkGraphicsBackend::vkCmdDrawMeshTasksIndirect(const VkCommandBuffer commandBuffer, const VkBuffer buffer, const VkDeviceSize offset,
	const uint32_t drawCount, const uint32_t stride) {
	ext.vkCmdDrawMeshTasksIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

VkResult VkGraphicsBackend::vkCreateShaders(const VkDevice device, const uint32_t createInfoCount,
	const VkShaderCreateInfoEXT *pCreateInfos, const VkAllocationCallbacks *pAllocator, VkShaderEXT*pShaders) {
	return ext.vkCreateShaders(device, createInfoCount, pCreateInfos, pAllocator, pShaders);
}

void VkGraphicsBackend::vkDestroyShader(const VkDevice device, const VkShaderEXT shader, const VkAllocationCallbacks *pAllocator) {
	ext.vkDestroyShader(device, shader, pAllocator);
}

void VkGraphicsBackend::vkCmdBindShaders(const VkCommandBuffer commandBuffer, const uint32_t stageCount, const VkShaderStageFlagBits *pStages, const VkShaderEXT *pShaders) {
	ext.vkCmdBindShaders(commandBuffer, stageCount, pStages, pShaders);
}
// ReSharper restore CppMemberFunctionMayBeConst