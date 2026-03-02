#include "png.hpp"

#include <cassert>
#include <iostream>

#include "util.hpp"

// for png only.
#define png_debug(str) (printf("[%s:%d] %s\n", &std::string(__FILE__)[42], __LINE__, str))
#define png_debugf(str, ...) (printf("[%s:%d] ", &std::string(__FILE__)[42], __LINE__), printf(str, __VA_ARGS__), printf("\n"))
#define png_assert(CND, ERR) if (!(CND)){fclose(file); png_debugf("Assertion failed! %s", to_string((ERR))); return (ERR);}


namespace {

	inline void process_ihdr() {}
	
}

png::result<std::vector<unsigned char>> png::decode(std::filesystem::path path) {

	std::vector<unsigned char> data;
	size_t rsz;
	
	FILE *file;
	std::cout << path.string() << std::endl;
	
	errno_t result = fopen_s(&file, path.string().c_str(), "rb");
	png_assert(result == 0, ERR_CANT_OPEN)
	{ //< Don't need signature after this point so keep it in it's own scope.
		char signature[8];
		rsz = fread_s(signature, 8, 1, 8, file);
		png_debugf("%X", *signature);
		png_assert(rsz == 8, ERR_INVALID_DATA)
	png_debugf("%llu", charsToType<u64>(signature));
	png_debugf("%llu", charsToType<u64>(PngSignature));
		png_assert(charsToType<u64>(signature) == charsToType<u64>(PngSignature), ERR_INVALID_DATA)
	}
	
	ImageHeader_t header{};
	SignificantBits_t significant_bits{};
	while (!feof(file)) {
		u32 chunkLength = 0;
		rsz = fread_s(&chunkLength, sizeof(u32), sizeof(u32), 1ull, file);
		png_assert(rsz == 1, ERR_INVALID_DATA)
		png_assert(chunkLength != 0, ERR_INVALID_DATA)

		char chunkType[5] = {};
		rsz = fread_s(chunkType, 5, 1, 4, file);
		png_assert(rsz == 4, ERR_INVALID_DATA)
		chunkType[4] = '\0'; // ensure null termination
		png_debugf("%s", chunkType);

		// ReSharper disable once CppCStyleCast
		switch (auto const chunk_type_n = charsToType<u32>(chunkType); chunk_type_n) {
			case charsToType<u32>(ImageHeader): {
				rsz = fread_s(&header, sizeof(ImageHeader_t), sizeof(ImageHeader_t), 1, file);
				png_assert(rsz == 1, ERR_INVALID_DATA)
				header.width = byteswap(header.width);
				header.height = byteswap(header.height);
				header.bit_depth = byteswap(header.bit_depth);
				header.color_type = (ColorType_e)byteswap(static_cast<u8>(header.color_type));
				header.compression_method = static_cast<CompressionMethod_e>(byteswap(static_cast<u8>(header.compression_method)));
				header.filter_method = static_cast<FilterMethod_e>(byteswap(static_cast<u8>(header.filter_method)));
				header.interlace_method = InterlaceMethod_e::NoInterlace;
				break;
			}
			case charsToType<u32>(PaletteChunkHeader): {
				
				break;
			}
			case charsToType<u32>(SignificantBits): {
				rsz = fread_s(&significant_bits, 10, 3, 1, file);
				png_assert(rsz == 3, ERR_INVALID_DATA)
				break;
			}
			case charsToType<u32>(DataChunk): {
				png_debug("data chunk hunk");
				data.reserve(chunkLength);
				rsz = fread_s(data.data(), chunkLength, 1, chunkLength, file);
				png_assert(rsz == chunkLength, ERR_INVALID_DATA)
				break;
			}
			default: {
				png_debug("default...");
				std::vector<u8> chunk(chunkLength);
				rsz = fread_s(chunk.data(), chunkLength, 1, chunkLength, file);
				png_assert(rsz == chunkLength, ERR_INVALID_DATA)
				
				break;
			}
		}
	}
	
	result = fclose(file);
	png_assert(result == 0, ERR_CANT_RESOLVE)
	
	// Should be IHDR
	return data;
}
