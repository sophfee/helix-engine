#pragma once

#include <cstdint>

struct alignas(8) RID
{
	
	uint32_t upper;
	uint32_t lower;
	
	constexpr RID() : upper(0), lower(0) {
	}
	
	constexpr RID(const uint32_t up, const uint32_t lo) : upper(up), lower(lo) {}

	constexpr RID(const uint64_t value) {
		upper = static_cast<uint32_t>((value & 0xFFFFFFFF00000000) << 32);
		lower = static_cast<uint32_t>((value & 0x00000000FFFFFFFF));
	}
	
	constexpr operator uint64_t() const
	{
		uint64_t output = lower;
		output += static_cast<uint64_t>(upper) << 32;
		return output;
	}
	
	constexpr [[nodiscard]] bool operator==(const RID rhs) const
	{
		return lower == rhs.lower && upper == rhs.upper;
	}
	
	constexpr bool valid() const {
		return lower > 0;
	}
};
