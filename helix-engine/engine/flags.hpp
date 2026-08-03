#pragma once
#include <type_traits>

#include "types.hpp"

template <typename T>
class BitFlag {
	static_assert(std::is_enum_v<T>, "BitFlag can only be used with enum types");
	std::uint64_t value = 0;

public:
	constexpr void set(BitFlag p_flag) { value |= p_flag.value; }
	constexpr void clear(BitFlag p_flag) { value &= ~p_flag.value; }
	constexpr void toggle(BitFlag p_flag) { value ^= p_flag.value; }
	constexpr void reset() { value = 0; }
	
	constexpr BitFlag operator=(BitFlag p_flag) const { BitFlag r{*this}; r.value = p_flag.value; return r; }
	constexpr BitFlag operator|(BitFlag p_flag) const { BitFlag r{*this}; r.value |= p_flag.value; return r; }
	constexpr BitFlag operator&(BitFlag p_flag) const { BitFlag r{*this}; r.value &= p_flag.value; return r; }
	constexpr BitFlag operator^(BitFlag p_flag) const { BitFlag r{*this}; r.value ^= p_flag.value; return r; }
	constexpr BitFlag operator~() const { BitFlag r{*this}; r.value = ~r.value; return r; }
	
	constexpr BitFlag& operator|=(BitFlag p_flag) { value |= p_flag.value; return *this; }
	constexpr BitFlag& operator&=(BitFlag p_flag) { value &= p_flag.value; return *this; }
	constexpr BitFlag& operator^=(BitFlag p_flag) { value ^= p_flag.value; return *this; }

	[[nodiscard]] constexpr bool has(const BitFlag p_flag) const {
		return (value & p_flag.value) == p_flag.value;
	}
	
	constexpr BitFlag() = default;
	constexpr BitFlag(T p_flag) : value(static_cast<std::uint64_t>(p_flag)) {}
	constexpr operator T() const { return static_cast<T>(value); }
	template <typename V> requires (std::is_arithmetic_v<V> && std::convertible_to<V, int>)
	constexpr BitFlag(V p_value) : value(static_cast<std::uint64_t>(p_value)) {}
	
	template <typename V> requires (std::is_arithmetic_v<V> && std::convertible_to<V, int>)
	constexpr operator V() const { return static_cast<V>(value); }
};

template <typename T>
concept enum_class =
	   std::is_enum_v<T>
	&& !std::is_convertible_v<T, std::underlying_type_t<T>>;

// extension point
template <enum_class T>
inline constexpr bool enable_enum_bitops = false;

template <typename T>
concept enum_bitops = enum_class<T> && enable_enum_bitops<T>;

template <enum_bitops T>
[[nodiscard]] constexpr T operator|(T const a, T const b) noexcept
{
	using I = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<I>(a) | static_cast<I>(b));
}

template <enum_bitops T>
[[nodiscard]] constexpr T operator&(T const a, T const b) noexcept
{
	using I = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<I>(a) & static_cast<I>(b));
}

template <enum_bitops T>
constexpr T& operator|=(T& a, T const b) noexcept
{
	return a = a | b;
}

template <enum_bitops T>
constexpr T& operator&=(T& a, T const b) noexcept
{
	return a = a & b;
}

template <enum_bitops T>
[[nodiscard]] constexpr T operator~(T const a) noexcept
{
	using I = std::underlying_type_t<T>;
	return static_cast<T>(~static_cast<I>(a));
}