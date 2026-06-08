#include "library.hpp"

#include "filesystem.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

Library::Library(std::string_view const path) {
	std::string const null_terminated_path(path.data(), path.size());
	handle = LoadLibraryA(null_terminated_path.c_str());
	if (!handle) throw std::runtime_error("Failed to load library");
}

Library::~Library() {
	if (handle) _LIKELY FreeLibrary(static_cast<HMODULE>(handle));
}

void Library::load(std::string_view path) {
	if (handle) {
		FreeLibrary(static_cast<HMODULE>(handle));
		handle = nullptr;
	}
	std::string const null_terminated_path(path.data(), path.size());
	handle = LoadLibraryA(null_terminated_path.c_str());
	if (!handle) throw std::runtime_error("Failed to load library");
}

Library::generic_fn Library::findFunction(std::string_view const name) const {
	std::string const null_terminated_path(name.data(), name.size());
	FARPROC const addr = GetProcAddress(handle, null_terminated_path.c_str());
	return addr;
}