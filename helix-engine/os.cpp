#include "os.hpp"

#include <Windows.h>

std::wstring os::getEnvironmentVariable(std::wstring_view const name) {
	std::wstring const nt_name(name.data(), name.length());
	DWORD const length = GetEnvironmentVariable(nt_name.c_str(), nullptr, 0);
	if (length == 0) { return TEXT("failed"); }
	// returns length w/o null terminator
	auto const buffer = new WCHAR[length + 1];
	std::memset(buffer, 0, length + 1);
	GetEnvironmentVariable(nt_name.c_str(), buffer, length);
	std::wstring result(buffer, length);
	delete[] buffer;
	return result;
}

std::wstring os::getCurrentDirectory() {
	DWORD const length = GetCurrentDirectory(0, nullptr);
	if (length == 0) { return TEXT("failed"); }
	auto const buffer = new WCHAR[length];
	std::memset(buffer, 0, length);
	GetCurrentDirectory(length, buffer);
	std::wstring result(buffer, length);
	delete[] buffer;
	return result;
}
