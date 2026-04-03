#include "dds.hpp"

#include <cassert>
#include <iostream>
#include <Windows.h>

#include "util.hpp"
#include "glad/glad.h"
#include "gpu/graphics.hpp"

typedef enum : DWORD {
	CAPS = 0x1,
	HEIGHT = 0x2,
	WIDTH = 0x4,
	PITCH = 0x8,
	PIXELFORMAT = 0x1000,
	MIPMAPCOUNT = 0x20000,
	LINEARSIZE = 0X80000,
	DEPTH = 0x800000
} DDS_FLAGS1;

typedef enum : DWORD {
	ALPHAPIXELS = 0x1,
	ALPHA = 0x2,
	FOURCC = 0x4,
	RGB = 0x40,
	YUV = 0x200,
	LUMINANCE = 0x20000
} DDS_FLAGS2;

typedef enum : DWORD {
	CUBE_MAP = 0x1008,
	TEXTURE_W_MIPMAPS = 0x401008
} DDS_CAPS1;

typedef enum : DWORD {
	CUBEMAP = 0x200,
	CUBEMAP_POSITIVE_X = 0x400,
	CUBEMAP_NEGATIVE_X = 0x800,
	CUBEMAP_POSITIVE_Y = 0x1000,
	CUBEMAP_NEGATIVE_Y = 0x2000,
	CUBEMAP_POSITIVE_Z = 0x4000,
	CUBEMAP_NEGATIVE_Z = 0x8000,
	VOLUME = 0x200000
} DDS_CAPS2;

typedef enum : DWORD {
	D3D10_RESOURCE_DIMENSION_UNKNOWN	= 0,
	D3D10_RESOURCE_DIMENSION_BUFFER		= 1,
	D3D10_RESOURCE_DIMENSION_TEXTURE1D 	= 2,
	D3D10_RESOURCE_DIMENSION_TEXTURE2D 	= 3,
	D3D10_RESOURCE_DIMENSION_TEXTURE3D 	= 4
} D3D10_RESOURCE_DIMENSION;

typedef enum : DWORD {
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
} DXGI_FORMAT;

typedef enum : DWORD {
	D3D10_RESOURCE_MISC_GENERATE_MIPS = 0x1L,
	D3D10_RESOURCE_MISC_SHARED = 0x2L,
	D3D10_RESOURCE_MISC_TEXTURECUBE = 0x4L,
	D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x10L,
	D3D10_RESOURCE_MISC_GDI_COMPATIBLE = 0x20L
} D3D10_RESOURCE_MISC_FLAG;

typedef enum : DWORD {
	D3D11_RESOURCE_MISC_GENERATE_MIPS = 0x1L,
	D3D11_RESOURCE_MISC_SHARED = 0x2L,
	D3D11_RESOURCE_MISC_TEXTURECUBE = 0x4L,
	D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS = 0x10L,
	D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS = 0x20L,
	D3D11_RESOURCE_MISC_BUFFER_STRUCTURED = 0x40L,
	D3D11_RESOURCE_MISC_RESOURCE_CLAMP = 0x80L,
	D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x100L,
	D3D11_RESOURCE_MISC_GDI_COMPATIBLE = 0x200L,
	D3D11_RESOURCE_MISC_SHARED_NTHANDLE = 0x800L,
	D3D11_RESOURCE_MISC_RESTRICTED_CONTENT = 0x1000L,
	D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE = 0x2000L,
	D3D11_RESOURCE_MISC_RESTRICT_SHARED_RESOURCE_DRIVER = 0x4000L,
	D3D11_RESOURCE_MISC_GUARDED = 0x8000L,
	D3D11_RESOURCE_MISC_TILE_POOL = 0x20000L,
	D3D11_RESOURCE_MISC_TILED = 0x40000L,
	D3D11_RESOURCE_MISC_HW_PROTECTED = 0x80000L,
	D3D11_RESOURCE_MISC_SHARED_DISPLAYABLE,
	D3D11_RESOURCE_MISC_SHARED_EXCLUSIVE_WRITER,
	D3D11_RESOURCE_MISC_NO_SHADER_ACCESS
} D3D11_RESOURCE_MISC_FLAG;

typedef union {
	D3D10_RESOURCE_MISC_FLAG flags10;
	D3D11_RESOURCE_MISC_FLAG flags11;
} DDS_RESOURCE_MISC_FLAG;

typedef enum : DWORD {
	DDS_ALPHA_MODE_UNKNOWN			= 0x0,
	DDS_ALPHA_MODE_STRAIGHT			= 0x1,
	DDS_ALPHA_MODE_PREMULTIPLIED	= 0x2,
	DDS_ALPHA_MODE_OPAQUE 			= 0x3,
	DDS_ALPHA_MODE_CUSTOM 			= 0x4,
} DDS_D3D10_MISC_FLAGS2;

typedef struct {
	DWORD		dwHeaderSize;
	DDS_FLAGS1	dwFlags1;
	DWORD		dwHeight;
	DWORD		dwWidth;
	DWORD		dwPitchOrLinearSize;
	DWORD		dwDepth;
	DWORD		dwMipMapCount;
	DWORD		dwReserved[11];
	DWORD		dwSize;
	DDS_FLAGS2	dwFlags2;
	CCHAR		dwFourCC[4];
	DWORD		dwRgbBitCount;
	DWORD 		dwRBitMask;
	DWORD 		dwGBitMask;
	DWORD 		dwBBitMask;
	DWORD 		dwABitMask;
	DDS_CAPS1 	dwCaps;
	DDS_CAPS2 	dwCaps2;
	DWORD 		dwCaps3;
	DWORD 		dwCaps4;
	DWORD 		dwReserved2;
} DDS_HEADER, *LP_DDS_HEADER;

typedef struct {
	DXGI_FORMAT				 dxgiFormat;
	D3D10_RESOURCE_DIMENSION resourceDimension;
	DDS_RESOURCE_MISC_FLAG	 miscFlag;
	UINT					 arraySize;
	UINT					 miscFlags2;
} DDS_HEADER_DXT10, *LP_DDS_HEADER_DXT10;

typedef struct {
	DWORD			 dwMagic; // must be 0x20534444
	DDS_HEADER		 header;
	DDS_HEADER_DXT10 header10;
	LPBYTE			 data;
} DDS_FILE, *LP_DDS_FILE;

typedef struct {
	DWORD dwInternalFormat;
	DWORD dwBlockSize;
} DDS_GL_FORMAT, *LP_DDS_GL_FORMAT;

static void ddsDumpHeader(DDS_HEADER const *dds) {
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

static Error dxgi2glFormat(DXGI_FORMAT const dxgiFormat, LP_DDS_GL_FORMAT const format) {
	switch (dxgiFormat) {
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			*format = {
				.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
				.dwBlockSize = 8
			};
			return OK;
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
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
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
			*format = {
				.dwInternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM,
				.dwBlockSize = 16
			};
			return OK;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			*format = {
				.dwInternalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
				.dwBlockSize = 16
			};
			return OK;
		default:
			return ERR_FILE_CANT_READ;
	}
}

static Error d3d9format2glFormat(LPCH const wc4, LP_DDS_GL_FORMAT const lpGlFormat) {
	switch (hash(wc4)) {
		case hash("DXT1"):
			*lpGlFormat = {
				.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
				.dwBlockSize = 8
			};
			return OK;
		case hash("DXT3"):
			*lpGlFormat = {
				.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
				.dwBlockSize = 16
			};
			return OK;
		case hash("DXT5"):
			*lpGlFormat = {
				.dwInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
				.dwBlockSize = 16
			};
			return OK;
		default:
			return ERR_FILE_CORRUPT;
	}
}

Error uploadDdsFromStdio(FILE *file, u32 const texture_object) {
	if (fseek(file, 0, SEEK_END) != 0) return ERR_FILE_NOT_FOUND;
	u32 const uiFileSize = ftell(file);
	if (fseek(file, 0, SEEK_SET) != 0) return ERR_FILE_NOT_FOUND;

	DDS_FILE dds;
	DDS_HEADER *ddsFileHeaderInfo = &dds.header;
	ZeroMemory(ddsFileHeaderInfo, sizeof(DDS_HEADER));

	UINT64 ullBufferSize = uiFileSize - sizeof(DWORD) - sizeof(DDS_HEADER);
	
	if (fread_s(&dds.dwMagic, sizeof(DWORD), sizeof(DWORD), 1, file) != 1)
		return ERR_FILE_CANT_READ;
	
	if (dds.dwMagic != static_cast<DWORD>(0x20534444)) {
		// "DDS "
		printf("File is not a valid DDS file (magic number mismatch) %u\n", dds.dwMagic);
		return ERR_FILE_UNRECOGNIZED;
	}
	
	if (fread_s(ddsFileHeaderInfo, sizeof(DDS_HEADER), sizeof(DDS_HEADER), 1, file) != 1)
		return ERR_FILE_CORRUPT;
	
	//ddsDumpHeader(ddsFileHeaderInfo);

	DDS_GL_FORMAT glFormat;
	ZeroMemory(&glFormat, sizeof(glFormat));
	if (hash(ddsFileHeaderInfo->dwFourCC) == hash("DX10")) {
		if (fread_s(&dds.header10, sizeof(DDS_HEADER_DXT10), sizeof(DDS_HEADER_DXT10), 1, file) != 1)
			return ERR_FILE_CORRUPT;

		ullBufferSize -= sizeof(DDS_HEADER_DXT10);
		
		if (Error const err = dxgi2glFormat(dds.header10.dxgiFormat, &glFormat); err != OK)
			return err;
	}
	else if (Error const err = d3d9format2glFormat(ddsFileHeaderInfo->dwFourCC, &glFormat); err != OK)
		return err;

	glTextureStorage2D(texture_object,
	                   (GLsizei)ddsFileHeaderInfo->dwMipMapCount,
	                   glFormat.dwInternalFormat,
	                   (GLsizei)ddsFileHeaderInfo->dwWidth,
	                   (GLsizei)ddsFileHeaderInfo->dwHeight);
	gpu_check;

	glTextureParameteri(texture_object, GL_TEXTURE_BASE_LEVEL, 0);gpu_check;
	glTextureParameteri(texture_object, GL_TEXTURE_MAX_LEVEL, (GLint)(ddsFileHeaderInfo->dwMipMapCount - 1));gpu_check;
	glTextureParameteri(texture_object, GL_TEXTURE_MAG_FILTER, GL_LINEAR);gpu_check;
	glTextureParameteri(texture_object, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);gpu_check;
	glTextureParameteri(texture_object, GL_TEXTURE_WRAP_S, GL_REPEAT);gpu_check;
	glTextureParameteri(texture_object, GL_TEXTURE_WRAP_T, GL_REPEAT);gpu_check;

	auto const buffer = (u8*)std::malloc(ullBufferSize);
	size_t const uiBufferSize = ullBufferSize;
	assert(fread_s(buffer, ullBufferSize, ullBufferSize, 1, file) == 1);
	
	GLsizeiptr uiOffset = 0;
	GLsizei uiImageSize = 0;
	GLsizei uiWidth = (GLsizei)ddsFileHeaderInfo->dwWidth;
	GLsizei uiHeight = (GLsizei)ddsFileHeaderInfo->dwHeight;
	
	for (GLint iTextureLevel = 0; iTextureLevel < static_cast<GLint>(ddsFileHeaderInfo->dwMipMapCount); iTextureLevel++) {
		uiImageSize = (GLsizei)((uiWidth + 3) / 4 * ((uiHeight + 3) / 4) * glFormat.dwBlockSize);

		glCompressedTextureSubImage2D(texture_object, iTextureLevel,
			0, 0, uiWidth, uiHeight,
			glFormat.dwInternalFormat, uiImageSize,
			buffer + uiOffset);
		gpu_check;
		
		uiOffset += uiImageSize;
		uiWidth /= 2;
		uiHeight /= 2;
	}

	std::free(buffer);

	return OK;
}