#pragma once
#include <any>
#include <chrono>

#include "types.hpp"
#include <string_view>

class Asset {
public:
	Asset() = default;
	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;
	Asset(Asset&&) = delete;
	Asset& operator=(Asset&&) = delete;
	virtual ~Asset() = default;

	[[nodiscard]] StringView get_name() const;
	void set_name(StringView name);
	
	[[nodiscard]] Path get_path() const;
	void set_path(const Path& path);
	
	[[nodiscard]] Path get_import_path() const;
	void set_import_path(const Path& path);
	
	[[nodiscard]] bool is_subresource() const;
	
	[[nodiscard]] virtual StringView get_base_extension() const;
	[[nodiscard]] virtual std::chrono::system_clock::time_point get_last_modified_time() const;
	[[nodiscard]] virtual std::chrono::system_clock::time_point get_last_import_time() const;
	
	virtual void get_subresources(Vector<Asset*>& out) const;

private:
	String name_;
	Path path_;
	Path import_path_;
	
	std::chrono::system_clock::time_point last_modified_time_;
	std::chrono::system_clock::time_point last_import_time_;
};