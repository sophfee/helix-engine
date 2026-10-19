#include "importer.hpp"

#include "os.hpp"
#include "util.hpp"
#include "simdjson/simdjson.h"

void Importer::set(const std::string_view property, const Any &value) {
	Object::set(property, value);
}

Any Importer::get(const std::string_view property) const {
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
	//	if (string_compare_equal(path.extension().string(), "asset"))
	//		return true;
	return false;
}

const Vector<String> &Importer::get_recognized_extensions() const {
	static Vector<String> extensions {"asset"};
	return extensions;
}

Future<Asset*> Importer::import(const Path &path) {
	if (!handles_file(path)) return {};
	return std::async([&]() -> Asset* {
		return import_async(path);
	});
}