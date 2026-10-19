#pragma once
#include "types.hpp"
#include "engine/object.hpp"

class Asset;

class Importer : public Object {
public:
	using SelfType = Importer;
	
	void set(const std::string_view property, const Any &value) override;
	[[nodiscard]] Any get(const std::string_view property) const override;
	
	[[nodiscard]] virtual bool handles_file(const Path& path) const;
	[[nodiscard]] virtual const Vector<String> &get_recognized_extensions() const;
	[[nodiscard]] virtual Future<Asset*> import(const Path& path);
	
protected:
	virtual Asset* import_async(const Path& path) = 0;
};

/**
 * \brief This imports types that are
 */
class EngineImporter : public Importer {
	
};