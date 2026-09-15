#pragma once
#include "types.hpp"
#include "engine/object.hpp"

class Asset;

class Importer : public Object {
public:
	using SelfType = Importer;
	
	void set(const std::string_view property, const std::any &value) override;
	std::any get(const std::string_view property) const override;
	
	[[nodiscard]] virtual bool handles_file(const Path& path) const;
	[[nodiscard]] virtual const Vector<String> &get_recognized_extensions() const;
	[[nodiscard]] virtual SharedPtr<Asset> import(const Path& path);
};