#pragma once
#include <any>
#include <string_view>

#include "types.hpp"

class Object {
public:
	using SelfType = Object;
	virtual ~Object() = 0;
	
	virtual StringView get_class_name() const;
	
	virtual void set(const StringView property, const Any& value);
	virtual Any get(const StringView property) const;
};
