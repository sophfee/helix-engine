// ReSharper disable CppClangTidyClangDiagnosticMissingDesignatedFieldInitializers
// ReSharper disable CppClangTidyClangDiagnosticMissingFieldInitializers
// ReSharper disable CppClangTidyMiscUseAnonymousNamespace
// ReSharper disable CppTooWideScopeInitStatement
// ReSharper disable CppVariableCanBeMadeConstexpr
#include "vulkan_backend.hpp"

#include <vma/vk_mem_alloc.h>
#include "detail/vulkan_enum_conversion.hpp"
#include "ecs/transform.h"
#include "glfw/glfw3.h"
#include "gpu/window.hpp"

using detail::has_flag;
using namespace gfx;

#ifdef _DEBUG

VkBool32 vulkan::vkDebugMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
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
	
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
		message += "[GENERAL] ";
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
		message += "[VALIDATION] ";
	else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		message += "[PERFORMANCE] ";
	
	message += pCallbackData->pMessage;
	std::cout << message << '\n';
	return false;
}

#endif

void vkResultCheckInner(VkResult result, const char *file, int line, const char *msg) {
	char message[4096] = {};
	
	int err = sprintf_s(message, 4096, "[%s:%d] Vulkan error: %s", file, line, msg);
	
	vk::detail::resultCheck(static_cast<vk::Result>(result), message);
}

VkGraphicsBackend::VkGraphicsBackend() : debug_messenger_(nullptr), allocator_(nullptr), graphics_queue_index_(0),
                                         compute_queue_index_(0), transfer_queue_index_(0) {
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
	__debugbreak();
	for (const vulkan::BufferStorage& storage : buffers_)
		if (storage.buffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(allocator_, storage.buffer, storage.allocation);
	buffers_.clear();

	for (const vk::ImageView id : image_views_)
		device_.destroyImageView(id);
	image_views_.clear();
	
	for (const vulkan::ImageStorage& id : images_) {
		vmaDestroyImage(allocator_, id.image, id.allocation);
	}
	images_.clear();
	
	for (const vk::Sampler id : samplers_)
		device_.destroySampler(id);
	samplers_.clear();
	
	for (const vk::ShaderModule id : shader_modules_)
		device_.destroyShaderModule(id);
	shader_modules_.clear();
	
	for (const vk::PipelineLayout id : pipeline_layouts_) {
		device_.destroyPipelineLayout(id);
	}
	pipeline_layouts_.clear();
	
	for (const vk::Pipeline pipeline : pipelines_)
		device_.destroyPipeline(pipeline);
	pipelines_.clear();
	
	for (const vk::DescriptorSet id : descriptor_sets_)
		device_.freeDescriptorSets(descriptor_pool_, { id });
	descriptor_sets_.clear();
	
	for (const vk::CommandPool id : command_pools_)
		device_.destroyCommandPool(id);
	command_pools_.clear();
	
	for (auto* window : windows_)
		if (window != nullptr)
			window->dispose();
	windows_.clear();
	
	device_.destroyCommandPool(command_pool_);
	device_.destroyCommandPool(transfer_command_pool_);
	device_.destroyDescriptorPool(descriptor_pool_);

	vmaDestroyAllocator(allocator_);
	device_.destroy();
	
#ifdef _DEBUG
	vkDestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
#endif
	
	instance_.destroy();
}

RID VkGraphicsBackend::buffer_create(const BufferDescriptor &desc) {
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
	
	vmaCreateBuffer(allocator_, &buffer_create_info, &allocation_info, &buffer, &allocation, nullptr);
	
#ifdef _DEBUG
	
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_BUFFER,
			.objectHandle = reinterpret_cast<uint64_t>(buffer),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
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

void VkGraphicsBackend::buffer_delete(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	if (storage && storage->is_allocated) {
		vmaDestroyBuffer(allocator_, storage->buffer, storage->allocation);
		--allocations_;
	}
}

void VkGraphicsBackend::buffer_set_name(const RID buffer_rid, const char *name) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaSetAllocationName(allocator_, allocation, name);
}

void * VkGraphicsBackend::buffer_map(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	void *mapped_data;
	VkResult result = vmaMapMemory(allocator_, allocation, &mapped_data);
	vkResultCheck(result, "Failed to map buffer memory");
	return mapped_data;
}

void VkGraphicsBackend::buffer_unmap(const RID buffer_rid) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaUnmapMemory(allocator_, allocation);
}

void * VkGraphicsBackend::buffer_mapped_data(const RID id) {
	const VmaAllocationInfo allocation_info = get_buffer_allocation_info(id);
	return allocation_info.pMappedData;
}

GpuDeviceAddress VkGraphicsBackend::buffer_virtual_address(const RID id) {
	const vulkan::BufferStorage *storage = buffers_.get(id.upper, id.lower);
	const VkBufferDeviceAddressInfo buffer_address_info = VkBufferDeviceAddressInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.pNext = nullptr,
		.buffer = storage->buffer
	};
	return vkGetBufferDeviceAddress(device_, &buffer_address_info);
}

void VkGraphicsBackend::buffer_flush(const RID buffer_rid, const ivec2 range) {
	const VmaAllocation allocation = get_buffer_allocation(buffer_rid);
	vmaFlushAllocation(allocator_, allocation, range.x, range.y == -1 ? VK_WHOLE_SIZE : range.y);
}

RID VkGraphicsBackend::image_create(const ImageDescriptor &desc) {
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
	vmaCreateImage(allocator_, &image_create_info, &allocation_info, &image, &allocation, nullptr);
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_IMAGE,
			.objectHandle = reinterpret_cast<uint64_t>(image),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
		.image = image,
		.allocation = allocation,
		.format = desc.format
	});
	RID rid = _make_rid(ResourceKind::eImage, handle.slot);
	rid.lower = handle.generation;
	++allocations_;
	return rid;
}

void VkGraphicsBackend::image_delete(const RID id) {
	const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	vmaDestroyImage(allocator_, storage->image, storage->allocation);
	assert(images_.erase(id.upper, id.lower));
}

void VkGraphicsBackend::image_set_name(const RID handle, const char *name) {
	const vulkan::ImageStorage storage = get_image_storage(handle);
	vmaSetAllocationName(allocator_, storage.allocation, name);
}

void VkGraphicsBackend::image_load_from_buffer(RID image, RID buffer, const Vec<vk::BufferImageCopy> &copy) {
	// @todo
}

void VkGraphicsBackend::image_load_from_buffer(RID image, RID buffer, const vk::BufferImageCopy &copy) {
	/*
	const vk::Image i = gltf::images[image].image;
	const vk::Buffer b = _buffer_inner(buffer);
	
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
*/
}

vk::Image VkGraphicsBackend::get_image(const RID id) const {
	const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	return storage->image;
}

const vulkan::ImageStorage &VkGraphicsBackend::get_image_storage(const RID id) const {
	const vulkan::ImageStorage *storage = images_.get(id.upper, id.lower);
	return *storage;
}

RID VkGraphicsBackend::image_view_create(const ImageViewDescriptor &desc) {
	const vulkan::ImageStorage *storaggge = images_.get(desc.image.upper, desc.image.lower);
	const vulkan::ImageStorage &storage = get_image_storage(desc.image);
	vk::Image image = storage.image;
	
	// VkImageviewus
	VkImageUsageFlags usageFlags = 0;
	switch (desc.subresource->aspect_mask) {
	case Aspect::eDepth:
		usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		break;
	default: break;
	}
	
	VkImageViewUsageCreateInfo image_view_usage_create_info = {
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
	printf(
		"SUB RESOURCE RANGE FLAGS ASPECT %u\n",
		image_view_create_info.subresourceRange.aspectMask
	);

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
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vk::ImageView>::Handle handle = image_views_.emplace(image_view);
	RID rid = _make_rid(ResourceKind::eImageView, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::image_view_delete(const RID id) {
	const vk::ImageView image_view = get_image_view(id);
	vkDestroyImageView(device_, image_view, nullptr);
	assert(image_views_.erase(id.upper, id.lower));
}

vk::ImageView VkGraphicsBackend::get_image_view(const RID id) const {
	const vk::ImageView* image_view = image_views_.get(id.upper, id.lower);
	return *image_view;
}

RID VkGraphicsBackend::sampler_create(const SamplerDescriptor &desc) {
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
		.compareOp = vk::detail::convert(desc.compare_op.value_or(::CompareOp::eAlways)),
		.minLod = desc.min_lod,
		.maxLod = desc.max_lod,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};

	VkSampler sampler;
	vkResultCheck(vkCreateSampler(device_, &sampler_create_info, nullptr, &sampler), "Failed to create sampler");
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SAMPLER,
			.objectHandle = reinterpret_cast<uint64_t>(sampler),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const auto handle = samplers_.emplace(sampler);
	RID rid = _make_rid(ResourceKind::eSampler, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::sampler_delete(const RID sampler) {
	const vk::Sampler vk_sampler = get_sampler(sampler);
	vkDestroySampler(device_, vk_sampler, nullptr);
	assert(samplers_.erase(sampler.upper, sampler.lower));
}

vk::Sampler VkGraphicsBackend::get_sampler(const RID id) const {
	const vk::Sampler* sampler = samplers_.get(id.upper, id.lower);
	return *sampler;
}

RID VkGraphicsBackend::bind_group_layout_create(const BindGroupLayoutDescriptor &desc) {
	Vec<VkDescriptorSetLayoutBinding> bindings(desc.entries.size());
	for (std::size_t i = 0; i < desc.entries.size(); ++i)
		bindings[i] = vk::detail::convert(desc.entries[i]);

	const VkDescriptorSetLayoutCreateInfo create_info = VkDescriptorSetLayoutCreateInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	VkDescriptorSetLayout layout;
	vkResultCheck(vkCreateDescriptorSetLayout(device_, &create_info, nullptr, &layout), "Failed to create descriptor set layout");
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
			.objectHandle = reinterpret_cast<uint64_t>(layout),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vk::DescriptorSetLayout>::Handle handle = descriptor_set_layouts_.emplace(layout);
	RID rid = _make_rid(ResourceKind::eBindGroupLayout, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::bind_group_layout_delete(const RID id) {
	const vk::DescriptorSetLayout layout = get_bind_group_layout(id);
	vkDestroyDescriptorSetLayout(device_, layout, nullptr);
	assert(descriptor_set_layouts_.erase(id.upper, id.lower));
}

vk::DescriptorSetLayout VkGraphicsBackend::get_bind_group_layout(const RID id) const {
	const vk::DescriptorSetLayout* layout = descriptor_set_layouts_.get(id.upper, id.lower);
	return *layout;
}

RID VkGraphicsBackend::bind_group_create(const BindGroupDescriptor &desc) {
	VkDescriptorSetLayout layout = get_bind_group_layout(desc.layout);
	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptor_pool_,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout
	};
	
	VkDescriptorSet descriptor_set;
	vkResultCheck(vkAllocateDescriptorSets(device_, &descriptor_set_allocate_info, &descriptor_set), "Failed to allocate descriptor set");
	
#ifdef _DEBUG
	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET,
			.objectHandle = reinterpret_cast<uint64_t>(descriptor_set),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vk::DescriptorSet>::Handle handle = descriptor_sets_.emplace(descriptor_set);
	RID bind_group = _make_rid(ResourceKind::eBindGroup, handle.slot);
	bind_group.lower = handle.generation;
	++allocations_;
	
	// If entries are specified, then we can write to the Descriptor Set
	if (desc.entries.empty())
		return bind_group;
	
	bind_group_update(bind_group, desc.entries);
	
	return bind_group;
}

void VkGraphicsBackend::bind_group_delete(const RID id) {
	const VkDescriptorSet descriptor_set = get_bind_group(id);
	vkResultCheck(vkFreeDescriptorSets(device_, descriptor_pool_, 1, &descriptor_set), "Failed to free descriptor set");
	assert(descriptor_sets_.erase(id.upper, id.lower));
}

void VkGraphicsBackend::bind_group_update(const RID bind_group_rid, const Vec<BindGroupEntryDescriptor> &entries) {
	const VkDescriptorSet descriptor_set = get_bind_group(bind_group_rid);
	
	Vec<VkDescriptorImageInfo> image_infos;
	Vec<VkDescriptorBufferInfo> buffer_infos;
	Vec<VkWriteDescriptorSet> write_descriptor_sets;//(entries.size());
	
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
			buffer_infos.push_back(buffer_info);
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = nullptr,
				.pBufferInfo = &buffer_infos.back(),
				.pTexelBufferView = nullptr
			};
			write_descriptor_sets.push_back(write_descriptor_set);
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
			image_infos.push_back(sampler_info);
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos.back(),
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			
			write_descriptor_sets.push_back(write_descriptor_set);
			
			break;
		}
		case BindingType::eSampledImage:
		case BindingType::eStorageImage: {
			auto image_binding = std::get<BindingResource::ImageBinding>(entry.resource.binding);
			VkDescriptorImageInfo image_info = {
				.imageView = get_image_view(image_binding.image_view),
				.imageLayout = vk::detail::convert(image_binding.layout)
			};
			image_infos.push_back(image_info);
			
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos.back(),
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			write_descriptor_sets.push_back(write_descriptor_set);
			
			break;
		}
		case BindingType::eImageSampler:
		auto combined_binding = std::get<BindingResource::CombinedImageSampler>(entry.resource.binding);
			VkDescriptorImageInfo combined_info = {
				.sampler = get_sampler(combined_binding.sampler),
				.imageView = get_image_view(combined_binding.image_view),
				.imageLayout = vk::detail::convert(combined_binding.layout)
			};
			image_infos.push_back(combined_info);
			VkWriteDescriptorSet write_descriptor_set = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptor_set,
				.dstBinding = entry.binding,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::detail::convert(entry.resource.type),
				.pImageInfo = &image_infos.back(),
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};
			write_descriptor_sets.push_back(write_descriptor_set);
			break;
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
 */
void VkGraphicsBackend::set_bind_group(const RID command_rid, const RID pipeline_layout_rid, u32 index,
                                       const RID bind_group_rid) {
	
	VkDescriptorSet descriptor_set = get_bind_group(bind_group_rid);
	
	VkBindDescriptorSetsInfo bind_descriptor_sets_info{
		.sType = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
		.pNext = nullptr,
		.stageFlags = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
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

RID VkGraphicsBackend::shader_create(const SpirvDescriptor &spirv_descriptor) {
	const VkShaderModuleCreateInfo shader_module_create_info = VkShaderModuleCreateInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = spirv_descriptor.code_size,
		.pCode = spirv_descriptor.code
	};

	VkShaderModule shader_module;
	vkResultCheck(vkCreateShaderModule(device_, &shader_module_create_info, nullptr, &shader_module), "Failed to create shader module");
	
#ifdef _DEBUG
	if (spirv_descriptor.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SHADER_MODULE,
			.objectHandle = reinterpret_cast<uint64_t>(shader_module),
			.pObjectName = spirv_descriptor.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vk::ShaderModule>::Handle handle = shader_modules_.emplace(shader_module);
	RID rid = _make_rid(ResourceKind::eShaderModule, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::shader_delete(const RID id) {
	const vk::ShaderModule shader_module = get_shader_module(id);
	vkDestroyShaderModule(device_, shader_module, nullptr);
	assert(shader_modules_.erase(id.upper, id.lower));
}

vk::ShaderModule VkGraphicsBackend::get_shader_module(const RID id) const {
	const vk::ShaderModule* shader_module = shader_modules_.get(id.upper, id.lower);
	return *shader_module;
}

namespace detail {
	static gfx::Format getFormatFromColorSpace(const Vec<vk::SurfaceFormatKHR> &surface_formats, vk::ColorSpaceKHR const &colorSpace) {
		assert(!surface_formats.empty() && "Surface formats must not be empty");
		for (vk::SurfaceFormatKHR const &surface_format : surface_formats)
			if (surface_format.colorSpace == colorSpace)
				return vk::detail::convert(static_cast<VkFormat>(surface_format.format));
	
		return vk::detail::convert(static_cast<VkFormat>(surface_formats[0].format));
	}

	static vk::ColorSpaceKHR getColorSpaceFromFormat(const Vec<vk::SurfaceFormatKHR> &surface_formats, vk::Format const &format) {
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

RID VkGraphicsBackend::surface_create(Window *window, const SurfaceDescriptor &desc) {
	// Step 1: make surface khr
	GLFWwindow* glfw_window = window->window;
	
	VkSurfaceKHR surface;
	vkResultCheck(
		glfwCreateWindowSurface(instance_, glfw_window, nullptr, &surface),
		"Failed to create window surface"
	);
	
#ifdef _DEBUG

	if (desc.label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SURFACE_KHR,
			.objectHandle = reinterpret_cast<uint64_t>(surface),
			.pObjectName = desc.label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
	
#endif
	
	assert(surface != VK_NULL_HANDLE && "Failed to create window surface");

	const ivec2 size = window->getSize();
	VkExtent2D image_extent = {
		.width = static_cast<uint32_t>(size.x),
		.height = static_cast<uint32_t>(size.y)
	};

	const vk::CommandBufferAllocateInfo command_buffer_allocate_info = vk::CommandBufferAllocateInfo()
		.setCommandPool(command_pool_)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount(2);
	
	Vec<vk::CommandBuffer> allocation = device_.allocateCommandBuffers(command_buffer_allocate_info);
	Array<RID, vulkan::framesInFlight> command_buffers;
	Array<RID, vulkan::framesInFlight> render_finished_semaphores;
	Array<RID, vulkan::framesInFlight> image_available_semaphores;
	Array<RID, vulkan::framesInFlight> graphics_fences;
	
	for (size_t i = 0; i < allocation.size(); ++i) {
		const SlotPool<vk::CommandBuffer>::Handle command_buffer_handle = command_buffers_.emplace(allocation[i]);
		RID command_buffer_rid = _make_rid(ResourceKind::eCommandBuffer, command_buffer_handle.slot);
		command_buffer_rid.lower = command_buffer_handle.generation;
		command_buffers[i] = command_buffer_rid;
		render_finished_semaphores[i] = semaphore_create(SemaphoreType::eBinary, "Render Finished Semaphore" + std::to_string(i));
		image_available_semaphores[i] = semaphore_create(SemaphoreType::eBinary, "Image Available Semaphore" + std::to_string(i));
		graphics_fences[i] = fence_create("Graphics Fence" + std::to_string(i), true);
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

Vec<gfx::Format> VkGraphicsBackend::surface_get_formats(const RID surface_rid) {
	return {}; // TODO
}

gfx::Format VkGraphicsBackend::surface_get_color_format(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.color_format;
}

RID VkGraphicsBackend::surface_get_active_image(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_images[storage.image_index];
}

RID VkGraphicsBackend::surface_get_active_image_view(const RID surface_rid) {
	const vulkan::SurfaceStorage& storage = get_surface_storage(surface_rid);
	return storage.swapchain_image_views[storage.image_index];
}

void VkGraphicsBackend::surface_delete(const RID surface_rid) {
	vulkan::SurfaceStorage& storage = get_surface_storage_mut(surface_rid);

	if (storage.swapchain != VK_NULL_HANDLE) {
		for (const RID image_view_rid : storage.swapchain_image_views)
			image_view_delete(image_view_rid);
		device_.destroySwapchainKHR(storage.swapchain);
	}
	instance_.destroySurfaceKHR(storage.surface);
	assert(surfaces_.erase(surface_rid.upper, surface_rid.lower));
}

void VkGraphicsBackend::update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) {
	vulkan::SurfaceStorage& storage = get_surface_storage_mut(surface_rid);
	vk::SurfaceKHR surface_khr = storage.surface;

	std::vector<vk::SurfaceFormatKHR> surface_formats = adapter_.getSurfaceFormatsKHR(surface_khr);
	
	const ivec2 size = storage.window->getSize();
	VkExtent2D image_extent = {
		.width = static_cast<uint32_t>(size.x),
		.height = static_cast<uint32_t>(size.y)
	};
	
	vk::Format color_format;
	vk::ColorSpaceKHR color_space;
	vk::PresentModeKHR present_mode;
	
	auto target_present_mode = desc.present_method;
	auto target_color_format = desc.format;
	auto target_color_space = desc.color_space;
	
	const vk::SurfaceCapabilitiesKHR surface_capabilities = adapter_.getSurfaceCapabilitiesKHR(surface_khr);
	storage.extent = vk::Extent2D(surface_capabilities.currentExtent.width,
		surface_capabilities.currentExtent.height);
	
	if (!target_present_mode.has_value())
		target_present_mode = PresentMethod::eFifo;
	
	// Choose a presentation mode.
	i32 current_presentation_mode_score = 20;
	if (!target_present_mode.has_value()) {
		const std::vector<vk::PresentModeKHR> present_modes = adapter_.getSurfacePresentModesKHR(surface_khr);
		present_mode = vk::PresentModeKHR::eFifo;

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
		.imageColorSpace = vk::detail::convert(target_color_space.value_or(gfx::ColorSpace::eSrgbNonLinear)),
		.imageExtent = image_extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::detail::convert(desc.usage.value_or(gfx::ImageUsage::eColorAttachment)),
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = vk::detail::convert(desc.composite_alpha.value_or(CompositeAlpha::eOpaque)),
		.presentMode = vk::detail::convert(desc.present_method.value_or(PresentMethod::eFifo)),
		.oldSwapchain = storage.swapchain
	};
	storage.color_format = vk::detail::convert(static_cast<VkFormat>(color_format));
	if (storage.swapchain != VK_NULL_HANDLE) {
		for (auto i = 0; i < vulkan::framesInFlight; ++i) {
			semaphore_delete(storage.render_finished_semaphores[i]);
			semaphore_delete(storage.image_available_semaphores[i]);
			fence_delete(storage.graphics_fences[i]);
		}
		for (const RID image_view_rid : storage.swapchain_image_views)
			image_view_delete(image_view_rid);
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
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif
	
	const Vec<vk::Image> swapchain_images = device_.getSwapchainImagesKHR(swapchain);
	Vec<RID> images; // (swapchain_images.size());
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
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
		
		const SlotPool<vulkan::ImageStorage>::Handle handle = images_.emplace(vulkan::ImageStorage{
			.image = image,
			.allocation = VK_NULL_HANDLE,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb)
		});
		RID rid = _make_rid(ResourceKind::eImage, handle.slot);
		rid.lower = handle.generation;
		images.push_back(rid);
	}
	
	Vec<RID> views;//(images.size());
	for (const RID image_rid : images) {
		ImageViewDescriptor imageViewDescriptor{
			.image = image_rid,
			.type = ImageViewType::e2D,
			.format = desc.format.value_or(gfx::Format::eRgba8Srgb),
			.subresource = ImageSubresourceDescriptor{
				.aspect_mask = Aspect::eColor,
				.base_mip_level = 0,
				.level_count = 1,
				.base_array_layer = 0,
				.layer_count = 1
			}
		};
		views.push_back(image_view_create(imageViewDescriptor));
	}
	
	storage.swapchain = swapchain;
	storage.swapchain_images = images;
	storage.swapchain_image_views = views;
	storage.color_format = target_color_format.value_or(gfx::Format::eRgba8Srgb);
}

const vulkan::SurfaceStorage & VkGraphicsBackend::get_surface_storage(const RID id) const {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id.upper, id.lower);
	return *storage;
}

vulkan::SurfaceStorage & VkGraphicsBackend::get_surface_storage_mut(const RID id) {
	const vulkan::SurfaceStorage *storage = surfaces_.get(id.upper, id.lower);
	return *const_cast<vulkan::SurfaceStorage*>(storage);
}

RID VkGraphicsBackend::pipeline_layout_create(const PipelineLayoutDescriptor &desc) {
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = static_cast<uint32_t>(desc.bind_group_layouts.size()),
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = static_cast<uint32_t>(desc.push_constants.size()),
		.pPushConstantRanges = nullptr
	};
	
	Vec<VkDescriptorSetLayout> descriptor_set_layouts(desc.bind_group_layouts.size());
	for (const RID bind_group_layout_rid : desc.bind_group_layouts) {
		const vk::DescriptorSetLayout layout = get_bind_group_layout(bind_group_layout_rid);
		descriptor_set_layouts.push_back(layout);
	}
	pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts.data();
	
	Vec<VkPushConstantRange> push_constant_ranges;//(desc.push_constants.size());
	for (const PushConstantRangeDescriptor &push_constant_range : desc.push_constants) {
		VkPushConstantRange range = {
			.stageFlags = vk::detail::convert(push_constant_range.visibility),
			.offset = push_constant_range.offset,
			.size = push_constant_range.size
		};
		push_constant_ranges.push_back(range);
	}
	pipeline_layout_create_info.pPushConstantRanges = push_constant_ranges.data();
	
	VkPipelineLayout pipeline_layout;
	vkCreatePipelineLayout(device_, &pipeline_layout_create_info, nullptr, &pipeline_layout);
	const SlotPool<vk::PipelineLayout>::Handle handle = pipeline_layouts_.emplace(pipeline_layout);
	RID rid = _make_rid(ResourceKind::ePipelineLayout, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::pipeline_layout_delete(const RID pipeline_layout_rid) {
	const vk::PipelineLayout pipeline_layout = get_pipeline_layout(pipeline_layout_rid);
	vkDestroyPipelineLayout(device_, pipeline_layout, nullptr);
	assert(pipeline_layouts_.erase(pipeline_layout_rid.upper, pipeline_layout_rid.lower));
}

vk::PipelineLayout VkGraphicsBackend::get_pipeline_layout(const RID rid) {
	const vk::PipelineLayout* pipeline_layout = pipeline_layouts_.get(rid.upper, rid.lower);
	return *pipeline_layout;
}

RID VkGraphicsBackend::pipeline_create(const GraphicsPipelineDescriptor &desc) {
	
	Vec<VkPipelineShaderStageCreateInfo> stages; // `(desc.stages.size());
	for (const auto& stage_desc : desc.stages) {
		VkPipelineShaderStageCreateInfo stage_create_info = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = vk::detail::convert2(stage_desc.stage),
			.module = get_shader_module(stage_desc.shader),
			.pName = stage_desc.entry_point.c_str()
		};
		stages.push_back(stage_create_info);
	}
	
	Vec<VkVertexInputBindingDescription> binding_descriptions; // (desc.vertex_input.bindings.size());
	for (const auto& binding_desc : desc.vertex_input.bindings) {
		VkVertexInputBindingDescription binding_description = {
			.binding = binding_desc.binding,
			.stride = binding_desc.stride,
			.inputRate = vk::detail::convert(binding_desc.input_rate)
		};
		binding_descriptions.push_back(binding_description);
	}
	
	Vec<VkVertexInputAttributeDescription> attribute_descriptions; // (desc.vertex_input.attributes.size());
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
	
	Vec<VkViewport> viewports(desc.viewport.viewports.size());
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
	
	Vec<VkRect2D> scissors(desc.viewport.scissors.size());
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
		.pSampleMask = nullptr, //< Not implemented
		.alphaToCoverageEnable = VK_FALSE, //< Not implemented
		.alphaToOneEnable = VK_FALSE //< Not implemented
	};
	
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = desc.depth_stencil.depth_test_enable ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = desc.depth_stencil.depth_write_enable ? VK_TRUE : VK_FALSE,
		.depthCompareOp = vk::detail::convert(desc.depth_stencil.depth_compare_op),
		.depthBoundsTestEnable = desc.depth_stencil.depth_bounds_test_enable ? VK_TRUE : VK_FALSE,
		.stencilTestEnable = desc.depth_stencil.stencil_test_enable ? VK_TRUE : VK_FALSE,
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
	
	Vec<VkDynamicState> dynamic_states;//(desc.dynamic_states.size());
	for (const DynamicState &state : desc.dynamic_states) {
		dynamic_states.push_back(vk::detail::convert(state));
	}
	
	VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
		.pDynamicStates = dynamic_states.data()
	};
	
	Vec<VkFormat> color_attachment_formats;//(desc.rendering.color_formats.size());
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
	vkResultCheck(result, "Failed to create graphics pipeline");
	
	const SlotPool<vk::Pipeline>::Handle handle = pipelines_.emplace(pipeline);
	RID rid = _make_rid(ResourceKind::ePipeline, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::pipeline_bind(const RID pipeline, const RID cmd_rid, const gfx::PipelineBindPoint bind_point) {
	const VkPipeline vk_pipeline = get_pipeline(pipeline);
	const VkCommandBuffer command_buffer = get_command_buffer(cmd_rid);
	vkCmdBindPipeline(command_buffer, vk::detail::convert(bind_point), vk_pipeline);
}

void VkGraphicsBackend::pipeline_delete(const RID pipeline_rid) {
	const vk::Pipeline pipeline = get_pipeline(pipeline_rid);
	vkDestroyPipeline(device_, pipeline, nullptr);
	assert(pipelines_.erase(pipeline_rid.upper, pipeline_rid.lower));
}

vk::Pipeline VkGraphicsBackend::get_pipeline(const RID id) const {
	const vk::Pipeline* pipeline = pipelines_.get(id.upper, id.lower);
	return *pipeline;
}

RID VkGraphicsBackend::fence_create(const Optional<String> &label, const bool signaled) {
	const VkFenceCreateInfo fence_create_info = VkFenceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
	};
	
	VkFence fence;
	vkResultCheck(vkCreateFence(device_, &fence_create_info, nullptr, &fence), "Failed to create fence");
	
#ifdef _DEBUG
	if (label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_FENCE,
			.objectHandle = reinterpret_cast<uint64_t>(fence),
			.pObjectName = label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif
	
	const SlotPool<vk::Fence>::Handle handle = fences_.emplace(fence);
	RID rid = _make_rid(ResourceKind::eFence, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::fence_delete(const RID fence_rid) {
	const vk::Fence fence = get_fence(fence_rid);
	vkDestroyFence(device_, fence, nullptr);
	assert(fences_.erase(fence_rid.upper, fence_rid.lower));
}

vk::Fence VkGraphicsBackend::get_fence(const RID id) const {
	const vk::Fence* fence = fences_.get(id.upper, id.lower);
	return *fence;
}

RID VkGraphicsBackend::semaphore_create(const gfx::SemaphoreType semaphore_type, const Optional<String> &label) {
	const VkSemaphoreTypeCreateInfo semaphore_type_create_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
		.pNext = nullptr,
		.semaphoreType = semaphore_type == gfx::SemaphoreType::eBinary ? VK_SEMAPHORE_TYPE_BINARY : VK_SEMAPHORE_TYPE_TIMELINE,
		.initialValue = 0u
	};
	
	const VkSemaphoreCreateInfo semaphore_create_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = &semaphore_type_create_info,
	};

	VkSemaphore semaphore;
	vkResultCheck(vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &semaphore), "Failed to create semaphore");

#ifdef _DEBUG
	if (label.has_value()) {
		const VkDebugUtilsObjectNameInfoEXT name_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = VK_OBJECT_TYPE_SEMAPHORE,
			.objectHandle = reinterpret_cast<uint64_t>(semaphore),
			.pObjectName = label.value().c_str()
		};
		vkSetDebugUtilsObjectNameEXT(device_, &name_info);
	}
#endif

	const SlotPool<vk::Semaphore>::Handle handle = semaphores_.emplace(semaphore);
	RID rid = _make_rid(ResourceKind::eSemaphore, handle.slot);
	rid.lower = handle.generation;
	return rid;
}

void VkGraphicsBackend::semaphore_delete(const RID semaphore_rid) {
	const vk::Semaphore semaphore = get_semaphore(semaphore_rid);
	vkDestroySemaphore(device_, semaphore, nullptr);
	assert(semaphores_.erase(semaphore_rid.upper, semaphore_rid.lower));
}

vk::Semaphore VkGraphicsBackend::get_semaphore(const RID id) const {
	const vk::Semaphore* semaphore = semaphores_.get(id.upper, id.lower);
	return *semaphore;
}

RID VkGraphicsBackend::begin_recording(const RID surface_rid) {
	using namespace vk::detail;
	
	vulkan::SurfaceStorage& surface_storage = get_surface_storage_mut(surface_rid);
	const VkCommandBuffer command = get_command_buffer(surface_storage.graphics_command_buffers[surface_storage.frame_index]);
	const VkFence fence = get_fence(surface_storage.graphics_fences[surface_storage.frame_index]);
	const VkSwapchainKHR swapchain = surface_storage.swapchain;
	constexpr VkCommandBufferBeginInfo begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};
	
	vkResultCheck(vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
	vkResultCheck(vkResetFences(device_, 1, &fence), "Failed to reset fence");
	
	const VkSemaphore image_available_semaphore = get_semaphore(surface_storage.image_available_semaphores[surface_storage.frame_index]);
	vkResultCheck(vkAcquireNextImageKHR(device_, swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &surface_storage.image_index), "Failed to acquire next swapchain image!");
	
	vkResultCheck(
		vkResetCommandBuffer(command, 0),
		"Failed to reset command buffer"
	);
	
	vkResultCheck(
		vkBeginCommandBuffer(command, &begin_info), 
		"Failed to begin command buffer recording"
	);
	
	return surface_storage.graphics_command_buffers[surface_storage.frame_index];
}

uint32_t VkGraphicsBackend::begin_rendering(const RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) {
	using namespace vk::detail;
	vulkan::SurfaceStorage& surface_storage = get_surface_storage_mut(surface_rid);
	const VkCommandBuffer command = get_command_buffer(command_rid);
	const VkPipeline pipeline = get_pipeline(pipeline_rid);
	
	const VkRenderingAttachmentInfo color_attachment_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = get_image_view(surface_storage.swapchain_image_views[surface_storage.image_index]),
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
	
	const VkRenderingAttachmentInfo depth_attachment_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = get_image_view(depth_image_view),
		.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.clearValue = { .depthStencil = {.depth = 1.0f, .stencil = 0 } }
	};

	const VkRenderingInfo rendering_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderArea = { 
			.offset = { .x = 0, .y = 0 },
			.extent = {
				.width = static_cast<u32>(surface_storage.window->getSize().x),
				.height = static_cast<u32>(surface_storage.window->getSize().y)
			}
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_info,
		.pDepthAttachment = &depth_attachment_info,
		.pStencilAttachment = nullptr
	};
	
	vkCmdBeginRendering(command, &rendering_info);
	vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	
	VkViewport window_viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(surface_storage.window->getSize().x),
		.height = static_cast<float>(surface_storage.window->getSize().y),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	
	vkCmdSetViewport(command, 0, 1, &window_viewport);
	
	VkRect2D window_scissor{
		.offset = { .x = 0, .y = 0 },
		.extent = {
			.width = static_cast<u32>(surface_storage.window->getSize().x),
			.height = static_cast<u32>(surface_storage.window->getSize().y)
		}
	};
	vkCmdSetScissor(command, 0, 1, &window_scissor);
	return surface_storage.image_index;
}

void VkGraphicsBackend::finish_rendering(const RID command_rid) const {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkCmdEndRendering(command);
}

void VkGraphicsBackend::finish_recording(const RID command_rid) const {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	vkResultCheck(vkEndCommandBuffer(command), "Failed to end command buffer recording");
}

void VkGraphicsBackend::transition(const RID command_rid, const ImageTransitionDescriptor &descriptor) {
	transition(command_rid, { descriptor });
}

void VkGraphicsBackend::transition(const RID command_rid, const Vec<ImageTransitionDescriptor> &descriptors) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	Vec<VkImageMemoryBarrier2> barriers;
	for (const ImageTransitionDescriptor &descriptor : descriptors) {
		const VkImage image = get_image(descriptor.image);
		VkImageMemoryBarrier2 barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = vk::detail::convert(descriptor.src.stage),
			.srcAccessMask = vk::detail::convert(descriptor.src.access),
			.dstStageMask = vk::detail::convert(descriptor.dst.stage),
			.dstAccessMask = vk::detail::convert(descriptor.dst.access),
			.oldLayout = vk::detail::convert(descriptor.src.layout),
			.newLayout = vk::detail::convert(descriptor.dst.layout),
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
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
}

void VkGraphicsBackend::command_submit(const RID surface_rid, const RID command_rid) {
	const vulkan::SurfaceStorage& surface_storage = get_surface_storage(surface_rid);
	const VkCommandBuffer command = get_command_buffer(command_rid);
	const VkSemaphore image_available_semaphore = get_semaphore(surface_storage.image_available_semaphores[surface_storage.frame_index]);
	const VkSemaphore render_finished_semaphore = get_semaphore(surface_storage.render_finished_semaphores[surface_storage.frame_index]);
	
	const VkSemaphoreSubmitInfo wait_semaphore_submit_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = image_available_semaphore,
		.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	
	VkCommandBufferSubmitInfo command_buffer_submit_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = command
	};
	
	const VkSemaphoreSubmitInfo signal_semaphore_submit_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = render_finished_semaphore,
		.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
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

	vkResultCheck(
		vkQueueSubmit2(graphics_queue_[0], 1, &submit_info, get_fence(surface_storage.graphics_fences[surface_storage.frame_index])),
		"Failed to submit command buffer"
	);
}

void VkGraphicsBackend::present(const RID surface_rid) {
	vulkan::SurfaceStorage& surface_storage = get_surface_storage_mut(surface_rid);
	const VkSemaphore render_finished_semaphore = get_semaphore(surface_storage.render_finished_semaphores[surface_storage.frame_index]);
	
	VkSwapchainKHR swapchain = surface_storage.swapchain;

	const VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &render_finished_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &surface_storage.image_index
	};
	vkResultCheck(
		vkQueuePresentKHR(graphics_queue_[0], &present_info),
		"Failed to present swapchain image"
	);
	
	surface_storage.frame_index = (surface_storage.frame_index + 1) % vulkan::framesInFlight;
}

void VkGraphicsBackend::push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor &descriptor, const void *data) {
	vkCmdPushConstants(get_command_buffer(command_rid), get_pipeline_layout(pipeline_layout_rid), 
		vk::detail::convert(BitFlag(descriptor.visibility)), descriptor.offset, descriptor.size, data);
}

vk::CommandBuffer VkGraphicsBackend::get_command_buffer(const RID id) const {
	const vk::CommandBuffer* command_buffer = command_buffers_.get(id.upper, id.lower);
	return *command_buffer;
}

void VkGraphicsBackend::bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) {
	bind_vertex_buffers(command_rid, { desc });
}

void VkGraphicsBackend::bind_vertex_buffers(const RID command_rid, const Vec<VertexBufferDescriptor> &desc) {
	const VkCommandBuffer command = get_command_buffer(command_rid);
	
	Vec<VkBuffer> buffers;//(desc.size());
	Vec<VkDeviceSize> offsets;//(desc.size());
	for (const VertexBufferDescriptor &buffer_desc : desc) {
		buffers.push_back(get_buffer(buffer_desc.buffer));
		offsets.push_back(buffer_desc.offset);
	}
	vkCmdBindVertexBuffers(command, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VkGraphicsBackend::bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) {
	VkCommandBuffer command = get_command_buffer(command_rid);
	vkCmdBindIndexBuffer(command, get_buffer(desc.buffer), desc.offset, vk::detail::convert(desc.index_type));
}

void VkGraphicsBackend::draw_indexed(const RID command_rid, const std::uint32_t first_index, const std::uint32_t index_count) {
	draw_indexed_instanced(command_rid, index_count, 1, first_index, 0, 0);
}

void VkGraphicsBackend::draw_indexed_instanced(const RID command_rid, const std::uint32_t index_count, const std::uint32_t instance_count,
                                               const std::uint32_t first_index, const std::int32_t vertex_offset, const std::uint32_t first_instance) {
	const VkCommandBuffer commandBuffer = get_command_buffer(command_rid);
	vkCmdDrawIndexed(commandBuffer, index_count, instance_count,
		first_index, vertex_offset, first_instance);
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

void VkGraphicsBackend::force_wait_for_device_idle() {
	vkDeviceWaitIdle(device_);
}

void VkGraphicsBackend::create_instance() {
	uint32_t extension_count = 0;
	const char** required_instance_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

	Vec<const char*> extensions(extension_count);
	for (uint32_t u = 0u; u < extension_count; u++)
		extensions[u] = required_instance_extensions[u];
	
#ifdef _DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	Vec enables ={
		VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		// VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
		// VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
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
	Vec<const char*> layers = {
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
#ifdef _DEBUG
		.setPNext(&validationFeaturesExt)
#endif
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
	
#ifdef _DEBUG
	
	vkCreateDebugUtilsMessengerEXT 	= (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
	vkCmdBeginDebugUtilsLabelEXT 	= (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdBeginDebugUtilsLabelEXT");
	vkCmdInsertDebugUtilsLabelEXT 	= (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdInsertDebugUtilsLabelEXT");
	vkCmdEndDebugUtilsLabelEXT 		= (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkCmdEndDebugUtilsLabelEXT");
	vkQueueBeginDebugUtilsLabelEXT 	= (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueBeginDebugUtilsLabelEXT");
	vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueInsertDebugUtilsLabelEXT");
	vkQueueEndDebugUtilsLabelEXT 	= (PFN_vkQueueEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance_, "vkQueueEndDebugUtilsLabelEXT");
	vkSetDebugUtilsObjectNameEXT 	= (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance_, "vkSetDebugUtilsObjectNameEXT");
	
	VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info{
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
	vkResultCheck(vkCreateDebugUtilsMessengerEXT(
		instance_, &debug_utils_messenger_create_info,
		nullptr, &debug_messenger_
	), "Failed to create debug utils messenger");
	
#endif
}

void VkGraphicsBackend::request_adapter() {
	const Vec<vk::PhysicalDevice> physical_devices = instance_.enumeratePhysicalDevices();
	
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
			graphics_queue_index_ = queue_index;
			graphics_queue_.resize(1);
			continue;
		}
		
		if (!found_compute_queue &&
		    properties.queueFlags & vk::QueueFlagBits::eCompute) {
			found_compute_queue = true;
			compute_queue_index_ = queue_index;
			compute_queue_.resize(1);
			continue;
		}
		
		auto dedicated_transfer_queue_flags = vk::QueueFlagBits::eTransfer & ~vk::QueueFlagBits::eGraphics & ~vk::QueueFlagBits::eCompute;
		if (!found_transfer_queue &&
		    properties.queueFlags & dedicated_transfer_queue_flags ) {
			found_transfer_queue = true;
			transfer_queue_index_ = queue_index;
			transfer_queue_.resize(1);
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
	   .setQueueFamilyIndex(graphics_queue_index_)
	   .setQueueCount(static_cast<uint32_t>(graphics_queue_.size()))
	   .setQueuePriorities({ queue_priority }),
		vk::DeviceQueueCreateInfo()
	   .setQueueFamilyIndex(compute_queue_index_)
	   .setQueueCount(static_cast<uint32_t>(compute_queue_.size()))
	   .setQueuePriorities({ queue_priority }),
		vk::DeviceQueueCreateInfo()
	   .setQueueFamilyIndex(transfer_queue_index_)
	   .setQueueCount(static_cast<uint32_t>(transfer_queue_.size()))
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
	
	device_ = adapter_.createDevice(device_create_info);
	
	for (std::uint32_t index = 0; index < graphics_queue_.size(); ++index)
		graphics_queue_[index] = device_.getQueue(graphics_queue_index_, index);
	
	for (std::uint32_t index = 0; index < compute_queue_.size(); ++index)
		compute_queue_[index] = device_.getQueue(compute_queue_index_, index);
	
	for (std::uint32_t index = 0; index < transfer_queue_.size(); ++index)
		transfer_queue_[index] = device_.getQueue(transfer_queue_index_, index);
	
#ifdef _DEBUG
	
	VkQueue gq = graphics_queue_[0];
	VkQueue cq = compute_queue_[0];
	VkQueue tq = transfer_queue_[0];

	VkDebugUtilsObjectNameInfoEXT graphics_queue_name{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.pNext = nullptr,
		.objectType = VK_OBJECT_TYPE_QUEUE,
		.objectHandle = reinterpret_cast<uint64_t>(gq),
		.pObjectName = "Graphics Queue"
	};
	VkDebugUtilsObjectNameInfoEXT compute_queue_name{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.pNext = nullptr,
		.objectType = VK_OBJECT_TYPE_QUEUE,
		.objectHandle = reinterpret_cast<uint64_t>(cq),
		.pObjectName = "Compute Queue"
	};
	VkDebugUtilsObjectNameInfoEXT transfer_queue_name{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.pNext = nullptr,
		.objectType = VK_OBJECT_TYPE_QUEUE,
		.objectHandle = reinterpret_cast<uint64_t>(tq),
		.pObjectName = "Transfer Queue"
	};
	
	vkSetDebugUtilsObjectNameEXT(device_, &graphics_queue_name);
	vkSetDebugUtilsObjectNameEXT(device_, &compute_queue_name);
	vkSetDebugUtilsObjectNameEXT(device_, &transfer_queue_name);
	
#endif
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
	vkResultCheck(result, "Failed to create Vulkan Memory Allocator");
}

void VkGraphicsBackend::create_default_pools() {
	std::vector descriptor_pool_sizes = {
		vk::DescriptorPoolSize()
	   .setType(vk::DescriptorType::eUniformBuffer)
	   .setDescriptorCount(2)
	};

	const vk::DescriptorPoolCreateInfo descriptor_pool_create_info = vk::DescriptorPoolCreateInfo()
	                                                                .setPoolSizes(descriptor_pool_sizes)
	                                                                .setMaxSets(2);
	
	descriptor_pool_ = device_.createDescriptorPool(descriptor_pool_create_info);

	using enum vk::CommandPoolCreateFlagBits;
	const vk::CommandPoolCreateInfo transfer_command_pool_create_info = vk::CommandPoolCreateInfo()
	                                                                   .setFlags(eTransient)
	                                                                   .setQueueFamilyIndex(transfer_queue_index_);
	
	transfer_command_pool_ = device_.createCommandPool(transfer_command_pool_create_info);

	VkCommandPoolCreateInfo graphics_command_pool_create_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = graphics_queue_index_
	};
	
	vkResultCheck(vkCreateCommandPool(device_, &graphics_command_pool_create_info, nullptr, (VkCommandPool*)&command_pool_), "Failed to create graphics command pool");
}

void VkGraphicsBackend::dispose() {
	shutdown();
}

bool VkGraphicsBackend::disposed() const {
	return instance_ == VK_NULL_HANDLE && device_ == VK_NULL_HANDLE && allocator_ == VK_NULL_HANDLE;
}
