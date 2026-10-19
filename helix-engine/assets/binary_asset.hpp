#pragma once

#include "asset.hpp"

class BinaryAsset : public Asset {
public:
	using SelfType = BinaryAsset;
	
	void set_binary_data(const ByteVector& data);
	[[nodiscard]] ByteVector& get_binary_data();
	[[nodiscard]] const ByteVector& get_binary_data() const;

	[[nodiscard]] StringView get_class_name() const override;
	void set(const StringView property, const Any &value) override;
	[[nodiscard]] Any get(const StringView property) const override;
	
private:
	ByteVector binary_data_;
};