#include "object.hpp"

#include "util.hpp"

StringView Object::get_class_name() const {
	return "Object";
}

void Object::set(const StringView property, const Any &value) {
}

Any Object::get(const StringView property) const {
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

