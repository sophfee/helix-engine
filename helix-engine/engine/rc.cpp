#include "rc.hpp"

#include "util.hpp"

void RefCounted::increment_reference() {
	++ref_count_;
}

void RefCounted::decrement_reference() {
	if (ref_count_ > 0) {
		--ref_count_;
		if (ref_count_ == 0)
			delete this;
	}
}

u32 RefCounted::get_reference_count() const {
	return ref_count_;
}

bool RefCounted::should_free() const {
	return ref_count_ == 0;
}

Any RefCounted::get(const StringView property) const {
	switch (hash(property)) {
	case hash("reference_count"):
		return ref_count_;
	default:
		return Object::get(property);
	}
}

void AtomicRefCounted::increment_reference() {
	++ref_count_;
}

void AtomicRefCounted::decrement_reference() {
	if (ref_count_ > 0) {
		--ref_count_;
		if (ref_count_ == 0)
			delete this;
	}
}

u32 AtomicRefCounted::get_reference_count() const {
	return ref_count_;
}

bool AtomicRefCounted::should_free() const {
	return ref_count_ == 0;
}

Any AtomicRefCounted::get(const StringView property) const {
	switch (hash(property)) {
	case hash("reference_count"):
		return ref_count_.load();
	default:
		return Object::get(property);
	}
}
