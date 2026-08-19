#pragma once
#include "types.hpp"

// Structure is somewhat based off of godot

class ResourceFormatImporter;

class ResourceLoader final {
	static Vector<SharedPtr<ResourceFormatImporter>> importers;
public:
	
};