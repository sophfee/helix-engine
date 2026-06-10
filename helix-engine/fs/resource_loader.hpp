#pragma once
#include "types.hpp"

// Structure is somewhat based off of godot

class ResourceFormatImporter;

class ResourceLoader final {
	static Vec<SharedPtr<ResourceFormatImporter>> importers;
public:
	
};