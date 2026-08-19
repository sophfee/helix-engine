// ReSharper disable CppClangTidyBugproneMacroParentheses
// ReSharper disable CppClangTidyClangDiagnosticPadded
#pragma once

//
// The rendering api is built around a mix of influences, primarily Vulkan and WGPU (specifically wgpu.rs)
//

#include <vulkan/vulkan.hpp>

#include "math.hpp"
#include "../types.hpp"
#include "engine/disposable.hpp"
#include "engine/rid.hpp"

class IWindow;
class GLFW3Window;
class SDL2Window;
class Window;
class GraphicsDriver;
class GraphicsBackend;

enum class RenderingApiBackend : u8 {
	eNone = 0,
	eVulkan = 1,
	eDirectX12 = 2,
	eOpenGLModern = 3, //< 4.6
	eOpenGLES = 4 //< 3.3 compatability
};

namespace gfx {
	enum class Access : u32 {
		eNone = 0,
		eIndirectCommandRead = 1 << 0,
		eIndexRead = 1 << 1,
		eVertexAttributeRead = 1 << 2,
		eUniformRead = 1 << 3,
		eInputAttachmentRead = 1 << 4,
		eShaderRead = 1 << 5,
		eShaderWrite = 1 << 6,
		eColorAttachmentRead = 1 << 7,
		eColorAttachmentWrite = 1 << 8,
		eDepthStencilAttachmentRead = 1 << 9,
		eDepthStencilAttachmentWrite = 1 << 10,
		eTransferRead = 1 << 11,
		eTransferWrite = 1 << 12,
		eHostRead = 1 << 13,
		eHostWrite = 1 << 14,
		eMemoryRead = 1 << 15,
		eMemoryWrite = 1 << 16
	};
	enum class AddressMode : u8 {
		eRepeat = 0,
		eMirroredRepeat = 1,
		eClampToEdge = 2,
		eClampToBorder = 3,
		eMirrorClampToEdge = 4
	};
	enum class AllocationHint : u16 {
		eNone = 0,
		eDedicated = 1 << 0,
		eNeverAllocate = 1 << 1,
		eMapped = 1 << 2,
		eUserDataCopyString = 1 << 3,
		eUpperAddress = 1 << 4,
		eDontBind = 1 << 5,
		eWithinBudget = 1 << 6,
		eCanAlias = 1 << 7,
		eHostSequentialWrite = 1 << 8,
		eAllowTransferInstead = 1 << 9,
		eStrategyMinTime = 1 << 10,
		eStrategyMinOffset = 1 << 11,
		eStrategyBestFit = 1 << 12,
		eStrategyFirstFit = 1 << 13,
		eStrategyMask = 1 << 14,
	};
	
	/**
	 * \brief Specifies the category of data contained within a given image.
	 * \note There are only 3 options here, but many more exist in VK and D3D12, I just don't feel they currently have any use within the engine.
	 */
	enum class Aspect : u8 {
		eNone = 0,
		eColor = 1 << 0,
		eDepth = 1 << 1,
		eStencil = 1 << 2
	};
	enum class BindingType {
		eUniformBuffer,
		eStorageBuffer,
		eSampler,
		eSampledImage,
		eStorageImage,
		eImageSampler
	};
	enum class BufferUsage : u32 {
		eNone = 0,
		eTransferSrc = 1 << 0,
		eTransferDst = 1 << 1,
		eUniformTexelBuffer = 1 << 2,
		eStorageTexelBuffer = 1 << 3,
		eUniform = 1 << 4,
		eStorage = 1 << 5,
		eIndex = 1 << 6,
		eVertex = 1 << 7,
		eIndirectBuffer = 1 << 8,
		eShaderDeviceAddress = 1 << 9,
		eVideoDecodeSrc = 1 << 10,
		eVideoDecodeDst = 1 << 11,
		eTransformFeedbackBuffer = 1 << 12,
		eTransformFeedbackCounterBuffer = 1 << 13,
		eConditionalRendering = 1 << 14,
		eAccelerationStructureBuildInputReadOnly = 1 << 15,
		eAccelerationStructureStorage = 1 << 16,
		eShaderBindingTable = 1 << 17,
		eRayTracing = 1 << 18,
		eVideoEncodeDst = 1 << 19,
		eVideoEncodeSrc = 1 << 20,
		eSamplerDescriptorBuffer = 1 << 21,
		eResourceDescriptorBuffer = 1 << 22,
		ePushDescriptorsDescriptorBuffer = 1 << 23,
		eMicromapBuildInputReadOnly = 1 << 24,
		eMicromapStorage = 1 << 25
	};
	enum class ColorSpace : u8 {
		eUndefined,
		eSrgbNonLinear // hdr can come later or wtv
	};
	enum class CompareOp : u8 {
		eNever = 0,
		eLess,
		eEqual,
		eLessOrEqual,
		eGreater,
		eNotEqual,
		eGreaterOrEqual,
		eAlways
	};
	enum class CompositeAlpha : u8 {
		eOpaque,
		ePreMultiplied,
		ePostMultiplied,
		eInherit
	};
	enum class CullMode {
		eNone,
		eFront,
		eBack,
		eFrontAndBack
	};
	enum class DynamicState {
		eViewport,
		eScissor,
		eLineWidth,
		eDepthBias,
		eBlendConstants,
		eDepthBounds,
		eStencilCompareMask,
		eStencilWriteMask,
		eStencilReference,
		eCullMode,
		eFrontFace,
		ePrimitiveTopology,
		eViewportWithCount,
		eScissorWithCount,
		eVertexInputBindingStride,
		eDepthTestEnable,
		eDepthWriteEnable,
		eDepthCompareOp,
		eDepthBoundsTestEnable,
		eStencilTestEnable,
		eStencilOp,
		eRasterizerDiscardEnable,
		eDepthBiasEnable,
		ePrimitiveRestartEnable,
	};
	enum class Filter : u8 {
		eNearest = 0,
		eLinear = 1
	};
	enum class Format : u8 {
		eUndefined,
		eRg4UnormPack8,
		eRgba4UnormPack16,
		eBgra4UnormPack16,
		eR5G6B5UnormPack16,
		eB5G6R5UnormPack16,
		eRgb5A1UnormPack16,
		eBgr5A1UnormPack16,
		eA1Rgb5UnormPack16,
		eR8Unorm,
		eR8Snorm,
		eR8Uscaled,
		eR8Sscaled,
		eR8Uint,
		eR8Sint,
		eR8Srgb,
		eRg8Unorm,
		eRg8Snorm,
		eRg8Uscaled,
		eRg8Sscaled,
		eRg8Uint,
		eRg8Sint,
		eRg8Srgb,
		eRgb8Unorm,
		eRgb8Snorm,
		eRgb8Uscaled,
		eRgb8Sscaled,
		eRgb8Uint,
		eRgb8Sint,
		eRgb8Srgb,
		eBgr8Unorm,
		eBgr8Snorm,
		eBgr8Uscaled,
		eBgr8Sscaled,
		eBgr8Uint,
		eBgr8Sint,
		eBgr8Srgb,
		eRgba8Unorm,
		eRgba8Snorm,
		eRgba8Uscaled,
		eRgba8Sscaled,
		eRgba8Uint,
		eRgba8Sint,
		eRgba8Srgb,
		eBgra8Unorm,
		eBgra8Snorm,
		eBgra8Uscaled,
		eBgra8Sscaled,
		eBgra8Uint,
		eBgra8Sint,
		eBgra8Srgb,
		eAbgr8UnormPack32,
		eAbgr8SnormPack32,
		eAbgr8UscaledPack32,
		eAbgr8SscaledPack32,
		eAbgr8UintPack32,
		eAbgr8SintPack32,
		eAbgr8SrgbPack32,
		eA2Rgb10UnormPack32,
		eA2Rgb10SnormPack32,
		eA2Rgb10UscaledPack32,
		eA2Rgb10SscaledPack32,
		eA2Rgb10UintPack32,
		eA2Rgb10SintPack32,
		eA2Bgr10UnormPack32,
		eA2Bgr10SnormPack32,
		eA2Bgr10UscaledPack32,
		eA2Bgr10SscaledPack32,
		eA2Bgr10UintPack32,
		eA2Bgr10SintPack32,
		eR16Unorm,
		eR16Snorm,
		eR16Uscaled,
		eR16Sscaled,
		eR16Uint,
		eR16Sint,
		eR16Sfloat,
		eRg16Unorm,
		eRg16Snorm,
		eRg16Uscaled,
		eRg16Sscaled,
		eRg16Uint,
		eRg16Sint,
		eRg16Sfloat,
		eRgb16Unorm,
		eRgb16Snorm,
		eRgb16Uscaled,
		eRgb16Sscaled,
		eRgb16Uint,
		eRgb16Sint,
		eRgb16Sfloat,
		eRgba16Unorm,
		eRgba16Snorm,
		eRgba16Uscaled,
		eRgba16Sscaled,
		eRgba16Uint,
		eRgba16Sint,
		eRgba16Sfloat,
		eR32Uint,
		eR32Sint,
		eR32Sfloat,
		eRg32Uint,
		eRg32Sint,
		eRg32Sfloat,
		eRgb32Uint,
		eRgb32Sint,
		eRgb32Sfloat,
		eRgba32Uint,
		eRgba32Sint,
		eRgba32Sfloat,
		eR64Uint,
		eR64Sint,
		eR64Sfloat,
		eRg64Uint,
		eRg64Sint,
		eRg64Sfloat,
		eRgb64Uint,
		eRgb64Sint,
		eRgb64Sfloat,
		eRgba64Uint,
		eRgba64Sint,
		eRgba64Sfloat,
		eB10Gr11UfloatPack32,
		eE5B9G9R9UfloatPack32,
		eD16Unorm,
		eX8D24UnormPack32,
		eDepth32Sfloat,
		eS8Uint,
		eDepth16UnormStencil8Uint,
		eDepth24UnormStencil8Uint,
		eDepth32SfloatStencil8Uint,
		eBc1RgbUnormBlock,
		eBc1RgbSrgbBlock,
		eBc1RgbaUnormBlock,
		eBc1RgbaSrgbBlock,
		eBc2UnormBlock,
		eBc2SrgbBlock,
		eBc3UnormBlock,
		eBc3SrgbBlock,
		eBc4UnormBlock,
		eBc4SnormBlock,
		eBc5UnormBlock,
		eBc5SnormBlock,
		eBc6HUfloatBlock,
		eBc6HSfloatBlock,
		eBc7UnormBlock,
		eBc7SrgbBlock,
		eEtc2Rgb8UnormBlock,
		eEtc2Rgb8SrgbBlock,
		eEtc2Rgb8A1UnormBlock,
		eEtc2Rgb8A1SrgbBlock,
		eEtc2Rgba8UnormBlock,
		eEtc2Rgba8SrgbBlock,
		eEacR11UnormBlock,
		eEacR11SnormBlock,
		eEacR11G11UnormBlock,
		eEacR11G11SnormBlock,
		eAstc4x4UnormBlock,
		eAstc4x4SrgbBlock,
		eAstc5x4UnormBlock,
		eAstc5x4SrgbBlock,
		eAstc5x5UnormBlock,
		eAstc5x5SrgbBlock,
		eAstc6x5UnormBlock,
		eAstc6x5SrgbBlock,
		eAstc6x6UnormBlock,
		eAstc6x6SrgbBlock,
		eAstc8x5UnormBlock,
		eAstc8x5SrgbBlock,
		eAstc8x6UnormBlock,
		eAstc8x6SrgbBlock,
		eAstc8x8UnormBlock,
		eAstc8x8SrgbBlock,
		eAstc10x5UnormBlock,
		eAstc10x5SrgbBlock,
		eAstc10x6UnormBlock,
		eAstc10x6SrgbBlock,
		eAstc10x8UnormBlock,
		eAstc10x8SrgbBlock,
		eAstc10x10UnormBlock,
		eAstc10x10SrgbBlock,
		eAstc12x10UnormBlock,
		eAstc12x10SrgbBlock,
		eAstc12x12UnormBlock,
		eAstc12x12SrgbBlock,
		eGbgr8422Unorm,
		eBgrg8422Unorm,
		eGbr83Plane420Unorm,
		eGbr82Plane420Unorm,
		eGbr83Plane422Unorm,
		eGbr82Plane422Unorm,
		eGbr83Plane444Unorm,
		eR10X6UnormPack16,
		eR10X6G10X6Unorm2Pack16,
		eR10X6G10X6B10X6A10X6Unorm4Pack16,
		eG10X6B10X6G10X6R10X6422Unorm4Pack16,
		eB10X6G10X6R10X6G10X6422Unorm4Pack16,
		eG10X6B10X6R10X63Plane420Unorm3Pack16,
		eG10X6B10X6R10X62Plane420Unorm3Pack16,
		eG10X6B10X6R10X63Plane422Unorm3Pack16,
		eG10X6B10X6R10X62Plane422Unorm3Pack16,
		eG10X6B10X6R10X63Plane444Unorm3Pack16,
		eR12X4UnormPack16,
		eR12X4G12X4Unorm2Pack16,
		eR12X4G12X4B12X4A12X4Unorm4Pack16,
		eG12X4B12X4G12X4R12X4422Unorm4Pack16,
		eB12X4G12X4R12X4G12X4422Unorm4Pack16,
		eG12X4B12X4R12X43Plane420Unorm3Pack16,
		eG12X4B12X4R12X42Plane420Unorm3Pack16,
		eG12X4B12X4R12X43Plane422Unorm3Pack16,
		eG12X4B12X4R12X42Plane422Unorm3Pack16,
		eG12X4B12X4R12X43Plane444Unorm3Pack16,
		eGbgr16422Unorm,
		eBgrg16422Unorm,
		eGbr163Plane420Unorm,
		eGbr162Plane420Unorm,
		eGbr163Plane422Unorm,
		eGbr162Plane422Unorm,
		eGbr163Plane444Unorm,
		eGbr82Plane444Unorm,
		eG10X6B10X6R10X62Plane444Unorm3Pack16,
		eG12X4B12X4R12X42Plane444Unorm3Pack16,
		eGbr162Plane444Unorm,
		eArgb4UnormPack16,
		eAbgr4UnormPack16,
		eAstc4x4SfloatBlock,
		eAstc5x4SfloatBlock,
		eAstc5x5SfloatBlock,
		eAstc6x5SfloatBlock,
		eAstc6x6SfloatBlock,
		eAstc8x5SfloatBlock,
		eAstc8x6SfloatBlock,
		eAstc8x8SfloatBlock,
		eAstc10x5SfloatBlock,
		eAstc10x6SfloatBlock,
		eAstc10x8SfloatBlock,
		eAstc10x10SfloatBlock,
		eAstc12x10SfloatBlock,
		eAstc12x12SfloatBlock,
		eA1Bgr5UnormPack16,
		eA8Unorm,
		ePvrtc12BppUnormBlockIMG,
		ePvrtc14BppUnormBlockIMG,
		ePvrtc22BppUnormBlockIMG,
		ePvrtc24BppUnormBlockIMG,
		ePvrtc12BppSrgbBlockIMG,
		ePvrtc14BppSrgbBlockIMG,
		ePvrtc22BppSrgbBlockIMG,
		ePvrtc24BppSrgbBlockIMG,
		eRg16Sfixed5NV,
		eRg16S105NV
	};
	enum class FrontFace {
		eCounterClockwise,
		eClockwise
	};
	enum class ImageLayout : u8 {
		eUndefined,
		eGeneral,
		eAttachmentOptimal,
		eColorAttachmentOptimal,
		eDepthStencilAttachmentOptimal,
		eDepthStencilReadOnlyOptimal,
		eShaderReadOnlyOptimal,
		eTransferSrcOptimal,
		eTransferDstOptimal,
		ePreinitialized,
		eReadOnly,
		ePresent
	};
	enum class ImageType : u8 {
		e1D = 0,
		e2D = 1,
		e3D = 2
	};
	enum class ImageUsage : u32 {
		eNone = 0,
		eTransferSrc = 1 << 0,
		eTransferDst = 1 << 1,
		eSampled = 1 << 2,
		eStorage = 1 << 3,
		eColorAttachment = 1 << 4,
		eDepthStencilAttachment = 1 << 5,
		eTransientAttachment = 1 << 6,
		eInputAttachment = 1 << 7,
		eHostTransfer = 1 << 8
	};
	enum class ImageViewType : u8 {
		e1D,
		e2D,
		e3D,
		eCube,
		e1DArray,
		e2DArray,
		eCubeArray
	};
	enum class IndexType : u8 {
		eNone = 0,
		eUInt8,
		eUInt16,
		eUInt32
	};
	enum class InputRate {
		eVertex,
		eInstance
	};
	enum class LoadOp : u8 {
		eNone = 0,
		eLoad = 1,
		eClear = 2,
		eDontCare = 3
	};
	enum class MemoryUsage : u8 {
		eUnknown = 0,
		eGpu,
		eCpu,
		eCpuToGpu,
		eGpuToCpu,
		eCpuCopy,
		eGpuLazilyAllocated,
		eAuto,
		ePreferDevice,
		ePreferHost
	};
	enum class MipmapFilter : u8 {
		eNearest = 0,
		eLinear = 1
	};
	enum class PipelineBindPoint : u8 {
		eGraphics = 0,
		eCompute = 1
	};
	enum class PipelineStage {
		eNone,
		eTopOfPipe,
		eBottomOfPipe,
		eEarlyFragmentTests,
		eLateFragmentTests,
		eDrawIndirect,
		eVertexInput,
		eVertexShader,
		eTessellationControlShader,
		eTessellationEvaluationShader,
		eGeometryShader,
		eFragmentShader,
		eColorAttachmentOutput,
		eComputeShader,
		eAllTransfer,
		eTransfer,
		eHost,
		eAllGraphics,
		eAllCommands,
		eCopy,
		eResolve,
		eBlit,
		eClear,
		eIndexInput,
		eVertexAttributeInput,
		ePreRasterizationShaders,
		eTransformFeedback,
		eConditionalRendering,
		eCommandPreprocess,
		eFragmentShadingRateAttachment,
		eShadingRateImage,
		eAccelerationStructureBuild,
		eRayTracingShader,
		eFragmentDensityProcess,
		eTaskShader,
		eMeshShader
	};
	enum class PolygonMode {
		eFill,
		eLine,
		ePoint
	};
	enum class PrimitiveTopology {
		ePointList,
		eLineList,
		eLineStrip,
		eTriangleList,
		eTriangleStrip,
		eTriangleFan,
		eLineListWithAdjacency,
		eLineStripWithAdjacency,
		eTriangleListWithAdjacency,
		eTriangleStripWithAdjacency,
		ePatchList
	};
	enum class PresentMethod : u8 {
		eImmediate = 0,
		eMailbox = 1,
		eFifo = 2,
		eFifoRelaxed = 3
	};
	enum class QueueFamilyType {
		eGraphics = 0,
		eCompute = 1,
		eTransfer = 2,
		eSparseBinding = 3
	};
	enum class SampleCount : u8 {
		e1 = 1,
		e2 = 2,
		e4 = 4,
		e8 = 8,
		e16 = 16,
		e32 = 32,
		e64 = 64
	};
	enum class SemaphoreType {
		eBinary,
		eTimeline
	};
	enum class ShaderStage {
		eVertex = 1ull << 0,
		eFragment = 1ull << 1,
		eGeometry = 1ull << 2,
		eTesselationControl = 1ull << 3,
		eTesselationEvaluation = 1ull << 4,
		eCompute = 1ull << 5,
		eMesh = 1ull << 6,
		eTask = 1ull << 7,
		eRaygen = 1ull << 8,
		eAnyHit = 1ull << 9,
		eClosestHit = 1ull << 10,
		eMiss = 1ull << 11,
		eIntersection = 1ull << 12,
		eCallable = 1ull << 13,
		eAll = eVertex | eFragment | eGeometry | eTesselationControl | eTesselationEvaluation | eCompute | eMesh | eTask | eRaygen | eAnyHit | eClosestHit | eMiss | eIntersection | eCallable,
		eAllGraphics = eVertex | eFragment | eGeometry | eTesselationControl | eTesselationEvaluation | eMesh | eTask
	};
	enum class StencilOp {
		eKeep = 0,
		eZero = 1,
		eReplace = 2,
		eIncrementAndClamp = 3,
		eDecrementAndClamp = 4,
		eInvert = 5,
		eIncrementAndWrap = 6,
		eDecrementAndWrap = 7
	};
	enum class StoreOp : u8 {
		eNone = 0,
		eStore = 1,
		eDontCare = 2
	};
	enum class Swizzle : u8 {
		eIdentity = 0,
		eZero = 1,
		eOne = 2,
		eR = 3,
		eRed = 3,
		eG = 4,
		eGreen = 4,
		eB = 5,
		eBlue = 5,
		eA = 6,
		eAlpha = 6
	};
	struct Viewport {
		f32 x;
		f32 y;
		f32 width;
		f32 height;
		f32 min_depth;
		f32 max_depth;
	};
	struct Offset2D {
		i32 x, y;
	};
	struct Extent2D {
		u32 width, height;
	};
	struct Rect2D {
		Offset2D offset;
		Extent2D extent;
	};
	
	struct BufferDescriptor {
		Optional<String> label;
		u64 size = 0;
		BitFlag<BufferUsage> usage = BufferUsage::eNone;
		Optional<MemoryUsage> memory_usage;
		Optional<AllocationHint> allocation_hints;
	};
	
	struct SwizzleDescriptor {
		union {
			Swizzle r = Swizzle::eIdentity;
			Swizzle red;
			Swizzle x;
		};
		union {
			Swizzle g = Swizzle::eIdentity;
			Swizzle green;
			Swizzle y;
		};
		union {
			Swizzle b = Swizzle::eIdentity;
			Swizzle blue;
			Swizzle z;
		};
		union {
			Swizzle a = Swizzle::eIdentity;
			Swizzle alpha;
			Swizzle w;
		};
	};
	struct ImageDescriptor {
		Optional<String> label;
		Format format = Format::eUndefined;
		ImageType type = ImageType::e2D;
		ImageUsage usage = ImageUsage::eNone;
		Optional<SampleCount> samples;
		Optional<MemoryUsage> memory_usage;
		Optional<AllocationHint> allocation_hints;
		Optional<ImageLayout> initial_layout;
		uvec3 size = uvec3(1u);
		u32 array_layers = 1u;
		u32 mip_levels = 1u;
	};
	struct ImageSubresourceDescriptor {
		BitFlag<Aspect> aspect_mask;
		u32 base_mip_level = 0;
		u32 level_count = 1;
		u32 base_array_layer = 0;
		u32 layer_count = 1;
	};
	struct ImageViewDescriptor {
		Optional<String> label;
		RID image;
		Optional<ImageViewType> type;
		Optional<Format> format;
		Optional<Swizzle> swizzle;
		Optional<ImageUsage> usage;
		Optional<ImageSubresourceDescriptor> subresource;
	};
	struct SamplerDescriptor {
		Optional<String> label;
		Optional<Filter> mag_filter;
		Optional<Filter> min_filter;
		Optional<MipmapFilter> mipmap_mode;
		Optional<AddressMode> address_mode_u;
		Optional<AddressMode> address_mode_v;
		Optional<AddressMode> address_mode_w;
		Optional<CompareOp> compare_op;
		bool enable_anisotropy = false;
		f32 mip_lod_bias = 0.0f;
		f32 max_anisotropy = 1.0f;
		f32 min_lod = 0.0f;
		f32 max_lod = 1.0f;
	};
	struct SurfaceDescriptor {
		Optional<String> label;
		Optional<vec2> size;
		Optional<u32> minimum_images;
		Optional<Format> format;
		Optional<ImageUsage> usage;
		Optional<PresentMethod> present_method;
		Optional<ColorSpace> color_space;
		Optional<CompositeAlpha> composite_alpha;
	};
	struct SpirvDescriptor {
		Optional<String> label;
		size_t code_size;
		const uint32_t *code;
	};
	struct BindGroupLayoutEntryDescriptor {
		u32 binding;
		BitFlag<ShaderStage> visibility;
		BindingType type;
		Optional<u32> count;
	};
	struct BindGroupLayoutDescriptor {
		Optional<String> label;
		Vector<BindGroupLayoutEntryDescriptor> entries;
	};
	struct BindingResource {
		struct BufferBinding {
			RID buffer;
			u64 offset;
			u64 size;
		};
	
		struct SamplerBinding {
			RID sampler;
		};
	
		struct ImageBinding {
			RID image_view;
			ImageLayout layout;
		};
	
		struct CombinedImageSampler {
			RID sampler;
			RID image_view;
			ImageLayout layout;
		};
		
		BindingResource(const RID image_view, const ImageLayout layout) : binding(ImageBinding{ image_view, layout }), type(BindingType::eSampledImage) {}
		BindingResource(const RID sampler) : binding(SamplerBinding{ sampler }), type(BindingType::eSampler) {}
		BindingResource(const RID buffer, const u64 offset, const u64 size) : binding(BufferBinding{ buffer, offset, size }), type(BindingType::eUniformBuffer) {}
		BindingResource(const RID sampler, const RID image_view, const ImageLayout layout) : binding(CombinedImageSampler{ sampler, image_view, layout }), type(BindingType::eImageSampler) {}
		BindingResource(Vector<BufferBinding> buffers) : binding(buffers), type(BindingType::eUniformBuffer) {}
		BindingResource(Vector<SamplerBinding> samplers) : binding(samplers), type(BindingType::eSampler) {}
		BindingResource(Vector<ImageBinding> images) : binding(images), type(BindingType::eSampledImage) {}
		BindingResource(Vector<CombinedImageSampler> combined) : binding(combined), type(BindingType::eImageSampler) {}
	
		std::variant<BufferBinding, Vector<BufferBinding>,
			SamplerBinding, Vector<SamplerBinding>,
			ImageBinding, Vector<ImageBinding>,
			CombinedImageSampler, Vector<CombinedImageSampler>
		> binding;
		BindingType type;
	};
	
	struct BindGroupEntryDescriptor {
		u32 binding;
		BindingResource resource;
	};
	struct BindGroupDescriptor {
		Optional<String> label;
		RID layout;
		Vector<BindGroupEntryDescriptor> entries;
	};
	struct PushConstantRangeDescriptor {
		ShaderStage visibility;
		u32 offset;
		u32 size;
	};
	struct VertexInputBindingDescriptor {
		u32 binding;
		u32 stride;
		InputRate input_rate = InputRate::eVertex;
	};
	struct VertexInputAttributeDescriptor {
		u32 location;
		u32 binding;
		Format format;
		u32 offset;
	};
	struct InputAssemblyDescriptor {
		PrimitiveTopology primitive_topology;
		bool primitive_restart_enable = false;
	};
	struct VertexInputDescriptor {
		Vector<VertexInputBindingDescriptor> bindings;
		Vector<VertexInputAttributeDescriptor> attributes;
	};
	struct VertexBufferDescriptor {
		RID buffer;
		u32 binding;
		u64 offset;
	};
	struct IndexBufferDescriptor {
		RID buffer;
		IndexType index_type;
		u64 offset;
	};
	struct PipelineLayoutDescriptor {
		Vector<RID> bind_group_layouts;
		Vector<PushConstantRangeDescriptor> push_constants;
	};
	struct GraphicsPipelineStageDescriptor {
		RID shader;
		ShaderStage stage;
		std::string entry_point = "main";
	};
	struct RasterizationStateDescriptor {
		bool depth_clamp_enable = false;
		bool rasterizer_discard_enable = false;
		PolygonMode polygon_mode = PolygonMode::eFill;
		CullMode cull_mode = CullMode::eBack;
		FrontFace front_face = FrontFace::eCounterClockwise;
		bool depth_bias_enable = false;
		f32 depth_bias_constant_factor = 0.0f;
		f32 depth_bias_clamp = 0.0f;
		f32 depth_bias_slope_factor = 0.0f;
		f32 line_width = 1.0f;
	};
	/**
	 * \note Not yet fully implemented
	 */
	struct ColorBlendStateDescriptor {
		bool blend_enable = false;
	};
	struct ViewportStateDescriptor {
		Vector<Viewport> viewports;
		Vector<Rect2D> scissors;
	};
	struct PipelineRenderingDescriptor {
		Vector<Format> color_formats;
		Optional<Format> depth_format;
		Optional<Format> stencil_format;
	};
	struct MultisampleDescriptor {
		SampleCount rasterization_samples = SampleCount::e1;
		bool sample_shading_enable = false;
		f32 min_sample_shading = 1.0f;
	};

	struct StencilOpDescriptor {
		StencilOp fail_op = StencilOp::eKeep;
		StencilOp pass_op = StencilOp::eKeep;
		StencilOp depth_fail_op = StencilOp::eKeep;
		CompareOp compare_op = CompareOp::eAlways;
		u32 compare_mask = 0;
		u32 write_mask = 0;
		u32 reference = 0;
	};

	struct DepthStencilDescriptor {
		bool depth_test;
		bool depth_write;
		bool depth_bounds_test;
		bool stencil_test;
		CompareOp depth_compare_op;
		StencilOpDescriptor front;
		StencilOpDescriptor back;
		float min_depth_bounds;
		float max_depth_bounds;
	};
	struct GraphicsPipelineDescriptor {
		Optional<String> label = std::nullopt;
		RID layout;
		Vector<GraphicsPipelineStageDescriptor> stages;
		PipelineRenderingDescriptor rendering;
		VertexInputDescriptor vertex_input;
		InputAssemblyDescriptor input_assembly;
		ViewportStateDescriptor viewport;
		RasterizationStateDescriptor rasterization;
		MultisampleDescriptor multisample;
		DepthStencilDescriptor depth_stencil;
		ColorBlendStateDescriptor blend;
		Vector<DynamicState> dynamic_states;
	};
	struct ImageTransitionStateDescriptor {
		ImageLayout layout;
		BitFlag<Access> access;
		BitFlag<PipelineStage> stage;
		Optional<QueueFamilyType> queue_family = std::nullopt;
	};
	struct ImageTransitionDescriptor {
		RID image;
		ImageTransitionStateDescriptor src;
		ImageTransitionStateDescriptor dst;
		Optional<ImageSubresourceDescriptor> subresource;
	};
	struct ClearColorValue {
		vec4 float32;
		ivec4 int32;
		uvec4 uint32;
	};
	struct ClearDepthStencilValue {
		f32 depth;
		u32 stencil;
	};
	struct RenderingAttachmentDescriptor {
		RID image_view;
		ImageLayout layout;
		LoadOp load_op = LoadOp::eLoad;
		StoreOp store_op = StoreOp::eStore;
		Optional<ClearColorValue> clear_color;
		Optional<ClearDepthStencilValue> clear_depth_stencil;
	};
	struct RenderingDescriptor {
		Vector<RenderingAttachmentDescriptor> color_attachments;
		Optional<RenderingAttachmentDescriptor> depth_attachment;
		Optional<RenderingAttachmentDescriptor> stencil_attachment;
		Rect2D render_area;
		u32 layer_count = 1;
	};
	struct BindShaderDescriptor {
		RID shader;
		ShaderStage stage;
	};
}

#pragma region Enum & Descriptor Defs

//using enum gfx::Access;
//using enum gfx::AddressMode;
//using enum gfx::AllocationHint;
//using enum gfx::Aspect;
//using enum gfx::BindingType;
//using enum gfx::BufferUsage;
//using enum gfx::ColorSpace;
//using enum gfx::CompareOp;
//using enum gfx::CompositeAlpha;
//using enum gfx::CullMode;
//using enum gfx::DynamicState;
//using enum gfx::Filter;
//using enum gfx::Format;
//using enum gfx::FrontFace;
//using enum gfx::ImageLayout;
//using enum gfx::ImageType;
//using enum gfx::ImageUsage;
//using enum gfx::ImageViewType;
//using enum gfx::IndexType;
//using enum gfx::InputRate;
//using enum gfx::LoadOp;
//using enum gfx::MemoryUsage;
//using enum gfx::MipmapFilter;
//using enum gfx::PipelineBindPoint;
//using enum gfx::PipelineStage;
//using enum gfx::PolygonMode;
//using enum gfx::PrimitiveTopology;
//using enum gfx::PresentMethod;
//using enum gfx::SampleCount;
//using enum gfx::ShaderStage;
//using enum gfx::StencilOp;
//using enum gfx::StoreOp;
//using enum gfx::Swizzle;
using gfx::Viewport;
using gfx::Offset2D;
using gfx::Extent2D;
using gfx::Rect2D;
using gfx::BufferDescriptor;
using gfx::SwizzleDescriptor;
using gfx::ImageDescriptor;
using gfx::ImageSubresourceDescriptor;
using gfx::ImageViewDescriptor;
using gfx::SamplerDescriptor;
using gfx::SurfaceDescriptor;
using gfx::SpirvDescriptor;
using gfx::BindGroupLayoutEntryDescriptor;
using gfx::BindGroupLayoutDescriptor;
using gfx::BindGroupEntryDescriptor;
using gfx::BindGroupDescriptor;
using gfx::BindingResource;
using gfx::BindShaderDescriptor;
using gfx::PushConstantRangeDescriptor;
using gfx::VertexInputBindingDescriptor;
using gfx::VertexInputAttributeDescriptor;
using gfx::VertexInputDescriptor;
using gfx::InputAssemblyDescriptor;
using gfx::VertexBufferDescriptor;
using gfx::IndexBufferDescriptor;
using gfx::PipelineLayoutDescriptor;
using gfx::GraphicsPipelineStageDescriptor;
using gfx::RasterizationStateDescriptor;
using gfx::ColorBlendStateDescriptor;
using gfx::ViewportStateDescriptor;
using gfx::PipelineRenderingDescriptor;
using gfx::MultisampleDescriptor;
using gfx::StencilOpDescriptor;
using gfx::DepthStencilDescriptor;
using gfx::GraphicsPipelineDescriptor;
using gfx::ImageTransitionStateDescriptor;
using gfx::ImageTransitionDescriptor;
using gfx::ClearColorValue;
using gfx::ClearDepthStencilValue;
using gfx::RenderingAttachmentDescriptor;
using gfx::RenderingDescriptor;

template<> inline constexpr bool enable_enum_bitops<gfx::Access> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::AllocationHint> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::BufferUsage> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::ImageUsage> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::MemoryUsage> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::PipelineStage> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::QueueFamilyType> = true;
template<> inline constexpr bool enable_enum_bitops<gfx::ShaderStage> = true;

#pragma endregion 

using GpuDeviceAddress = std::uintptr_t;

class GraphicsBackend : public IDisposable {
public:
	~GraphicsBackend() override = default;
	[[nodiscard]] virtual RenderingApiBackend backend() const = 0;
	
	virtual bool initialize() = 0;
	virtual void shutdown() = 0;
	virtual void yield_for_all_commands() = 0;
	
	virtual RID fence_create(const Optional<String> &label = std::nullopt, bool signaled = false) = 0;
	virtual void fence_delete(RID fence_rid) = 0;
	[[nodiscard]] virtual [[nodiscard]] vk::Fence get_fence(RID id) const = 0;
	
	virtual RID semaphore_create(const gfx::SemaphoreType semaphore_type = gfx::SemaphoreType::eBinary, const Optional<String> &label = std::nullopt) = 0;
	virtual void semaphore_delete(RID semaphore_rid) = 0;
	[[nodiscard]] virtual vk::Semaphore get_semaphore(RID id) const = 0;
	
	[[nodiscard]] virtual RID buffer_create(const BufferDescriptor &desc) = 0;
	virtual void buffer_delete(RID buffer_rid) = 0;
	virtual void buffer_flush(RID buffer_rid, ::ivec2 range) = 0;
	virtual void buffer_set_name(RID buffer_rid, const char* name) = 0;
	[[nodiscard]] virtual GpuDeviceAddress buffer_virtual_address(const RID buffer_rid) = 0;
	[[nodiscard]] virtual void* buffer_map(const RID buffer_rid) = 0;
	virtual void buffer_unmap(const RID buffer_rid) = 0;
	[[nodiscard]] virtual void* buffer_mapped_data(const RID buffer_rid) = 0;
	
	[[nodiscard]] virtual RID image_create() = 0;
	[[nodiscard]] virtual RID image_create(const ImageDescriptor &desc) = 0;
	virtual void image_create(RID image_rid, const ImageDescriptor &desc) = 0;
	virtual void image_delete(const RID image_rid) = 0;
	virtual void image_set_name(RID image_rid, const char* name) = 0;
	[[nodiscard]] virtual bool image_is_valid(RID image_rid) = 0;
	
	[[nodiscard]] virtual RID image_view_create(const ImageViewDescriptor &desc) = 0;
	virtual void image_view_delete(const RID image_view_rid) = 0;
	[[nodiscard]] virtual bool image_view_is_valid(const RID image_view_rid) = 0;
	
	[[nodiscard]] virtual RID sampler_create(const SamplerDescriptor &desc) = 0;
	virtual void sampler_delete(const RID sampler_rid) = 0;
	
	[[nodiscard]] virtual RID surface_create(IWindow *window, const SurfaceDescriptor &desc) = 0;
	[[nodiscard]] virtual RID surface_create_universal(IWindow *window, VkSurfaceKHR surface, const SurfaceDescriptor &desc) = 0;
	[[nodiscard]] virtual RID surface_create_sdl2(SDL2Window *window, const SurfaceDescriptor &desc) = 0;
	[[nodiscard]] virtual RID surface_create_glfw3(GLFW3Window *window, const SurfaceDescriptor &desc) = 0;
	[[nodiscard]] virtual Vector<gfx::Format> surface_get_formats(const RID surface_rid) = 0;
	[[nodiscard]] virtual gfx::Format surface_get_color_format(const RID surface_rid) = 0;
	[[nodiscard]] virtual RID surface_get_active_image(const RID surface_rid) = 0;
	[[nodiscard]] virtual RID surface_get_active_image_view(const RID surface_rid) = 0;
	virtual void update_surface_configuration(const RID surface_rid, const SurfaceDescriptor &desc) = 0;
	virtual void surface_delete(const RID surface_rid) = 0;
	
	[[nodiscard]] virtual RID shader_create(const SpirvDescriptor& spirv_descriptor) = 0;
	virtual void shader_delete(RID id) = 0;
	
	virtual RID bind_group_layout_create(const BindGroupLayoutDescriptor &desc) = 0;
	virtual void bind_group_layout_delete(const RID bind_group_layout_rid) = 0;
	
	[[nodiscard]] virtual RID bind_group_create(const BindGroupDescriptor &desc) = 0;
	virtual void bind_group_delete(const RID bind_group_rid) = 0;
	virtual void bind_group_update(const RID bind_group_rid, const Vector<BindGroupEntryDescriptor> &entries) = 0;
	virtual void set_bind_group(const RID command_rid, const RID pipeline_layout_rid, u32 index, const RID bind_group_rid, gfx::ShaderStage stage) = 0;
	
	[[nodiscard]] virtual RID pipeline_layout_create(const PipelineLayoutDescriptor &desc) = 0;
	virtual void pipeline_layout_delete(const RID pipeline_layout_rid) = 0;
	
	[[nodiscard]] virtual RID graphics_pipeline_create(const GraphicsPipelineDescriptor &desc) = 0;
	virtual void pipeline_delete(const RID pipeline_rid) = 0;
	
	virtual void push_constants(const RID command_rid, const RID pipeline_layout_rid, const PushConstantRangeDescriptor& descriptor, const void *data) = 0;
	virtual void bind_index_buffer(const RID command_rid, const IndexBufferDescriptor &desc) = 0;
	virtual void bind_vertex_buffer(const RID command_rid, const VertexBufferDescriptor &desc) = 0;
	virtual void bind_vertex_buffers(const RID command_rid, const Vector<VertexBufferDescriptor> &desc) = 0;
	virtual void pipeline_bind(const RID pipeline, const RID cmd_rid, gfx::PipelineBindPoint bind_point) = 0;
	
	[[nodiscard]] virtual RID begin_recording(RID surface_rid) = 0;
	virtual uint32_t begin_rendering(RID surface_rid, const RID command_rid, const RID pipeline_rid, const RID depth_image_view) = 0;
	virtual void finish_rendering(const RID command_rid) const = 0;
	virtual void finish_recording(const RID command_rid) const = 0;
	virtual void bind_shader(RID command_rid, RID shader_rid, gfx::ShaderStage stage) = 0;
	virtual void bind_shader(RID command_rid, Vector<RID> shader_rids, Vector<gfx::ShaderStage> stages) = 0;
	virtual void bind_shader(RID command_rid, Vector<gfx::BindShaderDescriptor> stages) = 0;
	virtual void transition(RID command_rid, const ImageTransitionDescriptor &descriptor) =0;
	virtual void transition(RID command_rid, const Vector<ImageTransitionDescriptor> &descriptors) =0;
	virtual void draw_indexed_instanced(RID command_rid, u32 index_count, u32 instance_count, u32 first_index, i32 vertex_offset, u32 first_instance) = 0;
	virtual void draw_mesh_tasks(RID command_rid, uvec3 groups) = 0;
	virtual void draw_mesh_tasks(RID command_rid, u32 groups_x, u32 groups_y, u32 groups_z) = 0;

	[[nodiscard]] virtual uint32_t queue_family(gfx::QueueFamilyType queue_family) const = 0;
	
	virtual void command_submit(RID surface_rid, RID command_rid) = 0;
	virtual void present(RID surface_rid) = 0;
	
	virtual void force_wait_for_device_idle() = 0;
protected:
	enum class ResourceKind : u8 {
		eNone = 0,
		eBuffer,
		eImage,
		eImageView,
		eSampler,
		eShaderModule,
		eCommandPool,
		ePipelineLayout,
		ePipeline,
		eFence,
		eSemaphore,
		eCommandBuffer,
		eBindGroupLayout,
		eBindGroup,
		eSurface
	};
	
	static bool is_valid_rid(const RID rid);

	static constexpr u32 RID_KIND_BITS = 8;
	static constexpr u32 RID_SLOT_BITS = 32 - RID_KIND_BITS;
	static constexpr u32 RID_SLOT_MASK = (1u << RID_SLOT_BITS) - 1u;
	
	[[nodiscard]] static RID _make_rid(ResourceKind kind, u32 slot);
	[[nodiscard]] static ResourceKind _rid_kind(RID rid);
	[[nodiscard]] static u32 _rid_slot(RID rid);
	[[nodiscard]] static bool _rid_is_kind(RID rid, ResourceKind kind);

	u64 rid_generation_counter_ = 0;
};

/**
 *\brief Contains all relevant information that needs to persist in one contiguous state.
 * This is now a thin wrapper around backend implementations.
 */
class GraphicsDriver {
public:
	GraphicsDriver(RenderingApiBackend backend = RenderingApiBackend::eVulkan);
	~GraphicsDriver();

	void start();
	void stop();
	void set_backend(RenderingApiBackend backend);

	static GraphicsDriver *singleton();
	static GraphicsBackend *get();
	[[nodiscard]] RenderingApiBackend backend() const { return backend_api_; }

private:
	UniquePtr<GraphicsBackend> backend_;
	RenderingApiBackend backend_api_ = RenderingApiBackend::eVulkan;

public:
	friend class Window;
	friend class GraphicsBackend;
	friend class VkGraphicsBackend;
	friend class D3D12DriverBackend;
};

namespace gfx {
	template <typename T>
	constexpr BufferDescriptor buffer(const Vector<T> &data, const BitFlag<BufferUsage> &usage) {
		return {
			.label = std::nullopt,
			.size = sizeof(T) * data.size(),
			.usage = usage,
			.memory_usage = MemoryUsage::eAuto,
			.allocation_hints = Optional(AllocationHint::eHostSequentialWrite | AllocationHint::eAllowTransferInstead | AllocationHint::eMapped)
		};
	}
	
	template <typename T>
	constexpr BufferDescriptor buffer(const String& label, const Vector<T> &data, const BitFlag<BufferUsage> &usage) {
		return {
			.label = label,
			.size = sizeof(T) * data.size(),
			.usage = usage,
			.memory_usage = MemoryUsage::eAuto,
			.allocation_hints = Optional(AllocationHint::eHostSequentialWrite | AllocationHint::eAllowTransferInstead | AllocationHint::eMapped)
		};
	}
	
	template <typename T>
	RID allocateBuffer(const Vector<T> &data, const BitFlag<BufferUsage> &usage) {
		GraphicsBackend* backend = GraphicsDriver::get();
		RID buffer = backend->buffer_create(gfx::buffer(data, usage));
		T* buffer_data = static_cast<T *>(backend->buffer_mapped_data(buffer));
		assert(buffer_data != nullptr);
		std::memcpy(buffer_data, data.data(), sizeof(T) * data.size());
		return buffer;
	}
	template <typename T>
	RID allocateBuffer(const String& label, const Vector<T> &data, const BitFlag<BufferUsage> &usage) {
		GraphicsBackend* backend = GraphicsDriver::get();
		RID buffer = backend->buffer_create(gfx::buffer(label, data, usage));
		T* buffer_data = static_cast<T*>(backend->buffer_mapped_data(buffer));
		assert(buffer_data != nullptr);
		std::memcpy(buffer_data, data.data(), sizeof(T) * data.size());
		return buffer;
	}
	
	namespace gfx_detail {
		template <typename T, typename ...X>
		void allocateBufferMultipleVectors(u8 *map_address, Vector<T> data, Vector<X>... rest) {
			std::memcpy(map_address, data.data(), sizeof(T) * data.size());
			if constexpr (sizeof...(rest) > 0) {
				allocateBufferMultipleVectors<X...>(map_address + sizeof(T) * data.size(), rest...);
			}
		}
		
		template <typename T>
		void allocateBufferMultipleVectors(u8* map_address, Vector<T> data) {
			std::memcpy(map_address, data.data(), sizeof(T) * data.size());
		}
		
		template <typename ...T>
		size_t calculateTotalSize(const Vector<T>... datas) {
			return sizeof(T) * datas.size() + calculateTotalSize(datas...);
		}
	}
	
	template <typename ...T>
	RID allocateBuffer(const String& label, const Vector<T> &...datas, const BitFlag<BufferUsage> &usage) {
		GraphicsBackend* backend = GraphicsDriver::get();
		const size_t buffer_size = gfx_detail::calculateTotalSize(datas...);

		const BufferDescriptor desc{
			.label = label,
			.size = buffer_size,
			.usage = usage,
			.memory_usage = MemoryUsage::eAuto,
			.allocation_hints = AllocationHint::eHostSequentialWrite | AllocationHint::eAllowTransferInstead | AllocationHint::eMapped
		};
		
		RID buffer = backend->buffer_create(desc);
		
		u8* mapped_data = static_cast<u8*>(backend->buffer_mapped_data(buffer));
		gfx_detail::allocateBufferMultipleVectors(mapped_data, datas...);
		
		return buffer;
	}
	
}
