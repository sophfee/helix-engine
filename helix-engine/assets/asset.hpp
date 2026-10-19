#pragma once
#include <any>
#include <chrono>

#include "types.hpp"
#include <string_view>

#include "engine/rc.hpp"

class Asset : public RefCounted {
public:
	Asset() = default;
	Asset(const Asset&) = delete;
	Asset& operator=(const Asset&) = delete;
	Asset(Asset&&) = delete;
	Asset& operator=(Asset&&) = delete;
	~Asset() override = default;

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

	[[nodiscard]] StringView get_class_name() const override;
	//[[nodiscard]] Any get(const StringView property) const override;
	//void set(const StringView property, const Any &value) override;

private:
	Asset* owner_ = nullptr;
	String name_;
	Path path_;
	Path import_path_;
	
	bool is_subresource_;

	/**
	 * \brief A stub asset only contains the basic information and is not yet loaded for use in the engine but exists in memory as an object
	 */
	bool is_stub_;
	
	std::chrono::system_clock::time_point last_modified_time_;
	std::chrono::system_clock::time_point last_import_time_;
};
