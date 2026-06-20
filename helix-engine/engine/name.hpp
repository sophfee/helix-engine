#pragma once
#include <unordered_set>

#include "types.hpp"
#include "util.hpp"
#include "gpu/gltf/KHR_lights_punctual.hpp"

// A name is a string that is pooled without needing to match references, as they all internally utilize the same pool.

namespace detail {
	template <typename TChar, size_t NChars>
	class NamePool {

		using string_view = std::basic_string_view<TChar>;
		using string = std::basic_string<TChar>;
		using string_ref = std::shared_ptr<string>;
		using string_weak_ref = std::weak_ptr<string>;
		
	public:
		static string_ref createOrRetrieve(TChar (&name)[NChars]) {
			static NamePool myNamePool;
			if (myNamePool.pool.contains(hash(name))) {
				if (auto existing = myNamePool.pool[hash(name)].lock()) {
					return *existing;
				}
				return nullptr;
			}
			myNamePool.pool[hash(name)] = std::make_shared<string>(name);
			return myNamePool.pool[hash(name)];
		}
		
	private:
		
		UnorderedMap<u32, string_weak_ref> pool; //< Also should accept std::string or std::wstring as valid inputs
	};
}

template <typename TChar>
struct Name {
private:

	using string_view = std::basic_string_view<TChar>;
	using string = std::basic_string<TChar>;
	using string_ref = std::shared_ptr<string>;
	
public:
	template <std::size_t NChars>
	Name(TChar (&name)[NChars]) : name(detail::NamePool<TChar, NChars>::createOrRetrieve(name)) {}

	[[nodiscard]] string_view get() const { return *name; }
	
	operator string_view() const { return *name; }
	explicit operator string() const { return *name; }

	[[nodiscard]] bool operator==(Name const & other) const { return name.get() == other.name.get(); /* compare pointer values */ }
	[[nodiscard]] bool operator!=(Name const & other) const { return !operator==(other); }

private:
	string_ref name;
};

using CName = Name<char>;
using WName = Name<char16_t>;