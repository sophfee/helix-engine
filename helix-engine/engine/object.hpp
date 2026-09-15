#pragma once
#include <any>
#include <string_view>

class Object {
public:
	using SelfType = Object;
	virtual ~Object() = 0;
	
	virtual void set(const std::string_view property, const std::any& value);
	virtual std::any get(const std::string_view property) const;
};
