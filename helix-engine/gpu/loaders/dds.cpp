// ReSharper disable CppClangTidyMiscUseInternalLinkage
// ReSharper disable CppClangTidyPerformanceEnumSize
#include "dds.hpp"

#include <cassert>
#include <format>
#include <iostream>
#include <Windows.h>

#include "util.hpp"
#include "glad/glad.h"
#include "gpu/driver.hpp"
#include "gpu/graphics.hpp"
#include "gpu/backends/vulkan_backend.hpp"

#define DDS_LOADER_DEBUG

#ifdef DDS_LOADER_DEBUG
#define DDS_DebugPrint(...) printf("[DDS] "); printf(__VA_ARGS__); printf("\n")
#else
#define DDS_DebugPrint(...)
#endif

#ifdef _DEBUG
typedef struct DDS_DEBUG_STATE {
	std::atomic<size_t> allocated_memory_;
	std::atomic<size_t> images_loaded;
} DDS_DEBUG_STATE;

static DDS_DEBUG_STATE state{0, 0};

#endif

typedef enum DDS_FLAGS1 : DWORD {
	CAPS        = 0x1,
	HEIGHT      = 0x2,
	WIDTH       = 0x4,
	PITCH       = 0x8,
	PIXELFORMAT = 0x1000,
	MIPMAPCOUNT = 0x20000,
	LINEARSIZE  = 0X80000,
	DEPTH       = 0x800000
} DDS_FLAGS1;

typedef enum DDS_FLAGS2 : DWORD {
	ALPHAPIXELS = 0x1,
	ALPHA       = 0x2,
	FOURCC      = 0x4,
	RGB         = 0x40,
	YUV         = 0x200,
	LUMINANCE   = 0x20000
} DDS_FLAGS2;

typedef enum DDS_CAPS1 : DWORD {
	CUBE_MAP          = 0x1008,
	TEXTURE_W_MIPMAPS = 0x401008
} DDS_CAPS1;

typedef enum DDS_CAPS2 : DWORD {
	CUBEMAP            = 0x200,
	CUBEMAP_POSITIVE_X = 0x400,
	CUBEMAP_NEGATIVE_X = 0x800,
	CUBEMAP_POSITIVE_Y = 0x1000,
	CUBEMAP_NEGATIVE_Y = 0x2000,
	CUBEMAP_POSITIVE_Z = 0x4000,
	CUBEMAP_NEGATIVE_Z = 0x8000,
	VOLUME             = 0x200000
} DDS_CAPS2;

typedef enum D3D10_RESOURCE_DIMENSION : DWORD {
	D3D10_RESOURCE_DIMENSION_UNKNOWN   = 0,
	D3D10_RESOURCE_DIMENSION_BUFFER    = 1,
	D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
	D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
	D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
} D3D10_RESOURCE_DIMENSION;

typedef enum DXGI_FORMAT : DWORD {
	DXGI_FORMAT_UNKNOWN                                 = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS                   = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT                      = 2,
	DXGI_FORMAT_R32G32B32A32_UINT                       = 3,
	DXGI_FORMAT_R32G32B32A32_SINT                       = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS                      = 5,
	DXGI_FORMAT_R32G32B32_FLOAT                         = 6,
	DXGI_FORMAT_R32G32B32_UINT                          = 7,
	DXGI_FORMAT_R32G32B32_SINT                          = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS                   = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT                      = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM                      = 11,
	DXGI_FORMAT_R16G16B16A16_UINT                       = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM                      = 13,
	DXGI_FORMAT_R16G16B16A16_SINT                       = 14,
	DXGI_FORMAT_R32G32_TYPELESS                         = 15,
	DXGI_FORMAT_R32G32_FLOAT                            = 16,
	DXGI_FORMAT_R32G32_UINT                             = 17,
	DXGI_FORMAT_R32G32_SINT                             = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS                       = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT                    = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS                = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT                 = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS                    = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM                       = 24,
	DXGI_FORMAT_R10G10B10A2_UINT                        = 25,
	DXGI_FORMAT_R11G11B10_FLOAT                         = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS                       = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM                          = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB                     = 29,
	DXGI_FORMAT_R8G8B8A8_UINT                           = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM                          = 31,
	DXGI_FORMAT_R8G8B8A8_SINT                           = 32,
	DXGI_FORMAT_R16G16_TYPELESS                         = 33,
	DXGI_FORMAT_R16G16_FLOAT                            = 34,
	DXGI_FORMAT_R16G16_UNORM                            = 35,
	DXGI_FORMAT_R16G16_UINT                             = 36,
	DXGI_FORMAT_R16G16_SNORM                            = 37,
	DXGI_FORMAT_R16G16_SINT                             = 38,
	DXGI_FORMAT_R32_TYPELESS                            = 39,
	DXGI_FORMAT_D32_FLOAT                               = 40,
	DXGI_FORMAT_R32_FLOAT                               = 41,
	DXGI_FORMAT_R32_UINT                                = 42,
	DXGI_FORMAT_R32_SINT                                = 43,
	DXGI_FORMAT_R24G8_TYPELESS                          = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT                       = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS                   = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT                    = 47,
	DXGI_FORMAT_R8G8_TYPELESS                           = 48,
	DXGI_FORMAT_R8G8_UNORM                              = 49,
	DXGI_FORMAT_R8G8_UINT                               = 50,
	DXGI_FORMAT_R8G8_SNORM                              = 51,
	DXGI_FORMAT_R8G8_SINT                               = 52,
	DXGI_FORMAT_R16_TYPELESS                            = 53,
	DXGI_FORMAT_R16_FLOAT                               = 54,
	DXGI_FORMAT_D16_UNORM                               = 55,
	DXGI_FORMAT_R16_UNORM                               = 56,
	DXGI_FORMAT_R16_UINT                                = 57,
	DXGI_FORMAT_R16_SNORM                               = 58,
	DXGI_FORMAT_R16_SINT                                = 59,
	DXGI_FORMAT_R8_TYPELESS                             = 60,
	DXGI_FORMAT_R8_UNORM                                = 61,
	DXGI_FORMAT_R8_UINT                                 = 62,
	DXGI_FORMAT_R8_SNORM                                = 63,
	DXGI_FORMAT_R8_SINT                                 = 64,
	DXGI_FORMAT_A8_UNORM                                = 65,
	DXGI_FORMAT_R1_UNORM                                = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP                      = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM                         = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM                         = 69,
	DXGI_FORMAT_BC1_TYPELESS                            = 70,
	DXGI_FORMAT_BC1_UNORM                               = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB                          = 72,
	DXGI_FORMAT_BC2_TYPELESS                            = 73,
	DXGI_FORMAT_BC2_UNORM                               = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB                          = 75,
	DXGI_FORMAT_BC3_TYPELESS                            = 76,
	DXGI_FORMAT_BC3_UNORM                               = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB                          = 78,
	DXGI_FORMAT_BC4_TYPELESS                            = 79,
	DXGI_FORMAT_BC4_UNORM                               = 80,
	DXGI_FORMAT_BC4_SNORM                               = 81,
	DXGI_FORMAT_BC5_TYPELESS                            = 82,
	DXGI_FORMAT_BC5_UNORM                               = 83,
	DXGI_FORMAT_BC5_SNORM                               = 84,
	DXGI_FORMAT_B5G6R5_UNORM                            = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM                          = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM                          = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM                          = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM              = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS                       = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB                     = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS                       = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB                     = 93,
	DXGI_FORMAT_BC6H_TYPELESS                           = 94,
	DXGI_FORMAT_BC6H_UF16                               = 95,
	DXGI_FORMAT_BC6H_SF16                               = 96,
	DXGI_FORMAT_BC7_TYPELESS                            = 97,
	DXGI_FORMAT_BC7_UNORM                               = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB                          = 99,
	DXGI_FORMAT_AYUV                                    = 100,
	DXGI_FORMAT_Y410                                    = 101,
	DXGI_FORMAT_Y416                                    = 102,
	DXGI_FORMAT_NV12                                    = 103,
	DXGI_FORMAT_P010                                    = 104,
	DXGI_FORMAT_P016                                    = 105,
	DXGI_FORMAT_420_OPAQUE                              = 106,
	DXGI_FORMAT_YUY2                                    = 107,
	DXGI_FORMAT_Y210                                    = 108,
	DXGI_FORMAT_Y216                                    = 109,
	DXGI_FORMAT_NV11                                    = 110,
	DXGI_FORMAT_AI44                                    = 111,
	DXGI_FORMAT_IA44                                    = 112,
	DXGI_FORMAT_P8                                      = 113,
	DXGI_FORMAT_A8P8                                    = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM                          = 115,
	DXGI_FORMAT_P208                                    = 130,
	DXGI_FORMAT_V208                                    = 131,
	DXGI_FORMAT_V408                                    = 132,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE         = 189,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
	DXGI_FORMAT_FORCE_UINT                              = 0xffffffff
} DXGI_FORMAT;

constexpr VkFormat DxgiFormatToVulkan(DXGI_FORMAT format) {
	switch (format) {
	case DXGI_FORMAT_UNKNOWN: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return VK_FORMAT_R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
	case DXGI_FORMAT_R32G32B32A32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
	case DXGI_FORMAT_R32G32B32_TYPELESS: return VK_FORMAT_R32G32B32_UINT;
	case DXGI_FORMAT_R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
	case DXGI_FORMAT_R32G32B32_UINT: return VK_FORMAT_R32G32B32_UINT;
	case DXGI_FORMAT_R32G32B32_SINT: return VK_FORMAT_R32G32B32_SINT;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return VK_FORMAT_R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
	case DXGI_FORMAT_R16G16B16A16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
	case DXGI_FORMAT_R32G32_TYPELESS: return VK_FORMAT_R32G32_UINT;
	case DXGI_FORMAT_R32G32_FLOAT: return VK_FORMAT_R32G32_SFLOAT;
	case DXGI_FORMAT_R32G32_UINT: return VK_FORMAT_R32G32_UINT;
	case DXGI_FORMAT_R32G32_SINT: return VK_FORMAT_R32G32_SINT;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case DXGI_FORMAT_R10G10B10A2_UNORM: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case DXGI_FORMAT_R10G10B10A2_UINT: return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case DXGI_FORMAT_R11G11B10_FLOAT: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS: return VK_FORMAT_R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
	case DXGI_FORMAT_R16G16_TYPELESS: return VK_FORMAT_R16G16_UNORM;
	case DXGI_FORMAT_R16G16_FLOAT: return VK_FORMAT_R16G16_SFLOAT;
	case DXGI_FORMAT_R16G16_UNORM: return VK_FORMAT_R16G16_UNORM;
	case DXGI_FORMAT_R16G16_UINT: return VK_FORMAT_R16G16_UINT;
	case DXGI_FORMAT_R16G16_SNORM: return VK_FORMAT_R16G16_SNORM;
	case DXGI_FORMAT_R16G16_SINT: return VK_FORMAT_R16G16_SINT;
	case DXGI_FORMAT_R32_TYPELESS: return VK_FORMAT_R32_SFLOAT;
	case DXGI_FORMAT_D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
	case DXGI_FORMAT_R32_FLOAT: return VK_FORMAT_R32_SFLOAT;
	case DXGI_FORMAT_R32_UINT: return VK_FORMAT_R32_UINT;
	case DXGI_FORMAT_R32_SINT: return VK_FORMAT_R32_SINT;
	case DXGI_FORMAT_R24G8_TYPELESS: return VK_FORMAT_R32_UINT;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return VK_FORMAT_R32_UINT;
	case DXGI_FORMAT_R8G8_TYPELESS: return VK_FORMAT_R8G8_UINT;
	case DXGI_FORMAT_R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
	case DXGI_FORMAT_R8G8_UINT: return VK_FORMAT_R8G8_UINT;
	case DXGI_FORMAT_R8G8_SNORM: return VK_FORMAT_R8G8_SNORM;
	case DXGI_FORMAT_R8G8_SINT: return VK_FORMAT_R8G8_SINT;
	case DXGI_FORMAT_R16_TYPELESS: return VK_FORMAT_R16_UNORM;
	case DXGI_FORMAT_R16_FLOAT: return VK_FORMAT_R16_SFLOAT;
	case DXGI_FORMAT_D16_UNORM: return VK_FORMAT_D16_UNORM;
	case DXGI_FORMAT_R16_UNORM: return VK_FORMAT_R16_UNORM;
	case DXGI_FORMAT_R16_UINT: return VK_FORMAT_R16_UINT;
	case DXGI_FORMAT_R16_SNORM: return VK_FORMAT_R16_SNORM;
	case DXGI_FORMAT_R16_SINT: return VK_FORMAT_R16_SINT;
	case DXGI_FORMAT_R8_TYPELESS: return VK_FORMAT_R8_UINT;
	case DXGI_FORMAT_R8_UNORM: return VK_FORMAT_R8_UNORM;
	case DXGI_FORMAT_R8_UINT: return VK_FORMAT_R8_UINT;
	case DXGI_FORMAT_R8_SNORM: return VK_FORMAT_R8_SNORM;
	case DXGI_FORMAT_R8_SINT: return VK_FORMAT_R8_SINT;
	case DXGI_FORMAT_A8_UNORM: return VK_FORMAT_A8_UNORM;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
	case DXGI_FORMAT_R8G8_B8G8_UNORM: return VK_FORMAT_B8G8R8G8_422_UNORM;
	case DXGI_FORMAT_G8R8_G8B8_UNORM: return VK_FORMAT_G8B8G8R8_422_UNORM;
	case DXGI_FORMAT_BC1_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case DXGI_FORMAT_BC1_UNORM_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case DXGI_FORMAT_BC2_TYPELESS: return VK_FORMAT_BC2_UNORM_BLOCK;
	case DXGI_FORMAT_BC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
	case DXGI_FORMAT_BC2_UNORM_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
	case DXGI_FORMAT_BC3_TYPELESS: return VK_FORMAT_BC3_UNORM_BLOCK;
	case DXGI_FORMAT_BC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
	case DXGI_FORMAT_BC3_UNORM_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
	case DXGI_FORMAT_BC4_TYPELESS: return VK_FORMAT_BC4_UNORM_BLOCK;
	case DXGI_FORMAT_BC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
	case DXGI_FORMAT_BC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
	case DXGI_FORMAT_BC5_TYPELESS: return VK_FORMAT_BC5_UNORM_BLOCK;
	case DXGI_FORMAT_BC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
	case DXGI_FORMAT_BC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
	case DXGI_FORMAT_B5G6R5_UNORM: return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case DXGI_FORMAT_B5G5R5A1_UNORM: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: return VK_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS: return VK_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
	case DXGI_FORMAT_BC6H_TYPELESS: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case DXGI_FORMAT_BC6H_UF16: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case DXGI_FORMAT_BC6H_SF16: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case DXGI_FORMAT_BC7_TYPELESS: return VK_FORMAT_BC7_UNORM_BLOCK;
	case DXGI_FORMAT_BC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
	case DXGI_FORMAT_BC7_UNORM_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;
	case DXGI_FORMAT_AYUV: return VK_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_Y410: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case DXGI_FORMAT_Y416: return VK_FORMAT_R16G16B16A16_UNORM;
	case DXGI_FORMAT_NV12: return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
	case DXGI_FORMAT_P010: return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
	case DXGI_FORMAT_P016: return VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
	case DXGI_FORMAT_420_OPAQUE: return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
	case DXGI_FORMAT_YUY2: return VK_FORMAT_G8B8G8R8_422_UNORM;
	case DXGI_FORMAT_Y210: return VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
	case DXGI_FORMAT_Y216: return VK_FORMAT_G16B16G16R16_422_UNORM;
	case DXGI_FORMAT_B4G4R4A4_UNORM: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE: return VK_FORMAT_R32G32_UINT;
	case DXGI_FORMAT_R32G8X24_TYPELESS: return VK_FORMAT_R32G32_UINT;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_R1_UNORM: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_BC1_TYPELESS: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_NV11: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_AI44: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_IA44: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_P8: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_A8P8: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_P208: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_V208: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_V408: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE: return VK_FORMAT_UNDEFINED;
	case DXGI_FORMAT_FORCE_UINT: return VK_FORMAT_UNDEFINED;
	}
}

typedef enum D3D10_RESOURCE_MISC_FLAG : DWORD {
	D3D10_RESOURCE_MISC_GENERATE_MIPS     = 0x1L,
	D3D10_RESOURCE_MISC_SHARED            = 0x2L,
	D3D10_RESOURCE_MISC_TEXTURECUBE       = 0x4L,
	D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x10L,
	D3D10_RESOURCE_MISC_GDI_COMPATIBLE    = 0x20L
} D3D10_RESOURCE_MISC_FLAG;

typedef enum D3D11_RESOURCE_MISC_FLAG : DWORD {
	D3D11_RESOURCE_MISC_GENERATE_MIPS                   = 0x1L,
	D3D11_RESOURCE_MISC_SHARED                          = 0x2L,
	D3D11_RESOURCE_MISC_TEXTURECUBE                     = 0x4L,
	D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS               = 0x10L,
	D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS          = 0x20L,
	D3D11_RESOURCE_MISC_BUFFER_STRUCTURED               = 0x40L,
	D3D11_RESOURCE_MISC_RESOURCE_CLAMP                  = 0x80L,
	D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX               = 0x100L,
	D3D11_RESOURCE_MISC_GDI_COMPATIBLE                  = 0x200L,
	D3D11_RESOURCE_MISC_SHARED_NTHANDLE                 = 0x800L,
	D3D11_RESOURCE_MISC_RESTRICTED_CONTENT              = 0x1000L,
	D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE        = 0x2000L,
	D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER = 0x4000L,
	D3D11_RESOURCE_MISC_GUARDED                         = 0x8000L,
	D3D11_RESOURCE_MISC_TILE_POOL                       = 0x20000L,
	D3D11_RESOURCE_MISC_TILED                           = 0x40000L,
	D3D11_RESOURCE_MISC_HW_PROTECTED                    = 0x80000L,
	D3D11_RESOURCE_MISC_SHARED_DISPLAYABLE,
	D3D11_RESOURCE_MISC_SHARED_EXCLUSIVE_WRITER,
	D3D11_RESOURCE_MISC_NO_SHADER_ACCESS
} D3D11_RESOURCE_MISC_FLAG;

typedef union DDS_RESOURCE_MISC_FLAG {
	D3D10_RESOURCE_MISC_FLAG flags10;
	D3D11_RESOURCE_MISC_FLAG flags11;
} DDS_RESOURCE_MISC_FLAG;

typedef enum DDS_D3D10_MISC_FLAGS2 : DWORD {
	DDS_ALPHA_MODE_UNKNOWN       = 0x0,
	DDS_ALPHA_MODE_STRAIGHT      = 0x1,
	DDS_ALPHA_MODE_PREMULTIPLIED = 0x2,
	DDS_ALPHA_MODE_OPAQUE        = 0x3,
	DDS_ALPHA_MODE_CUSTOM        = 0x4,
} DDS_D3D10_MISC_FLAGS2;

typedef struct DDS_HEADER {
	DWORD dwHeaderSize;
	DDS_FLAGS1 dwFlags1;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwPitchOrLinearSize;
	DWORD dwDepth;
	DWORD dwMipMapCount;
	DWORD dwReserved[11];
	DWORD dwSize;
	DDS_FLAGS2 dwFlags2;
	CCHAR dwFourCC[4];
	DWORD dwRgbBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
	DDS_CAPS1 dwCaps;
	DDS_CAPS2 dwCaps2;
	DWORD dwCaps3;
	DWORD dwCaps4;
	DWORD dwReserved2;
} DDS_HEADER, *LP_DDS_HEADER;

typedef struct DDS_HEADER_DXT10_S {
	DXGI_FORMAT dxgiFormat;
	D3D10_RESOURCE_DIMENSION resourceDimension;
	DDS_RESOURCE_MISC_FLAG miscFlag;
	UINT arraySize;
	UINT miscFlags2;
} DDS_HEADER_DXT10, *LP_DDS_HEADER_DXT10;

typedef struct DDS_FILE_S {
	DWORD dwMagic; // must be 0x20534444
	DDS_HEADER header;
	DDS_HEADER_DXT10 header10;
	LPBYTE data;
} DDS_FILE, *LP_DDS_FILE;

typedef struct DDS_GL_FORMAT_S {
	DWORD dwInternalFormat;
	DWORD dwBlockSize;
} DDS_GL_FORMAT, *LP_DDS_GL_FORMAT;

typedef struct DDS_VK_FORMAT_S {
	VkFormat format;
	size_t block_size;
} DDS_VK_FORMAT;

typedef struct DDS_GFX_FORMAT {
	gfx::Format dwFormat;
	size_t dwBlockSize;
} DDS_GFX_FORMAT;

#ifdef DDS_LOADER_DEBUG
static void DDS_DumpHeader(DDS_HEADER const *dds) {
	std::cout << "--== DDS FILE DUMP ==--\n"
		"Header size: " << std::dec << dds->dwHeaderSize << "\n"
		"Flags 1: " << std::hex << dds->dwFlags1 << "\n"
		"Height: " << std::dec << dds->dwHeight << "\n"
		"Width: " << std::dec << dds->dwWidth << "\n"
		"Pitch or linear size: " << std::dec << dds->dwPitchOrLinearSize << "\n"
		"Depth: " << std::dec << dds->dwDepth << "\n"
		"Mip map count: " << std::dec << dds->dwMipMapCount << "\n"
		"Flags 2: " << std::hex << dds->dwFlags2 << "\n"
		"FourCC: " << std::string(dds->dwFourCC, 4) << "\n"
		"RGB bit count: " << std::dec << dds->dwRgbBitCount << "\n"
		"R bit mask: " << std::hex << dds->dwRBitMask << "\n"
		"G bit mask: " << std::hex << dds->dwGBitMask << "\n"
		"B bit mask: " << std::hex << dds->dwBBitMask << "\n"
		"A bit mask: " << std::hex << dds->dwABitMask << "\n"
		"Caps 1: " << std::hex << dds->dwCaps << "\n"
		"Caps 2: " << std::hex << dds->dwCaps2 << "\n"
		"Caps 3: " << std::hex << dds->dwCaps3 << "\n"
		"Caps 4: " << std::hex << dds->dwCaps4 << "\n"
		"Reserved 2: " << std::hex << dds->dwReserved2 << "\n\n";
}

static void DDS_DumpHeader10(DDS_HEADER_DXT10 const *dds) {
	std::cout << "--== DX10 FILE DUMP ==--\n"
		"DXGI Format: " << std::dec << dds->dxgiFormat << "\n"
		"Resource dimension: " << std::dec << dds->resourceDimension << "\n"
		"Misc flag (10): " << std::hex << dds->miscFlag.flags10 << "\n"
		"Misc flag (11): " << std::hex << dds->miscFlag.flags11 << "\n"
		"Array size: " << std::dec << dds->arraySize << "\n"
		"Misc flags 2: " << std::hex << dds->miscFlags2 << "\n\n";
}
#else
#define DDS_DumpHeader(...)
#define DDS_DumpHeader10(...)
#endif

static Error DDS_DXGI2GL_Format(DXGI_FORMAT const dxgiFormat, LP_DDS_GL_FORMAT const format) {
	switch (dxgiFormat) {
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
			.dwBlockSize = 8
		};
		return OK;

	case DXGI_FORMAT_BC2_UNORM_SRGB:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
			.dwBlockSize = 16
		};
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			.dwBlockSize = 16
		};
		return OK;

	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			.dwBlockSize = 16
		};
		return OK;

	case DXGI_FORMAT_BC3_UNORM_SRGB:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
			.dwBlockSize = 16
		};
		return OK;

	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:

	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
		*format = {
			.dwInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
			.dwBlockSize = 16
		};
		return OK;

	case DXGI_FORMAT_BC5_SNORM: {
		*format = {
			.dwInternalFormat = GL_COMPRESSED_SIGNED_RG_RGTC2,
			.dwBlockSize = 16
		};
		return OK;
	}

	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM: {
		*format = {
			.dwInternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM,
			.dwBlockSize = 16
		};
		return OK;
	}
	case DXGI_FORMAT_BC7_UNORM_SRGB: {
		*format = {
			.dwInternalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
			.dwBlockSize = 16
		};
		return OK;
	}
	default:
		return ERR_FILE_CANT_READ;
	}
	return FAILED;
}

static Error DDS_D3D9Format2GLFormat(LPCH const wc4, LP_DDS_GL_FORMAT const lpGlFormat) {
	switch (hash(std::string(wc4, 4))) {
	case hash("DXT1"):
		*lpGlFormat = {
			.dwInternalFormat = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
			.dwBlockSize = 8
		};
		return OK;
	case hash("DXT3"):
		*lpGlFormat = {
			.dwInternalFormat = VK_FORMAT_BC2_SRGB_BLOCK,
			.dwBlockSize = 16
		};
		return OK;
	case hash("DXT5"):
		*lpGlFormat = {
			.dwInternalFormat = VK_FORMAT_BC3_SRGB_BLOCK,
			.dwBlockSize = 16
		};
		return OK;
	case hash("ATI2"):
		*lpGlFormat = {
			.dwInternalFormat = VK_FORMAT_BC5_SNORM_BLOCK,
			.dwBlockSize = 16
		};
		return OK;
	default:
		return ERR_FILE_CORRUPT;
	}
}

static Error DDS_D3D9Format2GfxFormat(LPCH const wc4, DDS_GFX_FORMAT *lpGfxFormat) {
	switch (hash(std::string(wc4, 4))) {
	case hash("DXT1"):
		*lpGfxFormat = {
			.dwFormat = gfx::Format::eBc1RgbaUnormBlock, //VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
			.dwBlockSize = 8
		};
		return OK;
	case hash("DXT3"):
		*lpGfxFormat = {
			.dwFormat = gfx::Format::eBc2UnormBlock, // VK_FORMAT_BC2_SRGB_BLOCK,
			.dwBlockSize = 16
		};
		return OK;
	case hash("DXT5"):
		*lpGfxFormat = {
			.dwFormat = gfx::Format::eBc3UnormBlock,
			.dwBlockSize = 16
		};
		return OK;
	case hash("ATI2"):
		*lpGfxFormat = {
			.dwFormat = gfx::Format::eBc5SnormBlock,
			.dwBlockSize = 16
		};
		return OK;
	default:
		return ERR_FILE_CORRUPT;
	}
}

static Error UploadFromStdIo_Async(FILE *file, u32 const texture_object, std::string &error) {
}

Error DDS_UploadFromStdIO(FILE *file, std::shared_ptr<RID> texture_rid, std::string &error) {
	if (fseek(file, 0, SEEK_END) != 0) return ERR_FILE_NOT_FOUND;
	u32 const uiFileSize = ftell(file);
	if (fseek(file, 0, SEEK_SET) != 0) return ERR_FILE_NOT_FOUND;

	DDS_FILE dds;
	DDS_HEADER *ddsFileHeaderInfo = &dds.header;
	ZeroMemory(ddsFileHeaderInfo, sizeof(DDS_HEADER));

	UINT64 ullBufferSize = uiFileSize - sizeof(DWORD) - sizeof(DDS_HEADER);

	if (uiFileSize < sizeof(DWORD) + sizeof(DDS_HEADER)) {
		DDS_DebugPrint("File is too small to be a valid DDS file! Size: %u bytes\n", uiFileSize);
		error = "File is too small";
		return ERR_FILE_UNRECOGNIZED;
	}
	
	GraphicsBackend* driver = GraphicsDriver::get();
	
	BufferDescriptor desc = {
		.label = "DDS loading buffer",
		.size = ullBufferSize,
		.usage = gfx::BufferUsage::eTransferSrc,
		.memory_usage = gfx::MemoryUsage::eAuto,
		.allocation_hints = gfx::AllocationHint::eHostSequentialWrite | gfx::AllocationHint::eMapped | gfx::AllocationHint::eAllowTransferInstead
	};
	
	RID buffer = driver->CreateBuffer(desc);
	LPBYTE buffer_data = static_cast<LPBYTE>(driver->GetMappedData(buffer));

	if (fread_s(&dds.dwMagic, sizeof(DWORD), sizeof(DWORD), 1, file) != 1) {
		DDS_DebugPrint("Failed to read DDS file magic number!");
		error = "Failed to read DDS file magic";
		return ERR_FILE_CANT_READ;
	}

	if (dds.dwMagic != static_cast<DWORD>(0x20534444)) {
		DDS_DebugPrint("File is not a valid DDS file (magic number mismatch) %lu\n", dds.dwMagic);
		error = "File is not a valid DDS file";
		return ERR_FILE_UNRECOGNIZED;
	}

	if (fread_s(ddsFileHeaderInfo, sizeof(DDS_HEADER), sizeof(DDS_HEADER), 1, file) != 1) {
		DDS_DebugPrint("Failed to read basic pre-DX10 DDS file header!");
		error = "Failed to read basic pre-DX10 DDS file header";
		return ERR_FILE_CORRUPT;
	}

	DDS_GFX_FORMAT gfxFormat;
	ZeroMemory(&gfxFormat, sizeof(gfxFormat));
	if (hash(ddsFileHeaderInfo->dwFourCC) == hash("DX10")) {
		if (fread_s(&dds.header10, sizeof(DDS_HEADER_DXT10), sizeof(DDS_HEADER_DXT10), 1, file) != 1) {
			DDS_DebugPrint("Failed to read DX10 extended DDS file header despite the presence of the DX10 FourCC code!");
			error = "Failed to read DX10 extended DDS file header";
			return ERR_FILE_CORRUPT;
		}
		ullBufferSize -= sizeof(DDS_HEADER_DXT10);
		DDS_DumpHeader10(&dds.header10);
	}
	else if (Error const err = DDS_D3D9Format2GfxFormat(ddsFileHeaderInfo->dwFourCC, &gfxFormat); err != OK) {
		DDS_DebugPrint("Failed to get D3D9 format from DDS file!");
		std::string four_cc(ddsFileHeaderInfo->dwFourCC, 4);
		error = std::format("Failed to get D3D9 format from DDS file ({})", four_cc);
		return err;
	}

	size_t uiOffset = 0;
	size_t uiWidth = ddsFileHeaderInfo->dwWidth;
	size_t uiHeight = ddsFileHeaderInfo->dwHeight;
	
	ImageDescriptor imageDescriptor{
		.label = "DDS image",
		.format = gfxFormat.dwFormat,
		.type = gfx::ImageType::e2D,
		.usage = gfx::ImageUsage::eTransferDst | gfx::ImageUsage::eSampled,
		.samples = gfx::SampleCount::e1,
		.initial_layout = gfx::ImageLayout::eUndefined,
		.size = uvec3(uiWidth, uiHeight, 1),
		.array_layers = 1,
		.mip_levels = ddsFileHeaderInfo->dwMipMapCount
	};
	
	*texture_rid = driver->CreateImage(imageDescriptor);
	
	Vector<VkBufferImageCopy2> copies(ddsFileHeaderInfo->dwMipMapCount);
	for (DWORD dwTextureLevel = 0; dwTextureLevel < ddsFileHeaderInfo->dwMipMapCount; dwTextureLevel++) {
		const DWORD dwImageSize = (DWORD)((uiWidth + 3) / 4 * ((uiHeight + 3) / 4) * gfxFormat.dwBlockSize);
		
		copies[dwTextureLevel] = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
			.pNext = nullptr,
			.bufferOffset = uiOffset,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = dwTextureLevel,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageOffset = { 0, 0, 0 },
			.imageExtent = {
				.width = static_cast<uint32_t>(uiWidth),
				.height = static_cast<uint32_t>(uiHeight),
				.depth = 1
			}
		};

		size_t code = fread_s(buffer_data + uiOffset, dwImageSize, 1, dwImageSize, file);
		if (code != dwImageSize) {
			DDS_DebugPrint("Failed to read DDS image data!");
			error = "Failed to read DDS image data";
			return ERR_FILE_CORRUPT;
		}
		
#ifdef _DEBUG
		state.allocated_memory_ += dwImageSize;
#endif
		
		uiOffset += dwImageSize;
		uiWidth /= 2;
		uiHeight /= 2;
	}
	
	VkGraphicsBackend*backend = dynamic_cast<VkGraphicsBackend*>(GraphicsDriver::get());
	VkFence fence = backend->LoadImageFromBuffer(*texture_rid, buffer, copies);
	vkWaitForFences(backend->GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
	backend->DestroyBuffer(buffer);
	
#ifdef _DEBUG
	++state.images_loaded;
#endif

	return OK;
}
