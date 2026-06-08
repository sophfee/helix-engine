#pragma once

#include <string_view>

#include "clr/coreclr_delegates.h"
#include "clr/hostfxr.h"
#include "engine/library.hpp"


using assembly_handle = void*;



class MonoAssembly {
	assembly_handle assembly = nullptr;
	hostfxr_handle hostfxr = nullptr;

public:
	MonoAssembly(assembly_handle assembly);
	MonoAssembly(std::string_view assembly_name);
	
	MonoAssembly(MonoAssembly const &) = delete;
	MonoAssembly& operator=(MonoAssembly const &) = delete;
	MonoAssembly(MonoAssembly &&) = default;
	MonoAssembly& operator=(MonoAssembly &&) = default;
};

class MonoBridge {
	Library hostfxr;
	
	hostfxr_initialize_for_dotnet_command_line_fn	initialize_for_dotnet_command_line_ = nullptr;
	hostfxr_get_runtime_property_value_fn	get_runtime_property_value_ = nullptr;
	hostfxr_set_runtime_property_value_fn	set_runtime_property_value_ = nullptr;
	hostfxr_get_runtime_properties_fn	get_runtime_properties_ = nullptr;
	hostfxr_get_runtime_delegate_fn	get_runtime_delegate_ = nullptr;
	hostfxr_set_error_writer_fn	set_error_writer_ = nullptr;
	hostfxr_close_fn	close_ = nullptr;
	hostfxr_main_fn	main_ = nullptr;

	load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer_ = nullptr;
	get_function_pointer_fn get_function_pointer_ = nullptr;
	load_assembly_bytes_fn load_assembly_bytes_ = nullptr;
	load_assembly_fn load_assembly_ = nullptr;
	

protected:

	int32_t initialize_for_dotnet_command_line(int argc, char_t const **argv, hostfxr_initialize_parameters const *parameters, hostfxr_handle *host_context_handle) const;
	int32_t get_runtime_property_value(hostfxr_handle const handle, char_t const *name, char_t const **value) const;
	int32_t set_runtime_property_value(hostfxr_handle const handle, char_t const *name, char_t const *value) const;
	int32_t get_runtime_properties(hostfxr_handle const handle, size_t *count, char_t const **names, char_t const **values) const;
	int32_t get_runtime_delegate(hostfxr_handle const handle, hostfxr_delegate_type type, void ** out_delegate) const;
	hostfxr_error_writer_fn set_error_writer(hostfxr_error_writer_fn error_writer) const;
	int32_t close(hostfxr_handle const handle) const;
	int32_t main(int argc, char_t const **argv) const;
	int32_t get_function_pointer(char_t const *type_name, char_t const *method_name, char_t const *delegate_type_name, void *load_context, void *reserved, void **out_delegate) const;
	int32_t load_assembly(char_t const *assembly_path, void *load_context, void *reserved) const;
	int32_t load_assembly_bytes(void const *assembly_bytes, size_t assembly_bytes_len, void const *symbols_bytes, size_t symbols_bytes_len, void *load_context, void *reserved) const;
	int32_t load_assembly_and_get_function_pointer(char_t const *assembly_path, char_t const *type_name, char_t const *method_name, char_t const *delegate_type_name, void *reserved, void **out_delegate) const;
	
public:
	MonoBridge();
	MonoBridge(MonoBridge const &) = delete;
	MonoBridge& operator=(MonoBridge const &) = delete;
	MonoBridge(MonoBridge &&) = default;
	MonoBridge& operator=(MonoBridge &&) = default;

	static [[nodiscard]] MonoBridge &singleton();

	friend class MonoAssembly;
};