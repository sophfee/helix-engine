#pragma once

#include "engine/flags.hpp"
#include "gpu/driver.hpp"

namespace vk::detail {
	constexpr [[nodiscard]] VkAccessFlags2 convert(const BitFlag<gfx::Access> access);
	constexpr [[nodiscard]] VmaAllocationCreateFlags convert(const BitFlag<gfx::AllocationHint> hints);
	constexpr [[nodiscard]] VkAttachmentLoadOp convert(const gfx::LoadOp load_op);
	constexpr [[nodiscard]] VkAttachmentStoreOp convert(const gfx::StoreOp store_op);
	constexpr [[nodiscard]] VkBlendFactor convert(const gfx::BlendFactor blend_factor);
	constexpr [[nodiscard]] VkBlendOp convert(const gfx::BlendOp blend_op);
	constexpr [[nodiscard]] VkBufferUsageFlags convert(const BitFlag<gfx::BufferUsage> buffer_usage);
	constexpr [[nodiscard]] VkColorSpaceKHR convert(const gfx::ColorSpace color_space);
	constexpr [[nodiscard]] VkCompareOp convert(const gfx::CompareOp compare_op);
	//constexpr [[nodiscard]] VkComponentMapping convert(const gfx::SwizzleDescriptor swizzle_descriptor);
	constexpr [[nodiscard]] VkComponentSwizzle convert(const gfx::Swizzle swizzle);
	constexpr [[nodiscard]] VkCompositeAlphaFlagBitsKHR convert(const gfx::CompositeAlpha composite_alpha);
	constexpr [[nodiscard]] VkCullModeFlags convert(const BitFlag<gfx::CullMode> polygon_mode);
	//constexpr [[nodiscard]] VkDescriptorSetLayoutBinding convert(const gfx::BindGroupLayoutEntryDescriptor &desc);
	constexpr [[nodiscard]] VkDescriptorType convert(gfx::BindingType type);
	constexpr [[nodiscard]] VkDynamicState convert(gfx::DynamicState type);
	constexpr [[nodiscard]] VkFilter convert(const gfx::Filter filter);
	constexpr [[nodiscard]] VkFormat convert(const gfx::Format format);
	constexpr [[nodiscard]] VkFrontFace convert(const gfx::FrontFace polygon_mode);
	constexpr [[nodiscard]] VkImageAspectFlags convert(const BitFlag<gfx::Aspect> aspect);
	constexpr [[nodiscard]] VkImageLayout convert(const gfx::ImageLayout type);
	constexpr [[nodiscard]] VkImageSubresourceRange convert(const ImageSubresourceDescriptor subresource_descriptor);
	constexpr [[nodiscard]] VkImageType convert(const gfx::ImageType image_type);
	constexpr [[nodiscard]] VkImageUsageFlags convert(const gfx::ImageUsage image_flags);
	constexpr [[nodiscard]] VkImageViewType convert(const gfx::ImageViewType image_type);
	constexpr [[nodiscard]] VkIndexType convert(const gfx::IndexType index_type);
	constexpr [[nodiscard]] VmaMemoryUsage convert(const gfx::MemoryUsage memory_usage);
	constexpr [[nodiscard]] VkPolygonMode convert(const gfx::PolygonMode polygon_mode);
	constexpr [[nodiscard]] VkPrimitiveTopology convert(const gfx::PrimitiveTopology primitive_topology);
	constexpr [[nodiscard]] VkPresentModeKHR convert(const gfx::PresentMethod present_method);
	constexpr [[nodiscard]] VkPipelineStageFlagBits2 convert(const gfx::PipelineStage stage);
	constexpr [[nodiscard]] VkPipelineBindPoint convert(const gfx::PipelineBindPoint stage);
	constexpr [[nodiscard]] VkPipelineRasterizationStateCreateInfo convert(const RasterizationStateDescriptor &desc);
	constexpr [[nodiscard]] VkSamplerAddressMode convert(const gfx::AddressMode address_mode);
	constexpr [[nodiscard]] VkSamplerMipmapMode convert(const gfx::MipmapFilter mipmap_filter);
	constexpr [[nodiscard]] VkSampleCountFlagBits convert(const gfx::SampleCount samples);
	constexpr [[nodiscard]] VkShaderStageFlags convert(const BitFlag<gfx::ShaderStage> stage);
	constexpr [[nodiscard]] VkShaderStageFlagBits convert2(const BitFlag<gfx::ShaderStage> stage);
	constexpr [[nodiscard]] VkStencilOp convert(const gfx::StencilOp stencil_op);
	constexpr [[nodiscard]] VkRect2D convert(gfx::Rect2D type);
	constexpr [[nodiscard]] VkVertexInputRate convert(const gfx::InputRate input_rate);
	
	constexpr [[nodiscard]] gfx::Format revert(const VkFormat format);
	constexpr [[nodiscard]] gfx::ImageLayout revert(const VkImageLayout format);
	
	constexpr [[nodiscard]] AccessFlags2 convert_hpp(const BitFlag<gfx::Access> access) { return static_cast<AccessFlags2>(convert(access)); }
	constexpr [[nodiscard]] AttachmentLoadOp convert_hpp(const gfx::LoadOp load_op) { return static_cast<AttachmentLoadOp>(convert(load_op)); }
	constexpr [[nodiscard]] AttachmentStoreOp convert_hpp(const gfx::StoreOp store_op) { return static_cast<AttachmentStoreOp>(convert(store_op)); }
	constexpr [[nodiscard]] BlendFactor convert_hpp(const gfx::BlendFactor blend_factor) { return static_cast<BlendFactor>(convert(blend_factor)); }
	constexpr [[nodiscard]] BlendOp convert_hpp(const gfx::BlendOp blend_op) { return static_cast<BlendOp>(convert(blend_op)); }
	constexpr [[nodiscard]] BufferUsageFlags convert_hpp(const BitFlag<gfx::BufferUsage> buffer_usage) { return static_cast<BufferUsageFlags>(convert(buffer_usage)); }
	constexpr [[nodiscard]] ColorSpaceKHR convert_hpp(const gfx::ColorSpace color_space) { return static_cast<ColorSpaceKHR>(convert(color_space)); }
	constexpr [[nodiscard]] CompareOp convert_hpp(const gfx::CompareOp compare_op) { return	static_cast<CompareOp>(convert(compare_op)); }
	constexpr [[nodiscard]] ComponentSwizzle convert_hpp(const gfx::Swizzle swizzle) { return static_cast<ComponentSwizzle>(convert(swizzle)); }
	constexpr [[nodiscard]] CompositeAlphaFlagBitsKHR convert_hpp(const gfx::CompositeAlpha composite_alpha) { return static_cast<CompositeAlphaFlagBitsKHR>(convert(composite_alpha)); }
	constexpr [[nodiscard]] CullModeFlags convert_hpp(const BitFlag<gfx::CullMode> polygon_mode) { return static_cast<CullModeFlags>(convert(polygon_mode)); }
	constexpr [[nodiscard]] DescriptorType convert_hpp(const gfx::BindingType type) { return static_cast<DescriptorType>(convert(type)); }
	constexpr [[nodiscard]] DynamicState convert_hpp(const gfx::DynamicState type) { return static_cast<DynamicState>(convert(type)); }
	constexpr [[nodiscard]] Filter convert_hpp(const gfx::Filter filter) { return static_cast<Filter>(convert(filter)); }
	constexpr [[nodiscard]] Format convert_hpp(const gfx::Format format) { return static_cast<Format>(convert(format)); }
	constexpr [[nodiscard]] FrontFace convert_hpp(const gfx::FrontFace polygon_mode) { return static_cast<FrontFace>(convert(polygon_mode)); }
	constexpr [[nodiscard]] ImageAspectFlags convert_hpp(const BitFlag<gfx::Aspect> aspect) { return static_cast<ImageAspectFlags>(convert(aspect)); }
	constexpr [[nodiscard]] ImageLayout convert_hpp(const gfx::ImageLayout type) { return static_cast<ImageLayout>(convert(type)); }
	//constexpr [[nodiscard]] ImageSubresourceRange convert_hpp(const ImageSubresourceDescriptor subresource_descriptor) { return static_cast<ImageSubresourceRange>(convert(subresource_descriptor)); }
	constexpr [[nodiscard]] ImageType convert_hpp(const gfx::ImageType image_type) { return static_cast<ImageType>(convert(image_type)); }
	constexpr [[nodiscard]] ImageUsageFlags convert_hpp(const gfx::ImageUsage image_flags) { return static_cast<ImageUsageFlags>(convert(image_flags)); }
	constexpr [[nodiscard]] ImageViewType convert_hpp(const gfx::ImageViewType image_type) { return static_cast<ImageViewType>(convert(image_type)); }
	constexpr [[nodiscard]] IndexType convert_hpp(const gfx::IndexType index_type) { return static_cast<IndexType>(convert(index_type)); }
	constexpr [[nodiscard]] PolygonMode convert_hpp(const gfx::PolygonMode polygon_mode) { return static_cast<PolygonMode>(convert(polygon_mode)); }
	constexpr [[nodiscard]] PrimitiveTopology convert_hpp(const gfx::PrimitiveTopology primitive_topology) { return static_cast<PrimitiveTopology>(convert(primitive_topology)); }
	constexpr [[nodiscard]] PresentModeKHR convert_hpp(const gfx::PresentMethod present_method) { return static_cast<PresentModeKHR>(convert(present_method)); }
	constexpr [[nodiscard]] PipelineStageFlagBits2 convert_hpp(const gfx::PipelineStage stage) { return static_cast<PipelineStageFlagBits2>(convert(stage)); }
	constexpr [[nodiscard]] PipelineBindPoint convert_hpp(const gfx::PipelineBindPoint stage) { return static_cast<PipelineBindPoint>(convert(stage)); }
	//constexpr [[nodiscard]] PipelineRasterizationStateCreateInfo convert_hpp(const RasterizationStateDescriptor &desc) { return convert(desc); }
	constexpr [[nodiscard]] SamplerAddressMode convert_hpp(const gfx::AddressMode address_mode) { return static_cast<SamplerAddressMode>(convert(address_mode)); }
	constexpr [[nodiscard]] SamplerMipmapMode convert_hpp(const gfx::MipmapFilter mipmap_filter) { return static_cast<SamplerMipmapMode>(convert(mipmap_filter)); }
	constexpr [[nodiscard]] SampleCountFlagBits convert_hpp(const gfx::SampleCount samples) { return static_cast<SampleCountFlagBits>(convert(samples)); }
	constexpr [[nodiscard]] ShaderStageFlags convert_hpp(const BitFlag<gfx::ShaderStage> stage) { return static_cast<ShaderStageFlags>(convert(stage)); }
	constexpr [[nodiscard]] ShaderStageFlagBits convert2_hpp(const BitFlag<gfx::ShaderStage> stage) { return static_cast<ShaderStageFlagBits>(convert(stage)); }
	constexpr [[nodiscard]] StencilOp convert_hpp(const gfx::StencilOp stencil_op) { return static_cast<StencilOp>(convert(stencil_op)); }
	//constexpr [[nodiscard]] Rect2D convert_hpp(const gfx::Rect2D type) { return convert(type); }
	constexpr [[nodiscard]] VertexInputRate convert_hpp(const gfx::InputRate input_rate) { return static_cast<VertexInputRate>(convert(input_rate)); }
	
}

// template<> inline constexpr bool enable_enum_bitops<VkAccessFlagBits2> = true;
// template<> inline constexpr bool enable_enum_bitops<VkImageAspectFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkImageUsageFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkBufferUsageFlagBits> = true;
// template<> inline constexpr bool enable_enum_bitops<VkShaderStageFlags> = true;
// template<> inline constexpr bool enable_enum_bitops<VkPipelineStageFlagBits2> = true;

#include "vulkan_enum_conversion.inl"