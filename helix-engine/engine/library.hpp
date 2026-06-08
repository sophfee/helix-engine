#pragma once
#include <string_view>

#include "filesystem.hpp"

class Library {
	HMODULE handle = nullptr;
public:

	using generic_fn = long long(*)();
	
	Library() = default;
	Library(std::string_view path);
	Library(Library const &) = delete;
	Library& operator=(Library const &) = delete;
	Library(Library &&) = default;
	Library& operator=(Library &&) = default;
	~Library();

	void load(std::string_view path);
	generic_fn findFunction(std::string_view name) const;

	template <typename /* fn ptr */ T>
	T findFunction(std::string_view const name) const {
		return reinterpret_cast<T>(findFunction(name));
	}
};
