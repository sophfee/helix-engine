#pragma once

#include "gpu/driver.hpp"

#include <vma/vk_mem_alloc.h>

namespace vk::detail {
	using ::detail::has_flag;

	constexpr VkAccessFlags2 convert(const BitFlag<gfx::Access> access) {
		using enum gfx::Access;
		
		VkAccessFlags2 flags = 0;
		if (access.has(BitFlag(eIndirectCommandRead))) flags |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
		if (access.has(BitFlag(eIndexRead))) flags |= VK_ACCESS_2_INDEX_READ_BIT;
		if (access.has(BitFlag(eVertexAttributeRead))) flags |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		if (access.has(BitFlag(eUniformRead))) flags |= VK_ACCESS_2_UNIFORM_READ_BIT;
		if (access.has(BitFlag(eInputAttachmentRead))) flags |= VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
		if (access.has(BitFlag(eShaderRead))) flags |= VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
		if (access.has(BitFlag(eShaderWrite))) flags |= VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
		if (access.has(BitFlag(eColorAttachmentRead))) flags |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
		if (access.has(BitFlag(eColorAttachmentWrite))) flags |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		if (access.has(BitFlag(eDepthStencilAttachmentRead))) flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		if (access.has(BitFlag(eDepthStencilAttachmentWrite))) flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		if (access.has(BitFlag(eTransferRead))) flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
		if (access.has(BitFlag(eTransferWrite))) flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		if (access.has(BitFlag(eHostRead))) flags |= VK_ACCESS_2_HOST_READ_BIT;
		if (access.has(BitFlag(eHostWrite))) flags |= VK_ACCESS_2_HOST_WRITE_BIT;
		if (access.has(BitFlag(eMemoryRead))) flags |= VK_ACCESS_2_MEMORY_READ_BIT;
		if (access.has(BitFlag(eMemoryWrite))) flags |= VK_ACCESS_2_MEMORY_WRITE_BIT;
		
		return flags;
	}
	constexpr VkSamplerAddressMode convert(const gfx::AddressMode address_mode) {
		using enum gfx::AddressMode;
		switch (address_mode) {
		case eRepeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case eMirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case eClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case eClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case eMirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		}
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
	constexpr VmaAllocationCreateFlags convert(const BitFlag<gfx::AllocationHint> hints) {
		using enum gfx::AllocationHint;
		VmaAllocationCreateFlags flags = 0;
		if (hints.has(eMapped)) flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		if (hints.has(eNeverAllocate)) flags |= VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT;
		if (hints.has(eUserDataCopyString)) flags |= VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
		if (hints.has(eUpperAddress)) flags |= VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT;
		if (hints.has(eDontBind)) flags |= VMA_ALLOCATION_CREATE_DONT_BIND_BIT;
		if (hints.has(eWithinBudget)) flags |= VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT;
		if (hints.has(eCanAlias)) flags |= VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT;
		if (hints.has(eHostSequentialWrite)) flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		if (hints.has(eAllowTransferInstead)) flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
		if (hints.has(eDedicated)) flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		if (hints.has(eStrategyMinTime)) flags |= VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
		if (hints.has(eStrategyMinOffset)) flags |= VMA_ALLOCATION_CREATE_STRATEGY_MIN_OFFSET_BIT;
		if (hints.has(eStrategyBestFit)) flags |= VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT;
		if (hints.has(eStrategyFirstFit)) flags |= VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT;
		return flags;
	}
	constexpr VkImageAspectFlags convert(const BitFlag<gfx::Aspect> aspect) {
		using enum gfx::Aspect;
		VkImageAspectFlags flags = 0;
		if (aspect.has(eColor)) flags |= VK_IMAGE_ASPECT_COLOR_BIT;
		if (aspect.has(eDepth)) flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (aspect.has(eStencil)) flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
		return flags;
	}
	constexpr VkDescriptorType convert(const gfx::BindingType type) {
		using enum gfx::BindingType;
		switch (type) {
		case eUniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case eStorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case eSampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
		case eImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case eSampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case eStorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		}
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	constexpr VkDynamicState convert(gfx::DynamicState type) {
		using enum gfx::DynamicState;
		switch (type) {
		case eViewport: return VK_DYNAMIC_STATE_VIEWPORT;
		case eScissor: return VK_DYNAMIC_STATE_SCISSOR;
		case eLineWidth: return VK_DYNAMIC_STATE_LINE_WIDTH;
		case eDepthBias: return VK_DYNAMIC_STATE_DEPTH_BIAS;
		case eBlendConstants: return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
		case eDepthBounds: return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
		case eStencilCompareMask: return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
		case eStencilWriteMask: return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
		case eStencilReference: return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
		case eCullMode: return VK_DYNAMIC_STATE_CULL_MODE;
		case eFrontFace: return VK_DYNAMIC_STATE_FRONT_FACE;
		case ePrimitiveTopology: return VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY;
		case eViewportWithCount: return VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT;
		case eScissorWithCount: return VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT;
		case eVertexInputBindingStride: return VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE;
		case eDepthTestEnable: return VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE;
		case eDepthWriteEnable: return VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE;
		case eDepthCompareOp: return VK_DYNAMIC_STATE_DEPTH_COMPARE_OP;
		case eDepthBoundsTestEnable: return VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE;
		case eStencilTestEnable: return VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE;
		case eStencilOp: return VK_DYNAMIC_STATE_STENCIL_OP;
		case eRasterizerDiscardEnable: return VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE;
		case eDepthBiasEnable: return VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE;
		case ePrimitiveRestartEnable: return VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE;
		}
		return VK_DYNAMIC_STATE_VIEWPORT;
	}

	constexpr VkBufferUsageFlags convert(const BitFlag<gfx::BufferUsage> buffer_usage) {
		using enum gfx::BufferUsage;
		VkBufferUsageFlags flags = 0;
		if (buffer_usage.has(eTransferSrc)) flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (buffer_usage.has(eTransferDst)) flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (buffer_usage.has(eUniform)) flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if (buffer_usage.has(eVertex)) flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (buffer_usage.has(eIndex)) flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if (buffer_usage.has(eStorage)) flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if (buffer_usage.has(eShaderDeviceAddress)) flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		return flags;
	}

	constexpr VmaMemoryUsage convert(const gfx::MemoryUsage memory_usage) {
		using enum gfx::MemoryUsage;
		switch (memory_usage) {
		case eAuto: return VMA_MEMORY_USAGE_AUTO;
		case ePreferDevice: return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		case ePreferHost: return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		case eGpu: return VMA_MEMORY_USAGE_GPU_ONLY;
		case eGpuToCpu: return VMA_MEMORY_USAGE_GPU_TO_CPU;
		case eGpuLazilyAllocated: return VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
		case eCpu: return VMA_MEMORY_USAGE_CPU_ONLY;
		case eCpuCopy: return VMA_MEMORY_USAGE_CPU_COPY;
		case eCpuToGpu: return VMA_MEMORY_USAGE_CPU_TO_GPU;
		case eUnknown: default: return VMA_MEMORY_USAGE_AUTO;
		}
	}

	constexpr VkImageUsageFlags convert(const BitFlag<gfx::ImageUsage> image_flags) {
		using enum gfx::ImageUsage;
		VkImageUsageFlags flags = 0;
		if (image_flags.has(eTransferSrc)) flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (image_flags.has(eTransferDst)) flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (image_flags.has(eSampled)) flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if (image_flags.has(eStorage)) flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if (image_flags.has(eColorAttachment)) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (image_flags.has(eDepthStencilAttachment)) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if (image_flags.has(eTransientAttachment)) flags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if (image_flags.has(eInputAttachment)) flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		return flags;
	}

	constexpr VkImageType convert(const gfx::ImageType image_type) {
		using enum gfx::ImageType;
		switch (image_type) {
		case e1D: return VK_IMAGE_TYPE_1D;
		case e2D: return VK_IMAGE_TYPE_2D;
		case e3D: return VK_IMAGE_TYPE_3D;
		}
		return VK_IMAGE_TYPE_2D;
	}

	constexpr VkImageViewType convert(const gfx::ImageViewType image_type) {
		using enum gfx::ImageViewType;
		switch (image_type) {
		case e1D: return VK_IMAGE_VIEW_TYPE_1D;
		case e2D: return VK_IMAGE_VIEW_TYPE_2D;
		case e3D: return VK_IMAGE_VIEW_TYPE_3D;
		case eCube: return VK_IMAGE_VIEW_TYPE_CUBE;
		case e1DArray: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		case e2DArray: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		case eCubeArray: return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		}
		return VK_IMAGE_VIEW_TYPE_2D;
	}

	constexpr VkFormat convert(const gfx::Format format) {
		using enum gfx::Format;
		switch (format) {
		case eBgra8Unorm: return VK_FORMAT_B8G8R8A8_UNORM;
		case eRgba8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
		case eDepth24UnormStencil8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
		case eDepth32SfloatStencil8Uint: return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case eUndefined: return VK_FORMAT_UNDEFINED;
		case eRg4UnormPack8: return VK_FORMAT_R4G4_UNORM_PACK8;
		case eRgba4UnormPack16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
		case eBgra4UnormPack16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
		case eR5G6B5UnormPack16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
		case eB5G6R5UnormPack16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
		case eRgb5A1UnormPack16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
		case eBgr5A1UnormPack16: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
		case eA1Rgb5UnormPack16: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
		case eR8Unorm: return VK_FORMAT_R8_UNORM;
		case eR8Snorm: return VK_FORMAT_R8_SNORM;
		case eR8Uscaled: return VK_FORMAT_R8_USCALED;
		case eR8Sscaled: return VK_FORMAT_R8_SSCALED;
		case eR8Uint: return VK_FORMAT_R8_UINT;
		case eR8Sint: return VK_FORMAT_R8_SINT;
		case eR8Srgb: return VK_FORMAT_R8_SRGB;
		case eRg8Unorm: return VK_FORMAT_R8G8_UNORM;
		case eRg8Snorm: return VK_FORMAT_R8G8_SNORM;
		case eRg8Uscaled: return VK_FORMAT_R8G8_USCALED;
		case eRg8Sscaled: return VK_FORMAT_R8G8_SSCALED;
		case eRg8Uint: return VK_FORMAT_R8G8_UINT;
		case eRg8Sint: return VK_FORMAT_R8G8_SINT;
		case eRg8Srgb: return VK_FORMAT_R8G8_SRGB;
		case eRgb8Unorm: return VK_FORMAT_R8G8B8_UNORM;
		case eRgb8Snorm: return VK_FORMAT_R8G8B8_SNORM;
		case eRgb8Uscaled: return VK_FORMAT_R8G8B8_USCALED;
		case eRgb8Sscaled: return VK_FORMAT_R8G8B8_SSCALED;
		case eRgb8Uint: return VK_FORMAT_R8G8B8_UINT;
		case eRgb8Sint: return VK_FORMAT_R8G8B8_SINT;
		case eRgb8Srgb: return VK_FORMAT_R8G8B8_SRGB;
		case eBgr8Unorm: return VK_FORMAT_B8G8R8_UNORM;
		case eBgr8Snorm: return VK_FORMAT_B8G8R8_SNORM;
		case eBgr8Uscaled: return VK_FORMAT_B8G8R8_USCALED;
		case eBgr8Sscaled: return VK_FORMAT_B8G8R8_SSCALED;
		case eBgr8Uint: return VK_FORMAT_B8G8R8_UINT;
		case eBgr8Sint: return VK_FORMAT_B8G8R8_SINT;
		case eBgr8Srgb: return VK_FORMAT_B8G8R8_SRGB;
		case eRgba8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
		case eRgba8Uscaled: return VK_FORMAT_R8G8B8A8_USCALED;
		case eRgba8Sscaled: return VK_FORMAT_R8G8B8A8_SSCALED;
		case eRgba8Uint: return VK_FORMAT_R8G8B8A8_UINT;
		case eRgba8Sint: return VK_FORMAT_R8G8B8A8_SINT;
		case eRgba8Srgb: return VK_FORMAT_R8G8B8A8_SRGB;
		case eBgra8Snorm: return VK_FORMAT_B8G8R8A8_SNORM;
		case eBgra8Uscaled: return VK_FORMAT_B8G8R8A8_USCALED;
		case eBgra8Sscaled: return VK_FORMAT_B8G8R8A8_SSCALED;
		case eBgra8Uint: return VK_FORMAT_B8G8R8A8_UINT;
		case eBgra8Sint: return VK_FORMAT_B8G8R8A8_SINT;
		case eBgra8Srgb: return VK_FORMAT_B8G8R8A8_SRGB;
		case eAbgr8UnormPack32: return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
		case eAbgr8SnormPack32: return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
		case eAbgr8UscaledPack32: return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
		case eAbgr8SscaledPack32: return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
		case eAbgr8UintPack32: return VK_FORMAT_A8B8G8R8_UINT_PACK32;
		case eAbgr8SintPack32: return VK_FORMAT_A8B8G8R8_SINT_PACK32;
		case eAbgr8SrgbPack32: return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
		case eA2Rgb10UnormPack32: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
		case eA2Rgb10SnormPack32: return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
		case eA2Rgb10UscaledPack32: return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
		case eA2Rgb10SscaledPack32: return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
		case eA2Rgb10UintPack32: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
		case eA2Rgb10SintPack32: return VK_FORMAT_A2R10G10B10_SINT_PACK32;
		case eA2Bgr10UnormPack32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case eA2Bgr10SnormPack32: return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
		case eA2Bgr10UscaledPack32: return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
		case eA2Bgr10SscaledPack32: return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
		case eA2Bgr10UintPack32: return VK_FORMAT_A2B10G10R10_UINT_PACK32;
		case eA2Bgr10SintPack32: return VK_FORMAT_A2B10G10R10_SINT_PACK32;
		case eR16Unorm: return VK_FORMAT_R16_UNORM;
		case eR16Snorm: return VK_FORMAT_R16_SNORM;
		case eR16Uscaled: return VK_FORMAT_R16_USCALED;
		case eR16Sscaled: return VK_FORMAT_R16_SSCALED;
		case eR16Uint: return VK_FORMAT_R16_UINT;
		case eR16Sint: return VK_FORMAT_R16_SINT;
		case eR16Sfloat: return VK_FORMAT_R16_SFLOAT;
		case eRg16Unorm: return VK_FORMAT_R16G16_UNORM;
		case eRg16Snorm: return VK_FORMAT_R16G16_SNORM;
		case eRg16Uscaled: return VK_FORMAT_R16G16_USCALED;
		case eRg16Sscaled: return VK_FORMAT_R16G16_SSCALED;
		case eRg16Uint: return VK_FORMAT_R16G16_UINT;
		case eRg16Sint: return VK_FORMAT_R16G16_SINT;
		case eRg16Sfloat: return VK_FORMAT_R16G16_SFLOAT;
		case eRgb16Unorm: return VK_FORMAT_R16G16B16_UNORM;
		case eRgb16Snorm: return VK_FORMAT_R16G16B16_SNORM;
		case eRgb16Uscaled: return VK_FORMAT_R16G16B16_USCALED;
		case eRgb16Sscaled: return VK_FORMAT_R16G16B16_SSCALED;
		case eRgb16Uint: return VK_FORMAT_R16G16B16_UINT;
		case eRgb16Sint: return VK_FORMAT_R16G16B16_SINT;
		case eRgb16Sfloat: return VK_FORMAT_R16G16B16_SFLOAT;
		case eRgba16Unorm: return VK_FORMAT_R16G16B16A16_UNORM;
		case eRgba16Snorm: return VK_FORMAT_R16G16B16A16_SNORM;
		case eRgba16Uscaled: return VK_FORMAT_R16G16B16A16_USCALED;
		case eRgba16Sscaled: return VK_FORMAT_R16G16B16A16_SSCALED;
		case eRgba16Uint: return VK_FORMAT_R16G16B16A16_UINT;
		case eRgba16Sint: return VK_FORMAT_R16G16B16A16_SINT;
		case eRgba16Sfloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case eR32Uint: return VK_FORMAT_R32_UINT;
		case eR32Sint: return VK_FORMAT_R32_SINT;
		case eR32Sfloat: return VK_FORMAT_R32_SFLOAT;
		case eRg32Uint: return VK_FORMAT_R32G32_UINT;
		case eRg32Sint: return VK_FORMAT_R32G32_SINT;
		case eRg32Sfloat: return VK_FORMAT_R32G32_SFLOAT;
		case eRgb32Uint: return VK_FORMAT_R32G32B32_UINT;
		case eRgb32Sint: return VK_FORMAT_R32G32B32_SINT;
		case eRgb32Sfloat: return VK_FORMAT_R32G32B32_SFLOAT;
		case eRgba32Uint: return VK_FORMAT_R32G32B32A32_UINT;
		case eRgba32Sint: return VK_FORMAT_R32G32B32A32_SINT;
		case eRgba32Sfloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case eR64Uint: return VK_FORMAT_R64_UINT;
		case eR64Sint: return VK_FORMAT_R64_SINT;
		case eR64Sfloat: return VK_FORMAT_R64_SFLOAT;
		case eRg64Uint: return VK_FORMAT_R64G64_UINT;
		case eRg64Sint: return VK_FORMAT_R64G64_SINT;
		case eRg64Sfloat: return VK_FORMAT_R64G64_SFLOAT;
		case eRgb64Uint: return VK_FORMAT_R64G64B64_UINT;
		case eRgb64Sint: return VK_FORMAT_R64G64B64_SINT;
		case eRgb64Sfloat: return VK_FORMAT_R64G64B64_SFLOAT;
		case eRgba64Uint: return VK_FORMAT_R64G64B64A64_UINT;
		case eRgba64Sint: return VK_FORMAT_R64G64B64A64_SINT;
		case eRgba64Sfloat: return VK_FORMAT_R64G64B64A64_SFLOAT;
		case eB10Gr11UfloatPack32: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case eE5B9G9R9UfloatPack32: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		case eD16Unorm: return VK_FORMAT_D16_UNORM;
		case eX8D24UnormPack32: return VK_FORMAT_X8_D24_UNORM_PACK32;
		case eDepth32Sfloat: return VK_FORMAT_D32_SFLOAT;
		case eS8Uint: return VK_FORMAT_S8_UINT;
		case eDepth16UnormStencil8Uint: return VK_FORMAT_D16_UNORM_S8_UINT;
		case eBc1RgbUnormBlock: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case eBc1RgbSrgbBlock: return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
		case eBc1RgbaUnormBlock: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case eBc1RgbaSrgbBlock: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		case eBc2UnormBlock: return VK_FORMAT_BC2_UNORM_BLOCK;
		case eBc2SrgbBlock: return VK_FORMAT_BC2_SRGB_BLOCK;
		case eBc3UnormBlock: return VK_FORMAT_BC3_UNORM_BLOCK;
		case eBc3SrgbBlock: return VK_FORMAT_BC3_SRGB_BLOCK;
		case eBc4UnormBlock: return VK_FORMAT_BC4_UNORM_BLOCK;
		case eBc4SnormBlock: return VK_FORMAT_BC4_SNORM_BLOCK;
		case eBc5UnormBlock: return VK_FORMAT_BC5_UNORM_BLOCK;
		case eBc5SnormBlock: return VK_FORMAT_BC5_SNORM_BLOCK;
		case eBc6HUfloatBlock: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case eBc6HSfloatBlock: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case eBc7UnormBlock: return VK_FORMAT_BC7_UNORM_BLOCK;
		case eBc7SrgbBlock: return VK_FORMAT_BC7_SRGB_BLOCK;
		case eEtc2Rgb8UnormBlock: return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
		case eEtc2Rgb8SrgbBlock: return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
		case eEtc2Rgb8A1UnormBlock: return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
		case eEtc2Rgb8A1SrgbBlock: return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
		case eEtc2Rgba8UnormBlock: return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		case eEtc2Rgba8SrgbBlock: return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
		case eEacR11UnormBlock: return VK_FORMAT_EAC_R11_UNORM_BLOCK;
		case eEacR11SnormBlock: return VK_FORMAT_EAC_R11_SNORM_BLOCK;
		case eEacR11G11UnormBlock: return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		case eEacR11G11SnormBlock: return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
		case eAstc4x4UnormBlock: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		case eAstc4x4SrgbBlock: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
		case eAstc5x4UnormBlock: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
		case eAstc5x4SrgbBlock: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
		case eAstc5x5UnormBlock: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
		case eAstc5x5SrgbBlock: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
		case eAstc6x5UnormBlock: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
		case eAstc6x5SrgbBlock: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
		case eAstc6x6UnormBlock: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
		case eAstc6x6SrgbBlock: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
		case eAstc8x5UnormBlock: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
		case eAstc8x5SrgbBlock: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
		case eAstc8x6UnormBlock: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
		case eAstc8x6SrgbBlock: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
		case eAstc8x8UnormBlock: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
		case eAstc8x8SrgbBlock: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
		case eAstc10x5UnormBlock: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
		case eAstc10x5SrgbBlock: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
		case eAstc10x6UnormBlock: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
		case eAstc10x6SrgbBlock: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
		case eAstc10x8UnormBlock: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
		case eAstc10x8SrgbBlock: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
		case eAstc10x10UnormBlock: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
		case eAstc10x10SrgbBlock: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
		case eAstc12x10UnormBlock: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
		case eAstc12x10SrgbBlock: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
		case eAstc12x12UnormBlock: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
		case eAstc12x12SrgbBlock: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
		case eGbgr8422Unorm: return VK_FORMAT_G8B8G8R8_422_UNORM;
		case eBgrg8422Unorm: return VK_FORMAT_B8G8R8G8_422_UNORM;
		case eArgb4UnormPack16: return VK_FORMAT_A4R4G4B4_UNORM_PACK16;
		case eAbgr4UnormPack16: return VK_FORMAT_A4B4G4R4_UNORM_PACK16;
		case eAstc4x4SfloatBlock: return VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK;
		case eAstc5x4SfloatBlock: return VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK;
		case eAstc5x5SfloatBlock: return VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK;
		case eAstc6x5SfloatBlock: return VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK;
		case eAstc6x6SfloatBlock: return VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK;
		case eAstc8x5SfloatBlock: return VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK;
		case eAstc8x6SfloatBlock: return VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK;
		case eAstc8x8SfloatBlock: return VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK;
		case eAstc10x5SfloatBlock: return VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK;
		case eAstc10x6SfloatBlock: return VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK;
		case eAstc10x8SfloatBlock: return VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK;
		case eAstc10x10SfloatBlock: return VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK;
		case eAstc12x10SfloatBlock: return VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK;
		case eAstc12x12SfloatBlock: return VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK;
		case eA1Bgr5UnormPack16: return VK_FORMAT_A1B5G5R5_UNORM_PACK16;
		case eA8Unorm: return VK_FORMAT_A8_UNORM;
		case ePvrtc12BppUnormBlockIMG: return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
		case ePvrtc14BppUnormBlockIMG: return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
		case ePvrtc22BppUnormBlockIMG: return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
		case ePvrtc24BppUnormBlockIMG: return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
		case ePvrtc12BppSrgbBlockIMG: return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
		case ePvrtc14BppSrgbBlockIMG: return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
		case ePvrtc22BppSrgbBlockIMG: return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
		case ePvrtc24BppSrgbBlockIMG: return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
		case eRg16Sfixed5NV: return VK_FORMAT_R16G16_SFIXED5_NV;
		case eRg16S105NV: return VK_FORMAT_R16G16_S10_5_NV;
		default: return VK_FORMAT_UNDEFINED;
		}
	}

	constexpr VkFilter convert(const gfx::Filter filter) {
		using enum gfx::Filter;
		switch (filter) {
		case eNearest: return VK_FILTER_NEAREST;
		case eLinear: return VK_FILTER_LINEAR;
		}
		return VK_FILTER_NEAREST;
	}

	constexpr VkCompareOp convert(const gfx::CompareOp compare_op) {
		using enum gfx::CompareOp;
		switch (compare_op) {
		case eNever: return VK_COMPARE_OP_NEVER;
		case eLess: return VK_COMPARE_OP_LESS;
		case eEqual: return VK_COMPARE_OP_EQUAL;
		case eLessOrEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
		case eGreater: return VK_COMPARE_OP_GREATER;
		case eNotEqual: return VK_COMPARE_OP_NOT_EQUAL;
		case eGreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case eAlways: return VK_COMPARE_OP_ALWAYS;
		}
		return VK_COMPARE_OP_NEVER;
	}

	constexpr VkColorSpaceKHR convert(const gfx::ColorSpace color_space) {
		using enum gfx::ColorSpace;
		switch (color_space) {
		case eSrgbNonLinear: return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		default: return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
	}

	constexpr VkCompositeAlphaFlagBitsKHR convert(const gfx::CompositeAlpha composite_alpha) {
		using enum gfx::CompositeAlpha;
		switch (composite_alpha) {
		case eOpaque: return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		case ePreMultiplied: return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		case ePostMultiplied: return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
		case eInherit: return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		}
		return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}

	constexpr VkAttachmentLoadOp convert(const gfx::LoadOp load_op) {
		using enum gfx::LoadOp;
		switch (load_op) {
		case eLoad: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case eClear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case eDontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		case eNone: return VK_ATTACHMENT_LOAD_OP_NONE;
		}
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

	constexpr VkAttachmentStoreOp convert(const gfx::StoreOp store_op) {
		using enum gfx::StoreOp;
		switch (store_op) {
		case eStore: return VK_ATTACHMENT_STORE_OP_STORE;
		case eDontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case eNone: return VK_ATTACHMENT_STORE_OP_NONE;
		}
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}
	constexpr VkSamplerMipmapMode convert(const gfx::MipmapFilter mipmap_filter) {
		using enum gfx::MipmapFilter;
		switch (mipmap_filter) {
		case eNearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case eLinear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	constexpr VkSampleCountFlagBits convert(const gfx::SampleCount samples) {
		using enum gfx::SampleCount;
		switch (samples) {
		case e1: return VK_SAMPLE_COUNT_1_BIT;
		case e2: return VK_SAMPLE_COUNT_2_BIT;
		case e4: return VK_SAMPLE_COUNT_4_BIT;
		case e8: return VK_SAMPLE_COUNT_8_BIT;
		case e16: return VK_SAMPLE_COUNT_16_BIT;
		case e32: return VK_SAMPLE_COUNT_32_BIT;
		case e64: return VK_SAMPLE_COUNT_64_BIT;
		}
		return VK_SAMPLE_COUNT_1_BIT;
	}

	constexpr VkShaderStageFlags convert(const BitFlag<gfx::ShaderStage> stage) {
		using enum gfx::ShaderStage;
		VkShaderStageFlags flags = 0;
		if (stage.has(eVertex)) flags |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stage.has(eFragment)) flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stage.has(eGeometry)) flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (stage.has(eTesselationControl)) flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if (stage.has(eTesselationEvaluation)) flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if (stage.has(eCompute)) flags |= VK_SHADER_STAGE_COMPUTE_BIT;
		if (stage.has(eMesh)) flags |= VK_SHADER_STAGE_MESH_BIT_EXT;
		if (stage.has(eTask)) flags |= VK_SHADER_STAGE_TASK_BIT_EXT;
		if (stage.has(eRaygen)) flags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		if (stage.has(eAnyHit)) flags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		if (stage.has(eClosestHit)) flags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		if (stage.has(eMiss)) flags |= VK_SHADER_STAGE_MISS_BIT_KHR;
		if (stage.has(eIntersection)) flags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		if (stage.has(eCallable)) flags |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		if (stage.has(eAll)) flags |= VK_SHADER_STAGE_ALL;
		if (stage.has(eAllGraphics)) flags |= VK_SHADER_STAGE_ALL_GRAPHICS;
		return flags;
	}

	constexpr VkShaderStageFlagBits convert2(const BitFlag<gfx::ShaderStage> stage) {
		using enum gfx::ShaderStage;
		if (stage.has(eVertex)) return VK_SHADER_STAGE_VERTEX_BIT;
		if (stage.has(eFragment)) return VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stage.has(eGeometry)) return VK_SHADER_STAGE_GEOMETRY_BIT;
		if (stage.has(eTesselationControl)) return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if (stage.has(eTesselationEvaluation)) return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if (stage.has(eCompute)) return VK_SHADER_STAGE_COMPUTE_BIT;
		if (stage.has(eMesh)) return VK_SHADER_STAGE_MESH_BIT_EXT;
		if (stage.has(eTask)) return VK_SHADER_STAGE_TASK_BIT_EXT;
		if (stage.has(eRaygen)) return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		if (stage.has(eAnyHit)) return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		if (stage.has(eClosestHit)) return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		if (stage.has(eMiss)) return VK_SHADER_STAGE_MISS_BIT_KHR;
		if (stage.has(eIntersection)) return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		if (stage.has(eCallable)) return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		if (stage.has(eAll)) return VK_SHADER_STAGE_ALL;
		if (stage.has(eAllGraphics)) return VK_SHADER_STAGE_ALL_GRAPHICS;
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	
	constexpr VkVertexInputRate convert(const gfx::InputRate input_rate) {
		using enum gfx::InputRate;
		switch (input_rate) {
		case eVertex: return VK_VERTEX_INPUT_RATE_VERTEX;
		case eInstance: return VK_VERTEX_INPUT_RATE_INSTANCE;
		}
		return VK_VERTEX_INPUT_RATE_VERTEX;
	}

	constexpr VkIndexType convert(const gfx::IndexType index_type) {
		using enum gfx::IndexType;
		switch (index_type) {
		case eUInt8: return VK_INDEX_TYPE_UINT8; 
		case eUInt16: return VK_INDEX_TYPE_UINT16;
		case eUInt32: return VK_INDEX_TYPE_UINT32;
		case eNone: return VK_INDEX_TYPE_NONE_KHR;
		}
		return VK_INDEX_TYPE_UINT16;
	}

	constexpr VkStencilOp convert(const gfx::StencilOp stencil_op) {
		using enum gfx::StencilOp;
		switch (stencil_op) {
		case eKeep: return VK_STENCIL_OP_KEEP;
		case eZero: return VK_STENCIL_OP_ZERO;
		case eReplace: return VK_STENCIL_OP_REPLACE;
		case eIncrementAndClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case eDecrementAndClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case eInvert: return VK_STENCIL_OP_INVERT;
		case eIncrementAndWrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case eDecrementAndWrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
		return VK_STENCIL_OP_KEEP;
	}

	constexpr VkComponentSwizzle convert(const gfx::Swizzle swizzle) {
		using enum gfx::Swizzle;
		switch (swizzle) {
		case eIdentity: return VK_COMPONENT_SWIZZLE_IDENTITY;
		case eZero: return VK_COMPONENT_SWIZZLE_ZERO;
		case eOne: return VK_COMPONENT_SWIZZLE_ONE;
		case eR: return VK_COMPONENT_SWIZZLE_R;
		case eG: return VK_COMPONENT_SWIZZLE_G;
		case eB: return VK_COMPONENT_SWIZZLE_B;
		case eA: return VK_COMPONENT_SWIZZLE_A;
		}
		return VK_COMPONENT_SWIZZLE_IDENTITY;
	}

	constexpr VkComponentMapping convert(const gfx::SwizzleDescriptor swizzle_descriptor) {
		return VkComponentMapping{
			.r = convert(swizzle_descriptor.r),
			.g = convert(swizzle_descriptor.g),
			.b = convert(swizzle_descriptor.b),
			.a = convert(swizzle_descriptor.a)
		};
	}

	constexpr VkImageSubresourceRange convert(const gfx::ImageSubresourceDescriptor subresource_descriptor) {
		return VkImageSubresourceRange{
			.aspectMask = convert(subresource_descriptor.aspect_mask),
			.baseMipLevel = subresource_descriptor.base_mip_level,
			.levelCount = subresource_descriptor.level_count,
			.baseArrayLayer = subresource_descriptor.base_array_layer,
			.layerCount = subresource_descriptor.layer_count
		};
	}
	constexpr VkDescriptorSetLayoutBinding convert(const BindGroupLayoutEntryDescriptor &desc) {
		const VkDescriptorSetLayoutBinding binding{
			.binding = desc.binding,
			.descriptorType = convert(desc.type),
			.descriptorCount = desc.count.value_or(1),
			.stageFlags = convert(desc.visibility),
			.pImmutableSamplers = nullptr
		};
		return binding;
	}
	constexpr VkImageLayout convert(const gfx::ImageLayout type) {
		using enum gfx::ImageLayout;
		switch (type) {
		case eUndefined: return VK_IMAGE_LAYOUT_UNDEFINED;
		case eGeneral: return VK_IMAGE_LAYOUT_GENERAL;
		case eAttachmentOptimal: return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		case eColorAttachmentOptimal: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case eDepthStencilAttachmentOptimal: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case eDepthStencilReadOnlyOptimal: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		case eShaderReadOnlyOptimal: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case eTransferSrcOptimal: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case eTransferDstOptimal: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case ePreinitialized: return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case ePresent: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			break;
		}
	}

	constexpr VkPolygonMode convert(const gfx::PolygonMode polygon_mode) {
		using enum gfx::PolygonMode;
		switch (polygon_mode) {
		case eFill: return VK_POLYGON_MODE_FILL;
		case eLine: return VK_POLYGON_MODE_LINE;
		case ePoint: return VK_POLYGON_MODE_POINT;
		}
		return VK_POLYGON_MODE_FILL;
	}

	constexpr VkPrimitiveTopology convert(const gfx::PrimitiveTopology primitive_topology) {
		using enum gfx::PrimitiveTopology;
		switch (primitive_topology) {
		case ePointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case eLineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case eLineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case eTriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case eTriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case eTriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case eLineListWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
		case eLineStripWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
		case eTriangleListWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
		case eTriangleStripWithAdjacency: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
		case ePatchList: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		}
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	constexpr VkPresentModeKHR convert(const gfx::PresentMethod present_method) {
		using enum gfx::PresentMethod;
		switch (present_method) {
		case eImmediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case eMailbox: return VK_PRESENT_MODE_MAILBOX_KHR;
		case eFifo: return VK_PRESENT_MODE_FIFO_KHR;
		case eFifoRelaxed: return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	constexpr VkPipelineStageFlagBits2 convert(const gfx::PipelineStage stage) {
		using enum gfx::PipelineStage;
		switch (stage) {
		case eTopOfPipe: return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		case eDrawIndirect: return VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
		case eVertexInput: return VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
		case eVertexShader: return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
		case eTessellationControlShader: return VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT;
		case eTessellationEvaluationShader: return VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;
		case eGeometryShader: return VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
		case eFragmentShader: return VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		case eEarlyFragmentTests: return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		case eLateFragmentTests: return VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		case eColorAttachmentOutput: return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		case eComputeShader: return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		case eTransfer: return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		case eBottomOfPipe: return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
		case eHost: return VK_PIPELINE_STAGE_2_HOST_BIT;
		case eAllGraphics: return VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		case eAllCommands: return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		case eCopy: return VK_PIPELINE_STAGE_2_COPY_BIT;
		case eResolve: return VK_PIPELINE_STAGE_2_RESOLVE_BIT;
		case eBlit: return VK_PIPELINE_STAGE_2_BLIT_BIT;
		case eClear: return VK_PIPELINE_STAGE_2_CLEAR_BIT;
		case eIndexInput: return VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
		case eVertexAttributeInput: return VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
		case ePreRasterizationShaders: return VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT;
		case eTransformFeedback: return VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT;
		case eConditionalRendering: return VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT;
		case eCommandPreprocess: return VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV;
		case eFragmentShadingRateAttachment: return VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		case eShadingRateImage: return VK_PIPELINE_STAGE_2_SHADING_RATE_IMAGE_BIT_NV;
		case eAccelerationStructureBuild: return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case eRayTracingShader: return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
		case eFragmentDensityProcess: return VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
		case eTaskShader: return VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_NV;
		case eMeshShader: return VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_NV;
		default: return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		}
	}

	constexpr VkPipelineBindPoint convert(const gfx::PipelineBindPoint stage) {
		using enum gfx::PipelineBindPoint;
		switch (stage) {
		case eGraphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
		case eCompute: return VK_PIPELINE_BIND_POINT_COMPUTE;
		default: return VK_PIPELINE_BIND_POINT_GRAPHICS;
		}
	}

	constexpr VkFrontFace convert(const gfx::FrontFace polygon_mode) {
		using enum gfx::FrontFace;
		switch (polygon_mode) {
		case eCounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case eClockwise: return VK_FRONT_FACE_CLOCKWISE;
		}
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}

	constexpr VkCullModeFlags convert(const BitFlag<gfx::CullMode> polygon_mode) {
		using enum gfx::CullMode;
		VkCullModeFlags flags = 0;
		if (polygon_mode.has(eFront)) flags |= VK_CULL_MODE_FRONT_BIT;
		if (polygon_mode.has(eBack)) flags |= VK_CULL_MODE_BACK_BIT;
		if (polygon_mode.has(eFrontAndBack)) flags |= VK_CULL_MODE_FRONT_AND_BACK;
		return flags;
	}

	constexpr VkPipelineRasterizationStateCreateInfo convert(const gfx::RasterizationStateDescriptor &desc) {
		return VkPipelineRasterizationStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = desc.depth_clamp_enable,
			.rasterizerDiscardEnable = desc.rasterizer_discard_enable,
			.polygonMode = convert(desc.polygon_mode),
			.cullMode = convert(desc.cull_mode),
			.frontFace = convert(desc.front_face),
			.depthBiasEnable = desc.depth_bias_enable,
			.depthBiasConstantFactor = desc.depth_bias_constant_factor,
			.depthBiasClamp = desc.depth_bias_clamp,
			.depthBiasSlopeFactor = desc.depth_bias_slope_factor,
			.lineWidth = desc.line_width
		};
	}

	constexpr gfx::Format convert(const VkFormat format) {
		using enum gfx::Format;
		switch (format) {
		case VK_FORMAT_R4G4_UNORM_PACK8: return gfx::Format::eRg4UnormPack8;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return gfx::Format::eRgba4UnormPack16;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return gfx::Format::eBgra4UnormPack16;
		case VK_FORMAT_R5G6B5_UNORM_PACK16: return gfx::Format::eR5G6B5UnormPack16;
		case VK_FORMAT_B5G6R5_UNORM_PACK16: return gfx::Format::eB5G6R5UnormPack16;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return gfx::Format::eRgb5A1UnormPack16;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return gfx::Format::eBgr5A1UnormPack16;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return gfx::Format::eA1Rgb5UnormPack16;
		case VK_FORMAT_R8G8B8_SRGB: return gfx::Format::eRgb8Srgb;
		case VK_FORMAT_R8G8B8A8_SRGB: return gfx::Format::eRgba8Srgb;
		case VK_FORMAT_B8G8R8_SRGB: return gfx::Format::eBgr8Srgb;
		case VK_FORMAT_B8G8R8A8_SRGB: return gfx::Format::eBgra8Srgb;
		case VK_FORMAT_UNDEFINED: return eUndefined;
		case VK_FORMAT_R8_UNORM: return eR8Unorm;
		case VK_FORMAT_R8_SNORM: return eR8Snorm;
		case VK_FORMAT_R8_USCALED: return eR8Uscaled;
		case VK_FORMAT_R8_SSCALED: return eR8Sscaled;
		case VK_FORMAT_R8_UINT: return eR8Uint;
		case VK_FORMAT_R8_SINT: return eR8Sint;
		case VK_FORMAT_R8_SRGB: return eR8Srgb;
		case VK_FORMAT_R8G8_UNORM: return eRg8Unorm;
		case VK_FORMAT_R8G8_SNORM: return eRg8Snorm;
		case VK_FORMAT_R8G8_USCALED: return eRg8Uscaled;
		case VK_FORMAT_R8G8_SSCALED: return eRg8Sscaled;
		case VK_FORMAT_R8G8_UINT: return eRg8Uint;
		case VK_FORMAT_R8G8_SINT: return eRg8Sint;
		case VK_FORMAT_R8G8_SRGB: return eRg8Srgb;
		case VK_FORMAT_R8G8B8_UNORM: return eRgb8Unorm;
		case VK_FORMAT_R8G8B8_SNORM: return eRgb8Snorm;
		case VK_FORMAT_R8G8B8_USCALED: return eRgb8Uscaled;
		case VK_FORMAT_R8G8B8_SSCALED: return eRgb8Sscaled;
		case VK_FORMAT_R8G8B8_UINT: return eRgb8Uint;
		case VK_FORMAT_R8G8B8_SINT: return eRgb8Sint;
		case VK_FORMAT_B8G8R8_UNORM: return eBgr8Unorm;
		case VK_FORMAT_B8G8R8_SNORM: return eBgr8Snorm;
		case VK_FORMAT_B8G8R8_USCALED: return eBgr8Uscaled;
		case VK_FORMAT_B8G8R8_SSCALED: return eBgr8Sscaled;
		case VK_FORMAT_B8G8R8_UINT: return eBgr8Uint;
		case VK_FORMAT_B8G8R8_SINT: return eBgr8Sint;
		case VK_FORMAT_R8G8B8A8_UNORM: return eRgba8Unorm;
		case VK_FORMAT_R8G8B8A8_SNORM: return eRgba8Snorm;
		case VK_FORMAT_R8G8B8A8_USCALED: return eRgba8Uscaled;
		case VK_FORMAT_R8G8B8A8_SSCALED: return eRgba8Sscaled;
		case VK_FORMAT_R8G8B8A8_UINT: return eRgba8Uint;
		case VK_FORMAT_R8G8B8A8_SINT: return eRgba8Sint;
		case VK_FORMAT_B8G8R8A8_UNORM: return eBgra8Unorm;
		case VK_FORMAT_B8G8R8A8_SNORM: return eBgra8Snorm;
		case VK_FORMAT_B8G8R8A8_USCALED: return eBgra8Uscaled;
		case VK_FORMAT_B8G8R8A8_SSCALED: return eBgra8Sscaled;
		case VK_FORMAT_B8G8R8A8_UINT: return eBgra8Uint;
		case VK_FORMAT_B8G8R8A8_SINT: return eBgra8Sint;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return eAbgr8UnormPack32;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return eAbgr8SnormPack32;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return eAbgr8UscaledPack32;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return eAbgr8SscaledPack32;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: return eAbgr8UintPack32;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: return eAbgr8SintPack32;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return eAbgr8SrgbPack32;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return eA2Rgb10UnormPack32;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return eA2Rgb10SnormPack32;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return eA2Rgb10UscaledPack32;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return eA2Rgb10SscaledPack32;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: return eA2Rgb10UintPack32;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: return eA2Rgb10SintPack32;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return eA2Bgr10UnormPack32;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return eA2Bgr10SnormPack32;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return eA2Bgr10UscaledPack32;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return eA2Bgr10SscaledPack32;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: return eA2Bgr10UintPack32;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: return eA2Bgr10SintPack32;
		case VK_FORMAT_R16_UNORM: return eR16Unorm;
		case VK_FORMAT_R16_SNORM: return eR16Snorm;
		case VK_FORMAT_R16_USCALED: return eR16Uscaled;
		case VK_FORMAT_R16_SSCALED: return eR16Sscaled;
		case VK_FORMAT_R16_UINT: return eR16Uint;
		case VK_FORMAT_R16_SINT: return eR16Sint;
		case VK_FORMAT_R16_SFLOAT: return eR16Sfloat;
		case VK_FORMAT_R16G16_UNORM: return eRg16Unorm;
		case VK_FORMAT_R16G16_SNORM: return eRg16Snorm;
		case VK_FORMAT_R16G16_USCALED: return eRg16Uscaled;
		case VK_FORMAT_R16G16_SSCALED: return eRg16Sscaled;
		case VK_FORMAT_R16G16_UINT: return eRg16Uint;
		case VK_FORMAT_R16G16_SINT: return eRg16Sint;
		case VK_FORMAT_R16G16_SFLOAT: return eRg16Sfloat;
		case VK_FORMAT_R16G16B16_UNORM: return eRgb16Unorm;
		case VK_FORMAT_R16G16B16_SNORM: return eRgb16Snorm;
		case VK_FORMAT_R16G16B16_USCALED: return eRgb16Uscaled;
		case VK_FORMAT_R16G16B16_SSCALED: return eRgb16Sscaled;
		case VK_FORMAT_R16G16B16_UINT: return eRgb16Uint;
		case VK_FORMAT_R16G16B16_SINT: return eRgb16Sint;
		case VK_FORMAT_R16G16B16_SFLOAT: return eRgb16Sfloat;
		case VK_FORMAT_R16G16B16A16_UNORM: return eRgba16Unorm;
		case VK_FORMAT_R16G16B16A16_SNORM: return eRgba16Snorm;
		case VK_FORMAT_R16G16B16A16_USCALED: return eRgba16Uscaled;
		case VK_FORMAT_R16G16B16A16_SSCALED: return eRgba16Sscaled;
		case VK_FORMAT_R16G16B16A16_UINT: return eRgba16Uint;
		case VK_FORMAT_R16G16B16A16_SINT: return eRgba16Sint;
		case VK_FORMAT_R16G16B16A16_SFLOAT: return eRgba16Sfloat;
		case VK_FORMAT_R32_UINT: return eR32Uint;
		case VK_FORMAT_R32_SINT: return eR32Sint;
		case VK_FORMAT_R32_SFLOAT: return eR32Sfloat;
		case VK_FORMAT_R32G32_UINT: return eRg32Uint;
		case VK_FORMAT_R32G32_SINT: return eRg32Sint;
		case VK_FORMAT_R32G32_SFLOAT: return eRg32Sfloat;
		case VK_FORMAT_R32G32B32_UINT: return eRgb32Uint;
		case VK_FORMAT_R32G32B32_SINT: return eRgb32Sint;
		case VK_FORMAT_R32G32B32_SFLOAT: return eRgb32Sfloat;
		case VK_FORMAT_R32G32B32A32_UINT: return eRgba32Uint;
		case VK_FORMAT_R32G32B32A32_SINT: return eRgba32Sint;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return eRgba32Sfloat;
		case VK_FORMAT_R64_UINT: return eR64Uint;
		case VK_FORMAT_R64_SINT: return eR64Sint;
		case VK_FORMAT_R64_SFLOAT: return eR64Sfloat;
		case VK_FORMAT_R64G64_UINT: return eRg64Uint;
		case VK_FORMAT_R64G64_SINT: return eRg64Sint;
		case VK_FORMAT_R64G64_SFLOAT: return eRg64Sfloat;
		case VK_FORMAT_R64G64B64_UINT: return eRgb64Uint;
		case VK_FORMAT_R64G64B64_SINT: return eRgb64Sint;
		case VK_FORMAT_R64G64B64_SFLOAT: return eRgb64Sfloat;
		case VK_FORMAT_R64G64B64A64_UINT: return eRgba64Uint;
		case VK_FORMAT_R64G64B64A64_SINT: return eRgba64Sint;
		case VK_FORMAT_R64G64B64A64_SFLOAT: return eRgba64Sfloat;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return eB10Gr11UfloatPack32;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return eE5B9G9R9UfloatPack32;
		case VK_FORMAT_D16_UNORM: return eD16Unorm;
		case VK_FORMAT_X8_D24_UNORM_PACK32: return eX8D24UnormPack32;
		case VK_FORMAT_D32_SFLOAT: return eDepth32Sfloat;
		case VK_FORMAT_S8_UINT: return eS8Uint;
		case VK_FORMAT_D16_UNORM_S8_UINT: return eDepth16UnormStencil8Uint;
		case VK_FORMAT_D24_UNORM_S8_UINT: return eDepth24UnormStencil8Uint;
		case VK_FORMAT_D32_SFLOAT_S8_UINT: return eDepth32SfloatStencil8Uint;
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return eBc1RgbUnormBlock;
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return eBc1RgbSrgbBlock;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return eBc1RgbaUnormBlock;
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return eBc1RgbaSrgbBlock;
		case VK_FORMAT_BC2_UNORM_BLOCK: return eBc2UnormBlock;
		case VK_FORMAT_BC2_SRGB_BLOCK: return eBc2SrgbBlock;
		case VK_FORMAT_BC3_UNORM_BLOCK: return eBc3UnormBlock;
		case VK_FORMAT_BC3_SRGB_BLOCK: return eBc3SrgbBlock;
		case VK_FORMAT_BC4_UNORM_BLOCK: return eBc4UnormBlock;
		case VK_FORMAT_BC4_SNORM_BLOCK: return eBc4SnormBlock;
		case VK_FORMAT_BC5_UNORM_BLOCK: return eBc5UnormBlock;
		case VK_FORMAT_BC5_SNORM_BLOCK: return eBc5SnormBlock;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK: return eBc6HUfloatBlock;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK: return eBc6HSfloatBlock;
		case VK_FORMAT_BC7_UNORM_BLOCK: return eBc7UnormBlock;
		case VK_FORMAT_BC7_SRGB_BLOCK: return eBc7SrgbBlock;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return eEtc2Rgb8UnormBlock;
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return eEtc2Rgb8SrgbBlock;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return eEtc2Rgb8A1UnormBlock;
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return eEtc2Rgb8A1SrgbBlock;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return eEtc2Rgba8UnormBlock;
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return eEtc2Rgba8SrgbBlock;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK: return eEacR11UnormBlock;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK: return eEacR11SnormBlock;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return eEacR11G11UnormBlock;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return eEacR11G11SnormBlock;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return eAstc4x4UnormBlock;
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return eAstc4x4SrgbBlock;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return eAstc5x4UnormBlock;
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return eAstc5x4SrgbBlock;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return eAstc5x5UnormBlock;
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return eAstc5x5SrgbBlock;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return eAstc6x5UnormBlock;
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return eAstc6x5SrgbBlock;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return eAstc6x6UnormBlock;
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return eAstc6x6SrgbBlock;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return eAstc8x5UnormBlock;
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return eAstc8x5SrgbBlock;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return eAstc8x6UnormBlock;
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return eAstc8x6SrgbBlock;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return eAstc8x8UnormBlock;
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return eAstc8x8SrgbBlock;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return eAstc10x5UnormBlock;
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return eAstc10x5SrgbBlock;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return eAstc10x6UnormBlock;
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return eAstc10x6SrgbBlock;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return eAstc10x8UnormBlock;
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return eAstc10x8SrgbBlock;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return eAstc10x10UnormBlock;
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return eAstc10x10SrgbBlock;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return eAstc12x10UnormBlock;
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return eAstc12x10SrgbBlock;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return eAstc12x12UnormBlock;
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return eAstc12x12SrgbBlock;
		}
		return eUndefined;
	}
}
