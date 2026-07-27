#pragma once

#include "types.hpp"

struct alignas(8) RID
{
	u32 upper;
	u32 lower;
	
	RID() : upper(0), lower(0) {
	}
	
	RID(const u32 up, const u32 lo) : upper(up), lower(lo) {}

	RID(const u64 value) {
		upper = static_cast<u32>((value & 0xFFFFFFFF00000000) << 32);
		lower = static_cast<u32>((value & 0x00000000FFFFFFFF));
	}
	
	operator u64() const
	{
		u64 output = lower;
		output += static_cast<u64>(upper) << 32;
		return output;
	}
	
	[[nodiscard]] bool operator==(const RID rhs) const
	{
		return lower == rhs.lower && upper == rhs.upper;
	}
};