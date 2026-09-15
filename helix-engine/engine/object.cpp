#include "object.hpp"

#include "util.hpp"

void Object::set(const std::string_view property, const std::any &value) {
}

std::any Object::get(const std::string_view property) const {
	switch (hash(property)) {
	case hash("class_name"):
		return "Object";
	case hash("raw_pointer"):
		return this;
	default:
		break;
	}
	return {};
}

