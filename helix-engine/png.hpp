#pragma once

// I really want to write a C++ PNG parse library, and so I'm giving it a shot.

#include <filesystem>
#include <vector>

#include "types.hpp"

namespace png {

	//< Found in IHDR chunk.
	enum class ColorType_e : u8 { //< u8 as prescribed in the png spec.
		Greyscale = 0,
		Truecolor = 2,
		IndexedColor = 3,
		GreyscaleWithAlpha = 4,
		TruecolorWithAlpha = 5,
	};

	enum class CompressionMethod_e : u8 {
		Deflate = 0, //< Only prescribed compression method in the specification
	};

	enum class FilterMethod_e : u8 {
		Adaptive = 0, //< Only prescribed filtering method in the specification
	};

	enum class InterlaceMethod_e : u8 {
		NoInterlace = 0,
		Adam7 = 1
	};

	//< Aka IHDR.
	constexpr char ImageHeader[] = "IHDR";
	struct ImageHeader_t {
		u32 width;
		u32 height;
		u8 bit_depth;
		ColorType_e color_type;
		CompressionMethod_e compression_method;
		FilterMethod_e filter_method;
		InterlaceMethod_e interlace_method;
	};

	struct PaletteEntry_t {
		u8 red, green, blue;
	};

	constexpr char PaletteChunkHeader[] = "PLTE";
	struct PaletteChunk_t {
		PaletteEntry_t entries[1 << 8]; //< 256 for a 8bit image, which is common.
	};

	struct WidePaletteChunk_t {
		PaletteEntry_t entries[1 << 16]; //< 65536 entries for a 16-bit image, not all need to be used.
	};

	template <typename T>
	concept PaletteChunk = requires(T pc) {
		(pc.entries[0]) -> PaletteEntry_t; //< Must have an array field named entries that contains PaletteEntry_t.
	};

	struct TransparencyInformation_t { //< TODO: needs implementation
	};

	constexpr char PrimaryChromaticitiesAndWhitePoint[] = "cHRM";
	struct PrimaryChromaticitiesAndWhitePoint_t { //< Values are stored as X or Y multiplied by 100,000.
		u32 white_point_x, white_point_y;
		u32 red_point_x, red_point_y;
		u32 green_point_x, green_point_y;
		u32 blue_point_x, blue_point_y;
	};
	
	constexpr char SignificantBits[] = "sBIT";
	struct SignificantBits_t {
		struct colorType0_s {
			u8 significant_grey_scale_bits;
		} colorType0;
		struct colorType2_3_s {
			u8 significant_red_bits;
			u8 significant_green_bits;
			u8 significant_blue_bits;
		} colorType2_3;
		struct colorType4_s {
			u8 significant_grey_scale_bits;
			u8 significant_alpha_bits;
		} colorType4;
		struct colorType6_s {
			u8 significant_red_bits;
			u8 significant_green_bits;
			u8 significant_blue_bits;
			u8 significant_alpha_bits;
		};
		union { //< Stupid name scheme but it'll do honestly.
			colorType0_s color_type_0;
			colorType2_3_s color_type_2_3;
			colorType4_s color_type_4;
			colorType6_s color_type_6;
		};
	};

	struct ChunkFrontInfo_t {
		u32 length;
		char chunk_type[4];
	};

	constexpr char DataChunk[] = "IDAT";

	template <typename T>
	struct result {
		Error error;
		int failed_at = 0;
		bool has_value;
		std::optional<T> value;

		result(Error e = FAILED, int fail = 0) : error(e), failed_at(fail), has_value(false) {}
		result(T &&v) : error(OK), has_value(true), value(std::move(v)) {}

		[[nodiscard]] T unwrap() { return value.value(); }
		// ReSharper disable once CppNonExplicitConversionOperator
		[[nodiscard]] operator T() { return unwrap(); }
	};

	constexpr char PngSignature[] = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A";
	
	extern result<std::vector<u8>> decode(std::filesystem::path path);
}
