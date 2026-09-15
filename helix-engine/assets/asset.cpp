#include "asset.hpp"

StringView Asset::get_name() const {
	return name_;
}

void Asset::set_name(StringView name) {
	name_ = name;
}

Path Asset::get_path() const {
	return path_;
}

void Asset::set_path(const Path &path) {
	path_ = path;
}

Path Asset::get_import_path() const {
	return import_path_;
}

void Asset::set_import_path(const Path &path) {
	import_path_ = path;
}

bool Asset::is_subresource() const {
	String path_string = path_.string();
	const std::size_t pos = path_string.find(':');
	return is_empty(path_) || pos != String::npos;
}

StringView Asset::get_base_extension() const {
	return "asset";
}

std::chrono::system_clock::time_point Asset::get_last_modified_time() const {
	return last_modified_time_;
}

std::chrono::system_clock::time_point Asset::get_last_import_time() const {
	return last_import_time_;
}

void Asset::get_subresources(Vector<Asset *> &out) const {
}
