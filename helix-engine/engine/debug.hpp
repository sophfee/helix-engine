#pragma once
#include <format>
#include <iostream>
#include <string_view>

#ifdef _DEBUG_PRINTING
namespace helix {
	template <size_t N, typename ...TArgs>
	void print(const char (&fmt)[N], TArgs &&... args) {
		std::cout << std::format(fmt, std::make_format_args(args...));
	}
	template <size_t N, typename ...TArgs>
	void print(const wchar_t (&fmt)[N], TArgs &&... args) {
		std::wcout << std::format(fmt, std::make_format_args(args...));
	}
}

#define helix_print(fmt, ...) \
	do{ \
	std::cout << std::format("[{}:{}] " fmt "\n", __FILE__, __LINE__, std::make_format_args(__VA_ARGS__)); \
	} while(0)

#else
#define helix_print(...)
#endif