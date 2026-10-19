#include "binary_asset.hpp"

#include "util.hpp"

void BinaryAsset::set_binary_data(const ByteVector &data) {
	binary_data_ = data;
}

ByteVector & BinaryAsset::get_binary_data() {
	return binary_data_;
}

const ByteVector & BinaryAsset::get_binary_data() const {
	return binary_data_;
}

StringView BinaryAsset::get_class_name() const {
	return "BinaryAsset";
}

void BinaryAsset::set(const StringView property, const Any &value) {
	switch (hash(property)) {
	case hash("data"):
		if (value.type() == typeid(ByteVector)) {
			binary_data_ = _STD any_cast<ByteVector>(value);
			return;
		}
		if (value.type() == typeid(u8*)) {
			binary_data_ = ByteVector(_STD any_cast<u8*>(value), _STD any_cast<u8*>(value) + binary_data_.size());
			return;
		}
		break;
	default: break;
	}
	Asset::set(property, value);
}

Any BinaryAsset::get(const StringView property) const {
	switch (hash(property)) {
	case hash("data"):
		return binary_data_;
	default:
		break;
	}
	return Asset::get(property);
}