#pragma once

#include <string>

constexpr std::string wstringToString(std::wstring const & ws) {
	std::string s;
	s.reserve(ws.length()); // Use of reserve is constexpr in C++20

	for (wchar_t const wc : ws) {
		// Simple check for ASCII range
		if (wc <= 0x7F) {
			s += static_cast<char>(wc);
		} else {
			// Cannot convert non-ASCII characters in this simple constexpr manner
			// In a real constexpr function, you might need a more sophisticated approach
			// or a static_assert if the input is expected to be pure ASCII.
			// Throwing an exception is allowed in constexpr functions if caught within the constant evaluation.
			// For a compile-time constant, the condition must always be met.
		}
	}
	return s;
}


#include <climits> // For CHAR_BIT
template<class T>
constexpr T byteswap(T i, T j = 0u, std::size_t const n = 0u) requires (std::is_unsigned_v<T>) {
	return n == sizeof(T) ?
		j : byteswap<T>(i >> CHAR_BIT, (j << CHAR_BIT) | (i & static_cast<T>(static_cast<unsigned char>(-1))), n + 1);
}

// for 4-char headers to become compile time switch arguments
template <typename T, std::size_t N>
constexpr T charsToType(char const(&c)[N], T j = 0u, std::size_t const n = 0u) {
	return n == sizeof(T) || c[n] == '\0' ?
		j : charsToType<T, N>(c, j | static_cast<T>(c[n]) << (CHAR_BIT * n), n + 1);
}