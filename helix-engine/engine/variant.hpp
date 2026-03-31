#pragma once
#include <cwchar>
#include <memory>
#include <type_traits>

#include "types.hpp"

class SceneTree;
class Component;
class Entity;
namespace detail {

	template <typename T>
	struct VariantType {
		T value;

		[[nodiscard]] T getValue() const {
			return value;
		}

		template <typename ...Args>
		static VariantType ctor(Args &&...args) {
			return VariantType{T(std::forward<Args>(args)...)};
		}
	};

	template <typename T>
	struct VariantType<T *> {
		using type = std::decay_t<T>;
		using wrapper = std::shared_ptr<type>;
		wrapper container;

		operator T() const {
			return *container;
		}

		[[nodiscard]] T &operator*() {
			return *container;
		}

		[[nodiscard]] T &getValue() const {
			return *container;
		}

		template <typename ...Args>
		static VariantType<T> ctor(Args &&...args) {
			return VariantType<T>{std::shared_ptr<type>(new T(std::forward<Args>(args)...))};
		}
	};

	template <typename T>
	struct VariantType<T &> {
		using type = std::decay_t<T>;
		using wrapper = std::reference_wrapper<type>;
		wrapper container;

		operator T() const {
			return *container;
		}

		[[nodiscard]] T &operator*() {
			return *container;
		}

		[[nodiscard]] T &getValue() const {
			return *container;
		}
		
		static VariantType<T> ctor(T &r) {
			return VariantType<T>{std::ref(r)};
		}

		void move(T &&r) {
			container = std::move(r);
		}
	};

	template <>
	struct VariantType<void *> {
		void *value;
		
		[[nodiscard]] void *getValue() const {
			return value;
		}

		static VariantType ctor(void *r) {
			return VariantType{r};
		}

		void move(void *&&r) {
			value = r;
		}
	};

	union VariantContainerData {
		VariantType<i64> i;
		VariantType<u64> u;
		VariantType<f64> f;
		VariantType<String> s;
		VariantType<Entity *> e;
		VariantType<Component *> c;
		VariantType<SceneTree *> t;
		VariantType<void *> p;
	};

	struct VariantContainer {
		VariantContainerData data_;
		u8 type_index_;
	};

	template <typename T>
	struct VariantTypeIndex {
		static constexpr auto value = 99;
	};

	template <typename T, typename ...O>
	struct VariantTypeList {
		using next = VariantTypeList<O...>;
		using type = T;
	};

	template <typename L, typename C, typename ...T, size_t N>
	constexpr auto type_list_iter(u8 index, u8 current_index) {
		if constexpr (sizeof...(T) == 0) {
			return 99;
		} else {
			
		}
	}
	
}

class Variant final {
public:
	

private:
	detail::VariantContainer container_;
};