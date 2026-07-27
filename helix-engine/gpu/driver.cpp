// ReSharper disable CppTooWideScopeInitStatement
#include "driver.hpp"

#include <glfw/glfw3.h>
#include "gltf.h"
#include "window.hpp"

#define VMA_LEAK_LOG_FORMAT(format, ...) do { \
        printf((format), __VA_ARGS__); \
        printf("\n"); \
    } while(false)

#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
       printf((format), __VA_ARGS__); \
       printf("\n"); \
   } while(false)

#define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "mesh.hpp"

void GraphicsDriver::createInstance() {
	uint32_t extension_count = 0;
	const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

	Vec<const char*> extensions(extension_count);
	for (uint32_t u = 0u; u < extension_count; u++) {
		extensions[u] = required_instance_extensions[u];
	}

	const Vec<const char*> layers = {
#ifdef _DEBUG
		//"VK_LAYER_KHRONOS_validation"
#endif
	};
	
	const auto applicationInfo = vk::ApplicationInfo()
		.setApiVersion(VK_API_VERSION_1_4)
		.setApplicationVersion(1)
		.setPApplicationName("Standard Application")
		.setEngineVersion(1)
		.setPEngineName("Helix");

	const auto instanceCreateInfo = vk::InstanceCreateInfo()
		.setPApplicationInfo(&applicationInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
		.setPpEnabledExtensionNames(extensions.data())
		.setEnabledLayerCount((uint32_t)layers.size())
		.setPpEnabledLayerNames(layers.data());
	
	instance = vk::createInstance(instanceCreateInfo);
}

void GraphicsDriver::requestAdapter() {
	const Vec<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();
	
	vk::PhysicalDevice chosen_adapter;
	const f64 chosen_adapter_score = 0.0f;
	
	for (vk::PhysicalDevice physical_device : physical_devices) {
		f64 adapter_score = 0.0f;
		const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
		
		adapter_score += static_cast<f64>(properties.limits.maxImageDimension2D);
		
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			adapter_score *= 1000000.0f;
			chosen_adapter = physical_device;
		}
	}
	
	adapter = chosen_adapter;
}

void GraphicsDriver::createDeviceAndQueues() {
	const std::vector<vk::QueueFamilyProperties> queue_families = adapter.getQueueFamilyProperties();
	
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
			graphics_queue_index = queue_index;
			graphics_queue.resize(1);
			continue;
		}
		
		if (!found_compute_queue &&
			properties.queueFlags & vk::QueueFlagBits::eCompute) {
			found_compute_queue = true;
			compute_queue_index = queue_index;
			compute_queue.resize(1);
			continue;
		}
		
		auto dedicated_transfer_queue_flags = vk::QueueFlagBits::eTransfer & ~vk::QueueFlagBits::eGraphics & ~vk::QueueFlagBits::eCompute;
		if (!found_transfer_queue &&
			properties.queueFlags & dedicated_transfer_queue_flags ) {
			found_transfer_queue = true;
			transfer_queue_index = queue_index;
			transfer_queue.resize(1);
			continue;
		}
		
		if (found_graphics_queue && found_compute_queue && found_transfer_queue)
			break;
	}

	constexpr float queue_priority = 0.9f;

	auto &device_features_12 = vk::PhysicalDeviceVulkan12Features()
		.setDescriptorIndexing(true)
		.setShaderSampledImageArrayNonUniformIndexing(true)
		.setBufferDeviceAddress(true)
		.setRuntimeDescriptorArray(true)
		.setDescriptorBindingVariableDescriptorCount(true);

	auto &device_features_13 = vk::PhysicalDeviceVulkan13Features()
		.setPNext(&device_features_12)
		.setDynamicRendering(true)
		.setSynchronization2(true);

	const auto &device_features_14 = vk::PhysicalDeviceVulkan14Features()
		.setPNext(&device_features_13)
		.setPipelineRobustness(true)
		.setPushDescriptor(true);
	
	const auto &device_features = vk::PhysicalDeviceFeatures()
		.setSamplerAnisotropy(true)
		.setFragmentStoresAndAtomics(true);

	const Array<vk::DeviceQueueCreateInfo, 3> queue_create_info = {
		vk::DeviceQueueCreateInfo()
			.setQueueFamilyIndex(graphics_queue_index)
			.setQueueCount(static_cast<uint32_t>(graphics_queue.size()))
			.setQueuePriorities({ queue_priority }),
		vk::DeviceQueueCreateInfo()
			.setQueueFamilyIndex(compute_queue_index)
			.setQueueCount(static_cast<uint32_t>(compute_queue.size()))
			.setQueuePriorities({ queue_priority }),
		vk::DeviceQueueCreateInfo()
			.setQueueFamilyIndex(transfer_queue_index)
			.setQueueCount(static_cast<uint32_t>(transfer_queue.size()))
			.setQueuePriorities({ queue_priority })
	};

	std::vector device_extensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const auto &device_create_info = vk::DeviceCreateInfo()
		.setPNext(&device_features_14)
		.setPEnabledExtensionNames(device_extensions)
		.setQueueCreateInfos(queue_create_info)
		.setPEnabledFeatures(&device_features);
	
	device = adapter.createDevice(device_create_info);
	
	for (std::uint32_t index = 0; index < graphics_queue.size(); ++index)
		graphics_queue[index] = device.getQueue(graphics_queue_index, index);
	
	for (std::uint32_t index = 0; index < compute_queue.size(); ++index)
		compute_queue[index] = device.getQueue(compute_queue_index, index);
	
	for (std::uint32_t index = 0; index < transfer_queue.size(); ++index)
		transfer_queue[index] = device.getQueue(transfer_queue_index, index);
}

void GraphicsDriver::createAllocator() {
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
		.physicalDevice = adapter,
		.device = device,
		.pVulkanFunctions = &vulkanFunctions,
		.instance = instance
	};

	const VkResult result = vmaCreateAllocator(
		&allocatorCreateInfo,
		&allocator
	);
	assert(result == VK_SUCCESS);
}

void GraphicsDriver::createPools() {
	std::vector descriptor_pool_sizes = {
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(2)
	};

	const vk::DescriptorPoolCreateInfo descriptor_pool_create_info = vk::DescriptorPoolCreateInfo()
		.setPoolSizes(descriptor_pool_sizes)
		.setMaxSets(2);
	
	descriptor_pool = device.createDescriptorPool(descriptor_pool_create_info);

	using enum vk::CommandPoolCreateFlagBits;
	const vk::CommandPoolCreateInfo transfer_command_pool_create_info = vk::CommandPoolCreateInfo()
		.setFlags(eTransient)
		.setQueueFamilyIndex(transfer_queue_index);
	
	transfer_command_pool = device.createCommandPool(transfer_command_pool_create_info);

	const vk::CommandPoolCreateInfo graphics_command_pool_create_info = vk::CommandPoolCreateInfo()
		.setFlags(eResetCommandBuffer)
		.setQueueFamilyIndex(graphics_queue_index);
	
	command_pool = device.createCommandPool(graphics_command_pool_create_info);
}

GraphicsDriver::GraphicsDriver() : graphics_queue_index(0), compute_queue_index(0), transfer_queue_index(0),
                                   allocator(nullptr) {
}

GraphicsDriver::~GraphicsDriver() {
	stop();
}

void GraphicsDriver::start() {
	createInstance();
	requestAdapter();
	createDeviceAndQueues();
	createAllocator();
	createPools();
}

void GraphicsDriver::stop() {
	
	graphics_queue[0].waitIdle();
	
	for (const BufferStorage& storage : buffers)
		if (storage.buffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(allocator, storage.buffer, storage.allocation);

	for (const auto id : image_views | std::views::keys)
		image_view_delete(id);
	
	for (const auto id : images | std::views::keys)
		image_delete(id);
	
	for (const auto id : samplers | std::views::keys)
		sampler_delete(id);
	
	for (const auto id : shader_modules | std::views::keys)
		shader_delete(id);
	
	for (const auto id : pipelines | std::views::keys) {
		pipeline_delete(id);
	}
	
	for (auto* window : windows)
		if (window != nullptr)
			window->dispose();
	
	device.destroyCommandPool(command_pool);
	device.destroyCommandPool(transfer_command_pool);
	device.destroyDescriptorPool(descriptor_pool);

	vmaDestroyAllocator(allocator);
	device.destroy();
	instance.destroy();
}

GraphicsDriver * GraphicsDriver::singleton() {
	static GraphicsDriver instance;
	return &instance;
}

RID GraphicsDriver::buffer_create(const VkBufferCreateInfo& buffer_create_info, const VmaAllocationCreateInfo& allocation_info) {
	VkBuffer buffer;
	VmaAllocation allocation;
	
	const VkResult result = vmaCreateBuffer(allocator,
		&buffer_create_info,
		&allocation_info,
		&buffer,
		&allocation,
		nullptr
	);
	assert(result == VK_SUCCESS);

	const BufferStorage bufferStorage = {
		.buffer = buffer,
		.allocation = allocation,
		.is_allocated = true
	};

	const RID rid(static_cast<u32>(buffers.size()), ++allocations);
	
	buffers.push_back(bufferStorage);
	// printf("Buffer created %llu\n", *(u64*)&rid);
	return rid;
}

void GraphicsDriver::buffer_delete(const RID id) {
	assert(static_cast<size_t>(id.upper) < buffers.size() && "out of range");
	auto& [buffer, allocation, is_allocated] = _buffer_storage_inner(id);
	if (!is_allocated) return;
	//assert(!is_allocated && "Free after free");
	vmaDestroyBuffer(allocator, buffer, allocation);
	buffer = VK_NULL_HANDLE;
	allocation = VK_NULL_HANDLE;
	is_allocated = false;
}

void GraphicsDriver::buffer_set_allocation_name(RID handle, const char* name) {
	const BufferStorage& storage = _buffer_storage_inner(handle);
	vmaSetAllocationName(allocator, storage.allocation, name);
}

bool GraphicsDriver::buffer_exists(const RID id) const {
	if (static_cast<size_t>(id.upper) >= buffers.size()) return false;
	const BufferStorage& storage = _buffer_storage_inner(id);
	return storage.is_allocated;
}

VmaAllocationInfo GraphicsDriver::buffer_get_allocation_info(const RID id) {
	const BufferStorage& storage = _buffer_storage_inner(id);
	const VmaAllocation allocation = storage.allocation;
	VmaAllocationInfo allocation_info;
	vmaGetAllocationInfo(allocator, allocation, &allocation_info);
	return allocation_info;
}

void* GraphicsDriver::buffer_get_mapped_address(const RID id) {
	return buffer_get_allocation_info(id.upper).pMappedData;
}

vk::DeviceAddress GraphicsDriver::buffer_get_device_address(const RID id) {
	const vk::BufferDeviceAddressInfo buffer_device_address_info = vk::BufferDeviceAddressInfo()
		.setBuffer(_buffer_inner(id));
	return device.getBufferAddress(buffer_device_address_info);
}

RID GraphicsDriver::image_create(const VkImageCreateInfo &image_create_info, const VmaAllocationCreateInfo &allocation_info) {
	
	VkImage image;
	VmaAllocation allocation;
	
	const VkResult result = vmaCreateImage(allocator,
		&image_create_info,
		&allocation_info,
		&image,
		&allocation,
		nullptr
	);
	assert(result == VK_SUCCESS);

	const ImageStorage image_storage = {
		.image = image,
		.allocation = allocation
	};

	const RID rid(++images_made, ++allocations);
	
	images[rid] = image_storage;
	return rid;
}

void GraphicsDriver::image_load_from_buffer(const RID image, const RID buffer, const vk::BufferImageCopy &copy) {
	image_load_from_buffer(image, buffer, std::vector{ copy });
}

void GraphicsDriver::image_load_from_buffer(const RID image, const RID buffer, const Vec<vk::BufferImageCopy> &copy) {
	const vk::Image i = images[image].image;
	const vk::Buffer b = buffers[buffer].buffer;
	
	vk::Fence fence = device.createFence(vk::FenceCreateInfo());

	const vk::CommandBufferAllocateInfo command_buffer_allocate_info = vk::CommandBufferAllocateInfo()
		.setCommandPool(transfer_command_pool)
		.setLevel(vk::CommandBufferLevel::eSecondary)
		.setCommandBufferCount(1);
	
	vk::CommandBuffer command = device.allocateCommandBuffers(command_buffer_allocate_info)[0];

	const auto bi = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	
	command.begin(bi);
	
	vk::ImageMemoryBarrier2 image_barrier = vk::ImageMemoryBarrier2()
		.setOldLayout(vk::ImageLayout::eUndefined)
		.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
		.setImage(i);

	const vk::DependencyInfo dependency_info = vk::DependencyInfo()
		.setImageMemoryBarriers({ image_barrier });
	
	command.pipelineBarrier2(dependency_info);
	command.copyBufferToImage(b, i, vk::ImageLayout::eTransferDstOptimal, copy);
	
	command.end();

	const vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBuffers({ command });
	
	transfer_queue[0].submit(submit_info, fence);
	
	dead_command_buffers.push_back(std::make_shared<DeadCommandBuffer>(command, fence));
}

void GraphicsDriver::image_delete(const RID id) {
	auto [image, allocation] = images[id];
	vmaDestroyImage(allocator, image, allocation);
	images.erase(images.find(id));
}

RID GraphicsDriver::image_view_create(const RID image, vk::ImageViewCreateInfo &image_view_create_info) {
	const ImageStorage &storage = images[image];
	image_view_create_info.image = (VkImage)storage.image;
	const RID id(++image_views_made, ++allocations);
	image_views[id] = device.createImageView(image_view_create_info);
	return id;
}

RID GraphicsDriver::image_view_create(const vk::ImageViewCreateInfo &image_view_create_info) {
	const RID id(++image_views_made, ++allocations);
	image_views[id] = device.createImageView(image_view_create_info);
	return id;
}

void GraphicsDriver::image_view_delete(const RID id) {
	const vk::ImageView image_view = image_views[id];
	device.destroyImageView(image_view);
	image_views.erase(image_views.find(id));
}

RID GraphicsDriver::sampler_create(const vk::SamplerCreateInfo &sampler_create_info) {
	const RID id(++samplers_made, ++allocations);
	samplers[id] = device.createSampler(sampler_create_info);
	return id;
}

void GraphicsDriver::sampler_delete(const RID sampler) {
	device.destroySampler(samplers[sampler]);
	samplers.erase(samplers.find(sampler));
}

RID GraphicsDriver::shader_create() {
	const RID id(++shader_modules_made,++allocations);
	shader_modules[id] = VK_NULL_HANDLE;
	return id;
}

void GraphicsDriver::shader_delete(const RID id) {
	device.destroyShaderModule(shader_modules[id]);
}

void GraphicsDriver::shader_load_spirv_from_file(const RID shader_module, const std::string_view file_path) {
	std::ifstream file((file_path.data()), std::ios::ate | std::ios::binary);
	const std::streamsize size = file.tellg();
	std::vector<char> buffer(size);
	file.seekg(0);
	file.read(buffer.data(), size);
	
	shader_upload_spirv(shader_module, reinterpret_cast<u32*>(buffer.data()), buffer.size());
}

void GraphicsDriver::shader_upload_spirv(const RID shader_module, Vec<u32> spirv) {
	const vk::ShaderModuleCreateInfo shader_module_create_info = vk::ShaderModuleCreateInfo()
		.setCode(spirv);
	shader_modules[shader_module] = device.createShaderModule(shader_module_create_info);
}

void GraphicsDriver::shader_upload_spirv(const RID shader_module, const u32 *spirv, const size_t spirv_length) {
	const vk::ShaderModuleCreateInfo shader_module_create_info = vk::ShaderModuleCreateInfo()
		.setPCode(spirv)
		.setCodeSize(spirv_length);
	shader_modules[shader_module] = device.createShaderModule(shader_module_create_info);
}

void GraphicsDriver::prune_dead_objects() {
	for (const std::shared_ptr<DeadCommandBuffer> &dead : dead_command_buffers) {
		const vk::Result result = device.waitForFences({ dead->fence }, vk::True, 1);
		if (result == vk::Result::eSuccess) {
			device.destroyFence(dead->fence);
			std::erase(dead_command_buffers, dead);
		}
	}
}

RID GraphicsDriver::start_recording(Window *render_target) {
	assert(render_target != nullptr && "Are you serious");
	
	if (!render_target->has_command_buffers) {
		const vk::CommandBufferAllocateInfo commandBufferAllocateInfo = vk::CommandBufferAllocateInfo()
			.setCommandBufferCount(Window::framesInFlight)
			.setCommandPool(command_pool)
			.setLevel(vk::CommandBufferLevel::ePrimary);

		const vk::ResultValueType<std::vector<vk::CommandBuffer>>::type commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);

		const RID cb1(++command_buffers_made, ++allocations);
		const RID cb2(++command_buffers_made, ++allocations);
		
		command_buffers[cb1] = commandBuffers[0];
		command_buffers[cb2] = commandBuffers[1];
		
		render_target->graphics_command_buffers = { cb1, cb2 };
		render_target->has_command_buffers = true;
	}
	
	vk::Fence fence = fences[render_target->graphics_fences[render_target->frame_index]];
	const vk::Result result = device.waitForFences({ fence }, vk::True, INFINITE);
	assert(result == vk::Result::eSuccess && "waitForFences error");
	device.resetFences({ fence });
	
	const RID cmd_rid = render_target->graphics_command_buffers[render_target->frame_index];
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];
	
	cmd.reset();

	const vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmd.begin(begin_info);

	const vk::ResultValue<uint32_t> nextImage = device.acquireNextImageKHR(
		render_target->swapchain_khr, UINT64_MAX,
		semaphores[render_target->image_available_semaphores[render_target->frame_index]], fence
	);
	assert(nextImage.result == vk::Result::eSuccess && "Failed @ acquireNextImageKHR");
	render_target->image_index = nextImage.value;
	
	std::array barriers = {
		vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setSrcAccessMask(vk::AccessFlagBits2::eNone)
			.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite)
			.setOldLayout(vk::ImageLayout::ePresentSrcKHR)
			.setNewLayout(vk::ImageLayout::eAttachmentOptimal)
			.setImage(render_target->swapchain_images[render_target->image_index])
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setLevelCount(1)
				.setLayerCount(1)),
		vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eLateFragmentTests)
			.setSrcAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eAttachmentOptimal)
			.setNewLayout(vk::ImageLayout::eAttachmentOptimal)
			.setImage(images[render_target->depth_image].image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
				.setLevelCount(1)
				.setLayerCount(1))
	};

	const vk::DependencyInfo dependency_info = vk::DependencyInfo()
		.setImageMemoryBarriers(barriers);
	
	cmd.pipelineBarrier2(dependency_info);
	
	return cmd_rid;
}

void GraphicsDriver::start_rendering(const Window *render_target, const RID cmd_rid, const RID pipeline_rid) {
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];
	
	std::array color_attachments = {
		vk::RenderingAttachmentInfo()
			.setImageView(image_views[render_target->swapchain_image_views[render_target->image_index]])
			.setImageLayout(vk::ImageLayout::eAttachmentOptimal)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setClearValue(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f))
	};

	const std::array depth_attachment = {
		vk::RenderingAttachmentInfo()
			.setImageView(image_views[render_target->depth_image_view])
			.setImageLayout(vk::ImageLayout::eAttachmentOptimal)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setClearValue(vk::ClearDepthStencilValue(1.0f, 0))
	};

	const vk::Rect2D render_area(vk::Offset2D(0, 0), render_target->extent);

	const vk::RenderingInfo rendering_info = vk::RenderingInfo()
		.setColorAttachments(color_attachments)
		.setPDepthAttachment(depth_attachment.data())
		.setRenderArea(render_area)
		.setLayerCount(1);
	
	cmd.beginRendering(rendering_info);

	const std::array viewport = {
		vk::Viewport()
			.setWidth(static_cast<f32>(render_target->extent.width))
			.setHeight(static_cast<f32>(render_target->extent.height))
			.setX(0.0f)
			.setY(0.0f)
	};
	
	cmd.setViewport(0, viewport);
	cmd.setScissor(0, render_area);
	
	PipelineStorage& storage = pipelines[pipeline_rid];
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, storage.pipeline);
}

void GraphicsDriver::stop_rendering(Window *render_target, const RID cmd_rid) {
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];
	cmd.endRendering();
}

void GraphicsDriver::stop_recording(const Window *render_target, const RID cmd_rid) {
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];
	
	std::array barriers = {
		vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setSrcAccessMask(vk::AccessFlagBits2::eNone)
			.setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eAttachmentOptimal)
			.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
			.setImage(render_target->swapchain_images[render_target->image_index])
			.setSubresourceRange(vk::ImageSubresourceRange()
							 .setAspectMask(vk::ImageAspectFlagBits::eColor)
							 .setLevelCount(1)
							 .setLayerCount(1)),
		vk::ImageMemoryBarrier2()
			.setSrcStageMask(vk::PipelineStageFlagBits2::eLateFragmentTests)
			.setSrcAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eAttachmentOptimal)
			.setNewLayout(vk::ImageLayout::eAttachmentOptimal)
			.setImage(images[render_target->depth_image].image)
			.setSubresourceRange(vk::ImageSubresourceRange()
							 .setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
							 .setLevelCount(1)
							 .setLayerCount(1))
	};

	const vk::DependencyInfo dependency_info = vk::DependencyInfo()
		.setImageMemoryBarriers(barriers);
	
	cmd.pipelineBarrier2(dependency_info);
	cmd.end();
}

void GraphicsDriver::submit(const Window *render_target, const RID cmd_rid) {
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];

	using namespace vk;
	constexpr const PipelineStageFlags2 wait_stages = PipelineStageFlagBits2::eColorAttachmentOutput;
	
	// TODO: Move all synchronization objects to each window!  
	SubmitInfo submit = SubmitInfo()
		.setWaitDstStageMask({ *reinterpret_cast<const vk::PipelineStageFlags*>(&wait_stages) })
		.setSignalSemaphores({ semaphores[render_target->render_finished_semaphores[render_target->image_index]] })
		.setWaitSemaphores({ semaphores[render_target->image_available_semaphores[render_target->image_index]] })
		.setCommandBuffers({ cmd });
	
	graphics_queue[0].submit({ submit }, fences[render_target->graphics_fences[render_target->frame_index]]);
	render_target->frame_index = (render_target->frame_index + 1) % Window::framesInFlight;
}

void GraphicsDriver::present(Window *render_target) {
	using namespace vk;
	const vk::PresentInfoKHR present_info = vk::PresentInfoKHR()
		.setWaitSemaphores({ semaphores[render_target->render_finished_semaphores[render_target->image_index]] })
		.setSwapchains({ render_target->swapchain_khr })
		.setImageIndices({ render_target->image_index });
	
	vk::Result present_result = vk::Result::eSuccess;
	try {
		present_result = graphics_queue[0].presentKHR(present_info);
	}
	catch (std::exception &e) {
		std::cerr << e.what();
	}
}

RID GraphicsDriver::pipeline_create(Window* surface, const RID shader) {
	using namespace vk;

	const ShaderModule module = shader_modules[shader];
	
	std::array descriptor_set_layout_bindings = {
		DescriptorSetLayoutBinding()
			.setStageFlags(ShaderStageFlagBits::eVertex | ShaderStageFlagBits::eFragment)
			.setDescriptorType(DescriptorType::eUniformBuffer)
			.setDescriptorCount(1)
			.setBinding(0)
	};

	const auto descriptor_set_layout_create_info = DescriptorSetLayoutCreateInfo()
		.setFlags(DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
		.setBindings(descriptor_set_layout_bindings);
	
	DescriptorSetLayout set_layout = device.createDescriptorSetLayout(descriptor_set_layout_create_info);
	
	DescriptorBindingFlags flags = DescriptorBindingFlagBits::eUpdateAfterBind |
		DescriptorBindingFlagBits::eUpdateUnusedWhilePending |
		DescriptorBindingFlagBits::eVariableDescriptorCount |
		DescriptorBindingFlagBits::ePartiallyBound;
	
	Array push_constant_ranges = {
		vk::PushConstantRange()
			.setStageFlags(vk::ShaderStageFlagBits::eVertex)
			.setSize(2*sizeof(vk::DeviceAddress))
			.setOffset(0)
	};

	const PipelineLayoutCreateInfo layout_create_info = PipelineLayoutCreateInfo()
		.setPushConstantRanges(push_constant_ranges)
		.setSetLayouts(set_layout);

	const PipelineLayout pipeline_layout = device.createPipelineLayout(layout_create_info);
	
	/* Shader Stages */
	
	std::array shader_stage_create_info = {
		PipelineShaderStageCreateInfo()
			.setStage(ShaderStageFlagBits::eVertex)
			.setPName("main")
			.setModule(module),
		PipelineShaderStageCreateInfo()
			.setStage(ShaderStageFlagBits::eFragment)
			.setPName("main")
			.setModule(module)
	};
	
	/* Vertex Input State */

	const PipelineVertexInputStateCreateInfo vertex_input_state_create_info = Vertex::inputState();
	
	/* Input Assembly */

	constexpr PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = PipelineInputAssemblyStateCreateInfo()
		.setTopology(PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);
	
	/* Viewport State */

	constexpr PipelineViewportStateCreateInfo viewport_state_create_info = PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setScissorCount(1);
	
	/* Rasterization State */
	
	PipelineRasterizationStateCreateInfo rasterization_state_create_info = PipelineRasterizationStateCreateInfo()
		.setDepthClampEnable(false)
		.setDepthBiasEnable(false)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(PolygonMode::eFill)
		.setCullMode(CullModeFlagBits::eBack)
		.setFrontFace(FrontFace::eCounterClockwise)
		.setLineWidth(1.0f);
	
	/* Multisample State */
	
	PipelineMultisampleStateCreateInfo multisample_state_create_info = PipelineMultisampleStateCreateInfo()
		.setRasterizationSamples(SampleCountFlagBits::e1)
		.setSampleShadingEnable(false);
	
	/* Depth Stencil State */
	
	PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = PipelineDepthStencilStateCreateInfo()
		.setDepthCompareOp(CompareOp::eLess)
		.setDepthBoundsTestEnable(false)
		.setStencilTestEnable(false)
		.setDepthWriteEnable(false)
		.setDepthTestEnable(false);
	
	/* Color Blend State */
	
	std::array color_blend_attachment_state = {
		PipelineColorBlendAttachmentState()
			.setColorWriteMask(ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | 
				ColorComponentFlagBits::eB | ColorComponentFlagBits::eA)
			.setBlendEnable(false)
	};
	
	PipelineColorBlendStateCreateInfo color_blend_state_create_info = PipelineColorBlendStateCreateInfo()
		.setAttachments(color_blend_attachment_state);
	
	/* Dynamic State */
	
	std::array dynamic_states = {
		DynamicState::eViewport,
		DynamicState::eScissor
	};
	
	PipelineDynamicStateCreateInfo dynamic_state_create_info = PipelineDynamicStateCreateInfo()
		.setDynamicStates(dynamic_states);
		
	/* Pipeline Rendering Create Info */
	
	std::array color_attachment_formats = { surface->color_format };
	const PipelineRenderingCreateInfo pipeline_rendering_create_info = PipelineRenderingCreateInfo()
		.setColorAttachmentFormats(color_attachment_formats)
		.setDepthAttachmentFormat(surface->depth_format);
	
	GraphicsPipelineCreateInfo pipeline_create_info = GraphicsPipelineCreateInfo()
		.setPNext(&pipeline_rendering_create_info)
		.setStages(shader_stage_create_info)
		.setPVertexInputState(&vertex_input_state_create_info)
		.setPInputAssemblyState(&input_assembly_state_create_info)
		.setPViewportState(&viewport_state_create_info)
		.setPRasterizationState(&rasterization_state_create_info)
		.setPMultisampleState(&multisample_state_create_info)
		.setPDepthStencilState(&depth_stencil_state_create_info)
		.setPColorBlendState(&color_blend_state_create_info)
		.setPDynamicState(&dynamic_state_create_info)
		.setLayout(pipeline_layout);
	
	auto pipeline = device.createGraphicsPipeline(nullptr, pipeline_create_info);
	assert(pipeline.result == vk::Result::eSuccess && "Failed to create graphics pipeline");
	
	PipelineStorage storage{
		.layout = pipeline_layout,
		.pipeline = pipeline.value,
		.set_layouts = { set_layout }
	};
	
	RID pipeline_rid(++pipelines_made, ++allocations);
	pipelines[pipeline_rid] = storage;
	
	return pipeline_rid;
}

void GraphicsDriver::pipeline_delete(const RID pipeline_rid) {
	const PipelineStorage& storage = pipelines[pipeline_rid];
	device.destroyPipeline(storage.pipeline);
	device.destroyPipelineLayout(storage.layout);
	for (const vk::DescriptorSetLayout &set_layout : storage.set_layouts)
		device.destroyDescriptorSetLayout(set_layout);
}

RID GraphicsDriver::descriptor_set_create(const RID pipeline, const uint32_t set_index) {
	PipelineStorage &storage = pipelines[pipeline];
	
	const RID descriptor_rid(++descriptor_sets_made, ++allocations);

	const vk::DescriptorSetAllocateInfo descriptor_allocate_info = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(descriptor_pool)
		.setDescriptorSetCount(1)
		.setSetLayouts({storage.set_layouts[set_index]});

	const std::vector<vk::DescriptorSet> descriptors = device.allocateDescriptorSets(descriptor_allocate_info);
	sets[descriptor_rid] = descriptors[0];
	
	return descriptor_rid;
}

void GraphicsDriver::descriptor_set_write(const RID descriptor_set_rid, const uint32_t binding, const RID buffer_rid) {
	const vk::Buffer buffer = _buffer_inner(buffer_rid);

	const vk::DescriptorSet set = sets[descriptor_set_rid];
	
	std::array buffer_info = {
		vk::DescriptorBufferInfo()
			.setBuffer(buffer)
			.setOffset(0)
			.setRange(vk::WholeSize)
	};
	
	vk::WriteDescriptorSet write_descriptor_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setBufferInfo(buffer_info)
		.setDescriptorCount(1)
		.setDstBinding(binding)
		.setDstArrayElement(0)
		.setDstSet(set);
	
	device.updateDescriptorSets({ write_descriptor_set }, {});
}

RID GraphicsDriver::fence_create(const bool signaled) {
	vk::FenceCreateInfo fence_create_info = vk::FenceCreateInfo();
	if (signaled)
		fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

	const RID fence_rid(++fences_made, ++allocations);
	fences[fence_rid] = device.createFence(fence_create_info);
	return fence_rid;
}

void GraphicsDriver::fence_delete(const RID fence_rid) {
	device.destroyFence(fences[fence_rid]);
}

RID GraphicsDriver::semaphore_create() {
	const RID semaphore_rid(++semaphores_made, ++allocations);
	
	vk::SemaphoreTypeCreateInfo semaphore_type_create_info = vk::SemaphoreTypeCreateInfo()
		.setSemaphoreType(vk::SemaphoreType::eBinary)
		.setInitialValue(0);
	
	semaphores[semaphore_rid] = device.createSemaphore(vk::SemaphoreCreateInfo().setPNext(&semaphore_type_create_info));
	
	return semaphore_rid;
}

void GraphicsDriver::semaphore_delete(const RID semaphore_rid) {
	device.destroySemaphore(semaphores[semaphore_rid]);
}

const BufferStorage & GraphicsDriver::_buffer_storage_inner(const RID handle) const {
	const BufferStorage &storage = buffers[handle.upper];
	return storage;
}

BufferStorage & GraphicsDriver::_buffer_storage_inner(const RID handle) {
	BufferStorage &storage = buffers[handle.upper];
	return storage;
}

vk::Buffer GraphicsDriver::_buffer_inner(const RID handle) const {
	const BufferStorage& storage = _buffer_storage_inner(handle);
	return storage.buffer;
}

void GraphicsDriver::pipeline_bind(const RID pipeline, const RID cmd_rid, const vk::PipelineBindPoint bind_point) {
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];
	cmd.bindPipeline(bind_point, pipelines[pipeline].pipeline);
}

void GraphicsDriver::pipeline_bind_descriptor_sets(const RID pipeline, const RID cmd_rid, const vk::PipelineBindPoint bind_point,
	const uint32_t first, const uint32_t set_count, const RID *descriptor_set_rids, const uint32_t offset_count, const uint32_t* offsets) {
	
	const vk::CommandBuffer cmd = command_buffers[cmd_rid];

	const PipelineStorage &storage = pipelines[pipeline];
	
	std::vector<vk::DescriptorSet> vk_descriptor_sets(set_count);
	
	for (uint32_t set = 0; set < set_count; ++set)
		vk_descriptor_sets[set] = sets[descriptor_set_rids[set]];

	cmd.bindDescriptorSets(bind_point, storage.layout, first, set_count,
		vk_descriptor_sets.data(), offset_count, offsets);
}

void GraphicsDriver::push_constants(const RID command_rid, const RID pipeline_rid, const vk::ShaderStageFlags stage, const uint32_t offset, const uint32_t size, const void *data) {
	const vk::CommandBuffer cmd = command_buffers[command_rid];
	const PipelineStorage& storage = pipelines[pipeline_rid];
	cmd.pushConstants(storage.layout, stage, offset, size, data);
}

void GraphicsDriver::bind_index_buffer(const RID command_rid, const RID buffer_rid, const vk::IndexType index_type, const vk::DeviceSize offset) {
	const vk::CommandBuffer cmd = command_buffers[command_rid];
	const vk::Buffer buffer = _buffer_inner(buffer_rid);
	cmd.bindIndexBuffer(buffer, offset, index_type);
}

void GraphicsDriver::bind_vertex_buffer(const RID command_rid, const RID buffer_rid) {
	const vk::CommandBuffer cmd = command_buffers[command_rid];
	const vk::Buffer buffer = _buffer_inner(buffer_rid);
	static constexpr VkDeviceSize offsets = 0;
	cmd.bindVertexBuffers(0, 1, &buffer, &offsets);
}

void GraphicsDriver::bind_vertex_buffers(const RID command_rid, const size_t buffer_count, const RID *buffer_rids) {
	const vk::CommandBuffer cmd = command_buffers[command_rid];
	Vec<vk::Buffer> buffers_vector(buffer_count);
	Vec<VkDeviceSize> offsets_vector(buffer_count);
	for (size_t u = 0; u < buffer_count; ++u) {
		buffers_vector.push_back(buffers[buffer_rids[u]].buffer);
		offsets_vector.push_back(0);
	}
	cmd.bindVertexBuffers(0, buffer_count, buffers_vector.data(), offsets_vector.data());
}

void GraphicsDriver::draw_indexed(const RID command_rid, const uint32_t start, const uint32_t count) {
	const vk::CommandBuffer cmd = command_buffers[command_rid];
	cmd.drawIndexed(count, 1, start, 0, 0);
}

RID GraphicsDriver::pipeline_layout_create() {
	return 0;
}

RID GraphicsDriver::command_pool_create() {
	vk::CommandPoolCreateInfo command_pool_create_info = vk::CommandPoolCreateInfo();
	return 0;
}
