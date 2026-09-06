#pragma once

#include "gpu/driver.hpp"
#include <d3d12.h>

namespace D3D12 {
	constexpr D3D12_FILTER Filter(const gfx::Filter filter) {
		switch (filter) {
		case gfx::Filter::eNearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case gfx::Filter::eLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}
		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}

	constexpr D3D12_TEXTURE_ADDRESS_MODE AddressMode(const gfx::AddressMode address_mode) {
		switch (address_mode) {
		case gfx::AddressMode::eRepeat: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case gfx::AddressMode::eMirroredRepeat: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case gfx::AddressMode::eClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case gfx::AddressMode::eClampToBorder: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case gfx::AddressMode::eMirrorClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		}
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}

	constexpr D3D12_COMPARISON_FUNC CompareOp(const gfx::CompareOp compare_op) {
		switch (compare_op) {
		case gfx::CompareOp::eNever: return D3D12_COMPARISON_FUNC_NEVER;
		case gfx::CompareOp::eLess: return D3D12_COMPARISON_FUNC_LESS;
		case gfx::CompareOp::eEqual: return D3D12_COMPARISON_FUNC_EQUAL;
		case gfx::CompareOp::eLessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case gfx::CompareOp::eGreater: return D3D12_COMPARISON_FUNC_GREATER;
		case gfx::CompareOp::eNotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case gfx::CompareOp::eGreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case gfx::CompareOp::eAlways: return D3D12_COMPARISON_FUNC_ALWAYS;
		}
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}

	constexpr DXGI_FORMAT Format(gfx::Format format) {
		using enum gfx::Format;
		switch (format) {
		case eUndefined: return DXGI_FORMAT_UNKNOWN;
		case eRgba32Uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case eRgba32Sfloat: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case eRgba32Sint: return DXGI_FORMAT_R32G32B32A32_SINT;
		case eRgb32Uint: return DXGI_FORMAT_R32G32B32_UINT;
		case eRgb32Sfloat: return DXGI_FORMAT_R32G32B32_FLOAT;
		case eRgb32Sint: return DXGI_FORMAT_R32G32B32_SINT;
		case eRgba16Uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case eRgba16Sfloat: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case eRgba16Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case eRgba16Snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case eRgba16Sint: return DXGI_FORMAT_R16G16B16A16_SINT;
		case eRg32Uint: return DXGI_FORMAT_R32G32_UINT;
		case eRg32Sfloat: return DXGI_FORMAT_R32G32_FLOAT;
		case eRg32Sint: return DXGI_FORMAT_R32G32_SINT;
		case eA2Rgb10UnormPack32: return DXGI_FORMAT_R10G10B10A2_UNORM;
		case eA2Rgb10UintPack32: return DXGI_FORMAT_R10G10B10A2_UINT;
		case eB10Gr11UfloatPack32: return DXGI_FORMAT_R11G11B10_FLOAT;
		case eRgba8Uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case eRgba8Unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case eRgba8Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case eRgba8Snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case eRgba8Sint: return DXGI_FORMAT_R8G8B8A8_SINT;
		case eRg16Unorm: return DXGI_FORMAT_R16G16_UNORM;
		case eRg16Sfloat: return DXGI_FORMAT_R16G16_FLOAT;
		case eRg16Uint: return DXGI_FORMAT_R16G16_UINT;
		case eRg16Snorm: return DXGI_FORMAT_R16G16_SNORM;
		case eRg16Sint: return DXGI_FORMAT_R16G16_SINT;
		case eR32Sfloat: return DXGI_FORMAT_R32_FLOAT;
		case eDepth32Sfloat: return DXGI_FORMAT_D32_FLOAT;
		case eR32Uint: return DXGI_FORMAT_R32_UINT;
		case eR32Sint: return DXGI_FORMAT_R32_SINT;
		case eDepth24UnormStencil8Uint: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case eRg8Uint: return DXGI_FORMAT_R8G8_UINT;
		case eRg8Unorm: return DXGI_FORMAT_R8G8_UNORM;
		case eRg8Snorm: return DXGI_FORMAT_R8G8_SNORM;
		case eRg8Sint: return DXGI_FORMAT_R8G8_SINT;
		case eR16Unorm: return DXGI_FORMAT_R16_UNORM;
		case eR16Sfloat: return DXGI_FORMAT_R16_FLOAT;
		case eDepth16Unorm: return DXGI_FORMAT_D16_UNORM;
		case eR16Uint: return DXGI_FORMAT_R16_UINT;
		case eR16Snorm: return DXGI_FORMAT_R16_SNORM;
		case eR16Sint: return DXGI_FORMAT_R16_SINT;
		case eR8Uint: return DXGI_FORMAT_R8_UINT;
		case eR8Unorm: return DXGI_FORMAT_R8_UNORM;
		case eR8Snorm: return DXGI_FORMAT_R8_SNORM;
		case eR8Sint: return DXGI_FORMAT_R8_SINT;
		case eA8Unorm: return DXGI_FORMAT_A8_UNORM;
		case eE5B9G9R9UfloatPack32: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case eBgrg8422Unorm: return DXGI_FORMAT_R8G8_B8G8_UNORM;
		case eGbgr8422Unorm: return DXGI_FORMAT_G8R8_G8B8_UNORM;
		case eBc1RgbaUnormBlock: return DXGI_FORMAT_BC1_UNORM;
		case eBc1RgbaSrgbBlock: return DXGI_FORMAT_BC1_UNORM_SRGB;
		case eBc2UnormBlock: return DXGI_FORMAT_BC2_UNORM;
		case eBc2SrgbBlock: return DXGI_FORMAT_BC2_UNORM_SRGB;
		case eBc3UnormBlock: return DXGI_FORMAT_BC3_UNORM;
		case eBc3SrgbBlock: return DXGI_FORMAT_BC3_UNORM_SRGB;
		case eBc4UnormBlock: return DXGI_FORMAT_BC4_UNORM;
		case eBc4SnormBlock: return DXGI_FORMAT_BC4_SNORM;
		case eBc5UnormBlock: return DXGI_FORMAT_BC5_UNORM;
		case eBc5SnormBlock: return DXGI_FORMAT_BC5_SNORM;
		case eB5G6R5UnormPack16: return DXGI_FORMAT_B5G6R5_UNORM;
		case eBgr5A1UnormPack16: return DXGI_FORMAT_B5G5R5A1_UNORM;
		case eBgra8Unorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case eA2Bgr10UnormPack32: return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		case eBgra8Srgb: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case eBc6HUfloatBlock: return DXGI_FORMAT_BC6H_UF16;
		case eBc6HSfloatBlock: return DXGI_FORMAT_BC6H_SF16;
		case eBc7UnormBlock: return DXGI_FORMAT_BC7_UNORM;
		case eBc7SrgbBlock: return DXGI_FORMAT_BC7_UNORM_SRGB;
		case eGbr82Plane420Unorm: return DXGI_FORMAT_NV12;
		case eG10X6B10X6R10X62Plane420Unorm3Pack16: return DXGI_FORMAT_P010;
		case eGbr162Plane420Unorm: return DXGI_FORMAT_P016;
		case eGbr83Plane420Unorm: return DXGI_FORMAT_420_OPAQUE;
		case eG10X6B10X6G10X6R10X6422Unorm4Pack16: return DXGI_FORMAT_Y210;
		case eGbgr16422Unorm: return DXGI_FORMAT_Y216;
		case eBgra4UnormPack16: return DXGI_FORMAT_B4G4R4A4_UNORM;
		case eDepth32SfloatStencil8Uint: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case eRg4UnormPack8:
		case eRgba4UnormPack16:
		case eR5G6B5UnormPack16:
		case eRgb5A1UnormPack16:
		case eA1Rgb5UnormPack16:
		case eR8Uscaled:
		case eR8Sscaled:
		case eR8Srgb:
		case eRg8Uscaled:
		case eRg8Sscaled:
		case eRg8Srgb:
		case eRgb8Unorm:
		case eRgb8Snorm:
		case eRgb8Uscaled:
		case eRgb8Sscaled:
		case eRgb8Uint:
		case eRgb8Sint:
		case eRgb8Srgb:
		case eBgr8Unorm:
		case eBgr8Snorm:
		case eBgr8Uscaled:
		case eBgr8Sscaled:
		case eBgr8Uint:
		case eBgr8Sint:
		case eBgr8Srgb:
		case eRgba8Uscaled:
		case eRgba8Sscaled:
		case eBgra8Snorm:
		case eBgra8Uscaled:
		case eBgra8Sscaled:
		case eBgra8Uint:
		case eBgra8Sint:
		case eAbgr8UnormPack32:
		case eAbgr8SnormPack32:
		case eAbgr8UscaledPack32:
		case eAbgr8SscaledPack32:
		case eAbgr8UintPack32:
		case eAbgr8SintPack32:
		case eAbgr8SrgbPack32:
		case eA2Rgb10SnormPack32:
		case eA2Rgb10UscaledPack32:
		case eA2Rgb10SscaledPack32:
		case eA2Rgb10SintPack32:
		case eA2Bgr10SnormPack32:
		case eA2Bgr10UscaledPack32:
		case eA2Bgr10SscaledPack32:
		case eA2Bgr10UintPack32:
		case eA2Bgr10SintPack32:
		case eR16Uscaled:
		case eR16Sscaled:
		case eRg16Uscaled:
		case eRg16Sscaled:
		case eRgb16Unorm:
		case eRgb16Snorm:
		case eRgb16Uscaled:
		case eRgb16Sscaled:
		case eRgb16Uint:
		case eRgb16Sint:
		case eRgb16Sfloat:
		case eRgba16Uscaled:
		case eRgba16Sscaled:
		case eR64Uint:
		case eR64Sint:
		case eR64Sfloat:
		case eRg64Uint:
		case eRg64Sint:
		case eRg64Sfloat:
		case eRgb64Uint:
		case eRgb64Sint:
		case eRgb64Sfloat:
		case eRgba64Uint:
		case eRgba64Sint:
		case eRgba64Sfloat:
		case eX8D24UnormPack32:
		case eS8Uint:
		case eDepth16UnormStencil8Uint:
		case eBc1RgbUnormBlock:
		case eBc1RgbSrgbBlock:
		case eEtc2Rgb8UnormBlock:
		case eEtc2Rgb8SrgbBlock:
		case eEtc2Rgb8A1UnormBlock:
		case eEtc2Rgb8A1SrgbBlock:
		case eEtc2Rgba8UnormBlock:
		case eEtc2Rgba8SrgbBlock:
		case eEacR11UnormBlock:
		case eEacR11SnormBlock:
		case eEacR11G11UnormBlock:
		case eEacR11G11SnormBlock:
		case eAstc4x4UnormBlock:
		case eAstc4x4SrgbBlock:
		case eAstc5x4UnormBlock:
		case eAstc5x4SrgbBlock:
		case eAstc5x5UnormBlock:
		case eAstc5x5SrgbBlock:
		case eAstc6x5UnormBlock:
		case eAstc6x5SrgbBlock:
		case eAstc6x6UnormBlock:
		case eAstc6x6SrgbBlock:
		case eAstc8x5UnormBlock:
		case eAstc8x5SrgbBlock:
		case eAstc8x6UnormBlock:
		case eAstc8x6SrgbBlock:
		case eAstc8x8UnormBlock:
		case eAstc8x8SrgbBlock:
		case eAstc10x5UnormBlock:
		case eAstc10x5SrgbBlock:
		case eAstc10x6UnormBlock:
		case eAstc10x6SrgbBlock:
		case eAstc10x8UnormBlock:
		case eAstc10x8SrgbBlock:
		case eAstc10x10UnormBlock:
		case eAstc10x10SrgbBlock:
		case eAstc12x10UnormBlock:
		case eAstc12x10SrgbBlock:
		case eAstc12x12UnormBlock:
		case eAstc12x12SrgbBlock:
		case eGbr83Plane422Unorm:
		case eGbr82Plane422Unorm:
		case eGbr83Plane444Unorm:
		case eR10X6UnormPack16:
		case eR10X6G10X6Unorm2Pack16:
		case eR10X6G10X6B10X6A10X6Unorm4Pack16:
		case eB10X6G10X6R10X6G10X6422Unorm4Pack16:
		case eG10X6B10X6R10X63Plane420Unorm3Pack16:
		case eG10X6B10X6R10X63Plane422Unorm3Pack16:
		case eG10X6B10X6R10X62Plane422Unorm3Pack16:
		case eG10X6B10X6R10X63Plane444Unorm3Pack16:
		case eR12X4UnormPack16:
		case eR12X4G12X4Unorm2Pack16:
		case eR12X4G12X4B12X4A12X4Unorm4Pack16:
		case eG12X4B12X4G12X4R12X4422Unorm4Pack16:
		case eB12X4G12X4R12X4G12X4422Unorm4Pack16:
		case eG12X4B12X4R12X43Plane420Unorm3Pack16:
		case eG12X4B12X4R12X42Plane420Unorm3Pack16:
		case eG12X4B12X4R12X43Plane422Unorm3Pack16:
		case eG12X4B12X4R12X42Plane422Unorm3Pack16:
		case eG12X4B12X4R12X43Plane444Unorm3Pack16:
		case eBgrg16422Unorm:
		case eGbr163Plane420Unorm:
		case eGbr163Plane422Unorm:
		case eGbr162Plane422Unorm:
		case eGbr163Plane444Unorm:
		case eGbr82Plane444Unorm:
		case eG10X6B10X6R10X62Plane444Unorm3Pack16:
		case eG12X4B12X4R12X42Plane444Unorm3Pack16:
		case eGbr162Plane444Unorm:
		case eArgb4UnormPack16:
		case eAbgr4UnormPack16:
		case eAstc4x4SfloatBlock:
		case eAstc5x4SfloatBlock:
		case eAstc5x5SfloatBlock:
		case eAstc6x5SfloatBlock:
		case eAstc6x6SfloatBlock:
		case eAstc8x5SfloatBlock:
		case eAstc8x6SfloatBlock:
		case eAstc8x8SfloatBlock:
		case eAstc10x5SfloatBlock:
		case eAstc10x6SfloatBlock:
		case eAstc10x8SfloatBlock:
		case eAstc10x10SfloatBlock:
		case eAstc12x10SfloatBlock:
		case eAstc12x12SfloatBlock:
		case eA1Bgr5UnormPack16:
		case ePvrtc12BppUnormBlockIMG:
		case ePvrtc14BppUnormBlockIMG:
		case ePvrtc22BppUnormBlockIMG:
		case ePvrtc24BppUnormBlockIMG:
		case ePvrtc12BppSrgbBlockIMG:
		case ePvrtc14BppSrgbBlockIMG:
		case ePvrtc22BppSrgbBlockIMG:
		case ePvrtc24BppSrgbBlockIMG:
		case eRg16Sfixed5NV:
		case eRg16S105NV:
			return DXGI_FORMAT_UNKNOWN;
		}
	}
}
