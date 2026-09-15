#include "importer.hpp"

#include "os.hpp"
#include "util.hpp"
#include "simdjson/simdjson.h"

void Importer::set(const std::string_view property, const std::any &value) {
	Object::set(property, value);
}

std::any Importer::get(const std::string_view property) const {
	switch (hash(property)) {
		case hash("class_name"):
			return "Importer";
		case hash("recognized_extensions"):
			return get_recognized_extensions();
		default:break;
	}
	return Object::get(property);
}

bool Importer::handles_file(const Path &path) const {
	return false;
}

const Vector<String> &Importer::get_recognized_extensions() const {
	static Vector<String> extensions {"asset"};
	return extensions;
}

SharedPtr<Asset> Importer::import(const Path &path) {
	if (!handles_file(path))
		return nullptr;
	
	simdjson::padded_string asset_contents = simdjson::padded_string::load(path.string());
	
	return nullptr;
}

