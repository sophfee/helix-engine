#pragma once

#include "engine/flags.hpp"
#include "gpu/driver.hpp"

namespace vk::detail {
	constexpr [[nodiscard]] VkAccessFlags2 convert(const BitFlag<gfx::Access> access);
	constexpr [[nodiscard]] VkSamplerAddressMode convert(const gfx::AddressMode address_mode);
	constexpr [[nodiscard]] VmaAllocationCreateFlags convert(const BitFlag<gfx::AllocationHint> hints);
	constexpr [[nodiscard]] VkImageAspectFlags convert(const BitFlag<gfx::Aspect> aspect);
	constexpr [[nodiscard]] VkDescriptorType convert(gfx::BindingType type);
	constexpr [[nodiscard]] VkDynamicState convert(gfx::DynamicState type);
	constexpr [[nodiscard]] VkBufferUsageFlags convert(const BitFlag<gfx::BufferUsage> buffer_usage);
	constexpr [[nodiscard]] VmaMemoryUsage convert(const gfx::MemoryUsage memory_usage);
	constexpr [[nodiscard]] VkImageUsageFlags convert(const gfx::ImageUsage image_flags);
	constexpr [[nodiscard]] VkImageType convert(const gfx::ImageType image_type);
	constexpr [[nodiscard]] VkImageViewType convert(const gfx::ImageViewType image_type);
	constexpr [[nodiscard]] VkFormat convert(const gfx::Format format);
	constexpr [[nodiscard]] VkFilter convert(const gfx::Filter filter);
	constexpr [[nodiscard]] VkCompareOp convert(const gfx::CompareOp compare_op);
	constexpr [[nodiscard]] VkColorSpaceKHR convert(const gfx::ColorSpace color_space);
	constexpr [[nodiscard]] VkCompositeAlphaFlagBitsKHR convert(const gfx::CompositeAlpha composite_alpha);
	constexpr [[nodiscard]] VkAttachmentLoadOp convert(const gfx::LoadOp load_op);
	constexpr [[nodiscard]] VkAttachmentStoreOp convert(const gfx::StoreOp store_op);
	constexpr [[nodiscard]] VkSamplerMipmapMode convert(const gfx::MipmapFilter mipmap_filter);
	constexpr [[nodiscard]] VkSampleCountFlagBits convert(const gfx::SampleCount samples);
	constexpr [[nodiscard]] VkShaderStageFlags convert(const BitFlag<gfx::ShaderStage> stage);
	constexpr [[nodiscard]] VkShaderStageFlagBits convert2(const BitFlag<gfx::ShaderStage> stage);
	constexpr [[nodiscard]] VkRect2D convert(gfx::Rect2D type);
	constexpr [[nodiscard]] VkVertexInputRate convert(const gfx::InputRate input_rate);
	constexpr [[nodiscard]] VkIndexType convert(const gfx::IndexType index_type);
	constexpr [[nodiscard]] VkStencilOp convert(const gfx::StencilOp stencil_op);
	constexpr [[nodiscard]] VkComponentSwizzle convert(const gfx::Swizzle swizzle);
	constexpr [[nodiscard]] VkComponentMapping convert(const gfx::SwizzleDescriptor swizzle_descriptor);
	constexpr [[nodiscard]] VkImageSubresourceRange convert(const gfx::ImageSubresourceDescriptor subresource_descriptor);
	constexpr [[nodiscard]] VkDescriptorSetLayoutBinding convert(const gfx::BindGroupLayoutEntryDescriptor &desc);
	constexpr [[nodiscard]] VkImageLayout convert(const gfx::ImageLayout type);
	constexpr [[nodiscard]] VkPolygonMode convert(const gfx::PolygonMode polygon_mode);
	constexpr [[nodiscard]] VkPrimitiveTopology convert(const gfx::PrimitiveTopology primitive_topology);
	constexpr [[nodiscard]] VkPresentModeKHR convert(const gfx::PresentMethod present_method);
	constexpr [[nodiscard]] VkPipelineStageFlagBits2 convert(const gfx::PipelineStage stage);
	constexpr [[nodiscard]] VkPipelineBindPoint convert(const gfx::PipelineBindPoint stage);
	constexpr [[nodiscard]] VkFrontFace convert(const gfx::FrontFace polygon_mode);
	constexpr [[nodiscard]] VkCullModeFlags convert(const BitFlag<gfx::CullMode> polygon_mode);
	constexpr [[nodiscard]] VkPipelineRasterizationStateCreateInfo convert(const gfx::RasterizationStateDescriptor &desc);
	
	constexpr [[nodiscard]] gfx::Format revert(const VkFormat format);
}

// template<> inline constexpr bool enable_enum_bitops<VkAccessFlagBits2> = true;
// template<> inline constexpr bool enable_enum_bitops<VkImageAspectFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkImageUsageFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkBufferUsageFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkShaderStageFlags> = true;
// template<> inline constexpr bool enable_enum_bitops<VkPipelineStageFlagBits2> = true;

#include "vulkan_enum_conversion.inl"