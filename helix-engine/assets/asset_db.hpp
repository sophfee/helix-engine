#pragma once

#include "engine/object.hpp"

class Asset;

class AssetDB : public Object {
public:
	AssetDB() = default;
	AssetDB(AssetDB&&) = delete;
	AssetDB(const AssetDB&) = delete;
	AssetDB& operator=(AssetDB&&) = delete;
	AssetDB& operator=(const AssetDB&) = delete;
	~AssetDB() override = default;

	void add_asset(Asset* asset);
	void remove_asset(Asset* asset);
	[[nodiscard]] Asset* get_asset(const StringView name) const;
	[[nodiscard]] Vector<Asset*> get_assets() const;

	StringView get_class_name() const override;
	void set(const std::string_view property, const Any &value) override;
	[[nodiscard]] Any get(const std::string_view property) const override;
	
private:
	Vector<Asset*> assets_;
};
