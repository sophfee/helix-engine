#pragma once
#include "types.hpp"

class Resource;

class ResourceFormatImporter {
public:
	virtual ~ResourceFormatImporter() = default;
	virtual SharedPtr<Resource> load(String const &path, String const &original_path = "", Error *r_error = nullptr) const = 0;
	virtual bool handlesType(String const &extension) const = 0;
	virtual String resourceType(String const &path) const = 0;
};