#include "mono_bridge.hpp"

#include <cassert>

#include "util.hpp"
#include "clr/hostfxr.h"
#include "clr/coreclr_delegates.h"

#include "engine/filesystem.hpp"


MonoAssembly::MonoAssembly(assembly_handle const assembly): assembly(assembly) {}
MonoAssembly::MonoAssembly(std::string_view assembly_name) {
	MonoBridge const &bridge = MonoBridge::singleton();
	std::string const assem(assembly_name.data(), assembly_name.size());
	assert(bridge.load_assembly(stringToWideString(assem).c_str(), nullptr, nullptr));
}

//
// Mono Bridge
//

MonoBridge::MonoBridge() : hostfxr("C:\\Program Files\\dotnet\\host\\fxr\\10.0.3\\hostfxr.dll") {
	initialize_for_dotnet_command_line_ = hostfxr.findFunction<hostfxr_initialize_for_dotnet_command_line_fn>("hostfxr_initialize_for_dotnet_command_line");
	get_runtime_property_value_ = hostfxr.findFunction<hostfxr_get_runtime_property_value_fn>("hostfxr_get_runtime_property_value");
	set_runtime_property_value_ = hostfxr.findFunction<hostfxr_set_runtime_property_value_fn>("hostfxr_set_runtime_property_value");
	get_runtime_properties_ = hostfxr.findFunction<hostfxr_get_runtime_properties_fn>("hostfxr_get_runtime_properties");
	get_runtime_delegate_ = hostfxr.findFunction<hostfxr_get_runtime_delegate_fn>("hostfxr_get_runtime_delegate");
	set_error_writer_ = hostfxr.findFunction<hostfxr_set_error_writer_fn>("hostfxr_set_error_writer");
	close_ = hostfxr.findFunction<hostfxr_close_fn>("hostfxr_close");
	main_ = hostfxr.findFunction<hostfxr_main_fn>("hostfxr_main");

	assert(get_runtime_delegate(
		nullptr,
		hdt_load_assembly_and_get_function_pointer,
		reinterpret_cast<void**>(&load_assembly_and_get_function_pointer_)
	));
	assert(load_assembly_and_get_function_pointer_ != nullptr);

	assert(get_runtime_delegate(
		nullptr,
		hdt_get_function_pointer,
		reinterpret_cast<void**>(&get_function_pointer_)
	));
	assert(get_function_pointer_ != nullptr);

	assert(get_runtime_delegate(
		nullptr,
		hdt_load_assembly_bytes,
		reinterpret_cast<void**>(&load_assembly_bytes_)
	));
	assert(load_assembly_bytes_ != nullptr);

	assert(get_runtime_delegate(
		nullptr,
		hdt_load_assembly,
		reinterpret_cast<void**>(&load_assembly_)
	));
	assert(load_assembly_ != nullptr);
}

MonoBridge & MonoBridge::singleton() {
	static MonoBridge singleton;
	return singleton;
}

int32_t MonoBridge::initialize_for_dotnet_command_line(int const argc, char_t const **argv, hostfxr_initialize_parameters const *parameters, hostfxr_handle*host_context_handle) const {
	return initialize_for_dotnet_command_line_(argc, argv, parameters, host_context_handle);
}
int32_t MonoBridge::get_runtime_property_value(hostfxr_handle const handle, char_t const *name, char_t const **value) const {
	return get_runtime_property_value_(handle, name, value);
}
int32_t MonoBridge::set_runtime_property_value(hostfxr_handle const handle, char_t const *name, char_t const *value) const {
	return set_runtime_property_value_(handle, name, value);
}
int32_t MonoBridge::get_runtime_properties(hostfxr_handle const handle, size_t *count, char_t const **names, char_t const **values) const {
	return get_runtime_properties_(handle, count, names, values);
}

int32_t MonoBridge::get_runtime_delegate(hostfxr_handle const handle, hostfxr_delegate_type const type, void **out_delegate) const {
	return get_runtime_delegate_(handle, type, out_delegate);
}

hostfxr_error_writer_fn MonoBridge::set_error_writer(hostfxr_error_writer_fn const error_writer) const {
	return set_error_writer_(error_writer);
}
int32_t MonoBridge::close(hostfxr_handle const handle) const {
	return close_(handle);
}
int32_t MonoBridge::main(int const argc, char_t const **argv) const {
	return main_(argc, argv);
}
int32_t MonoBridge::get_function_pointer(char_t const *type_name, char_t const *method_name, char_t const *delegate_type_name, void *load_context, void *reserved, void **out_delegate) const {
	return get_function_pointer_(type_name, method_name, delegate_type_name, load_context, reserved, out_delegate);
}
int32_t MonoBridge::load_assembly(char_t const *assembly_path, void *load_context, void *reserved) const {
	return load_assembly_(assembly_path, load_context, reserved);
}
int32_t MonoBridge::load_assembly_bytes(void const *assembly_bytes, size_t assembly_bytes_len, void const *symbols_bytes, size_t symbols_bytes_len, void *load_context, void *reserved) const {
	return load_assembly_bytes_(assembly_bytes, assembly_bytes_len, symbols_bytes, symbols_bytes_len, load_context, reserved);
}
int32_t MonoBridge::load_assembly_and_get_function_pointer(char_t const *assembly_path, char_t const *type_name, char_t const *method_name, char_t const *delegate_type_name, void *reserved, void **out_delegate) const {
	return load_assembly_and_get_function_pointer_(assembly_path, type_name, method_name, delegate_type_name, reserved, out_delegate);
}