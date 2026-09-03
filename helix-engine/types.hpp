#pragma once

#include <cstdint>
#include <cmath>
#include <array>
#include <assert.h>
#include <functional>
#include <future>
#include <string>
#include <string_view>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <stack>
#include <unordered_map>
#include <variant>
#include <vector>
#include <thread>
#include <span>
#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <utility>

#include "engine/flags.hpp"
#include "engine/rid.hpp"

#ifndef _LIKELY
#define _LIKELY [[likely]]
#endif

#ifndef _UNLIKELY
#define _UNLIKELY [[unlikely]]
#endif

namespace detail {
	template <typename TEnum>
	[[nodiscard]] constexpr bool has_flag(const TEnum value, const TEnum flag) {
		BitFlag<TEnum> a(value);
		return a.has(flag);
	}
}

using u8 = _STD uint8_t;
using u16 = _STD uint16_t;
using u32 = _STD uint32_t;
using u64 = _STD uint64_t;

using i8 = _STD int8_t;
using i16 = _STD int16_t;
using i32 = _STD int32_t;
using i64 = _STD int64_t;

using f32 = _STD float_t;
using f64 = _STD double_t;

template <typename T> using Vector = _STD vector<T>;
template <typename T> using List = _STD list<T>;
template <typename T> using Queue = _STD queue<T>;
template <typename T> using Deque = _STD deque<T>;
template <typename T> using Stack = _STD stack<T>;
template <typename T, _STD size_t N> using Array = _STD array<T, N>;
template <typename K, typename V> using Map = _STD map<K,V>;
template <typename K, typename V> using UnorderedMap = _STD unordered_map<K,V>;

using String = _STD string;
using WString = _STD wstring;

template <typename T> using SharedPtr = _STD shared_ptr<T>;
template <typename T> using Weak = _STD weak_ptr<T>;
template <typename T> using UniquePtr = _STD unique_ptr<T>;
template <typename T> using Box = _STD unique_ptr<T>; // rust semantics are kinda cool

template <typename T> using Span = _STD span<T>;

template <typename T> using Optional = _STD optional<T>;
template <typename ...T> using Variant = _STD variant<T...>;

template <typename Fn>
struct first_arg;

template <typename R, typename C, typename... Args>
struct first_arg<R(C, Args...)> { using type = C; };

// For function pointers
template <typename R, typename C, typename... Args>
struct first_arg<R(*)(C, Args...)> { using type = C; };

// For lambdas / functors via operator()
template <typename F>
struct first_arg : first_arg<decltype(&F::operator())> {};

template <typename Cls, typename R, typename C, typename... Args>
struct first_arg<R(Cls::*)(C, Args...) const> { using type = C; };

using Thread = _STD jthread; // jthread is a joinable thread that automatically joins on destruction, so we don't have to worry about it. It also supports cooperative cancellation, which is nice.

template <typename D>
using Func = _STD function<D>;

template <typename T>
using Task = _STD packaged_task<T>;
using Mutex = ::std::mutex;
using RecursiveMutex = ::std::recursive_mutex;

using ConditionVariable = ::std::condition_variable;
using AtomicFlag = ::std::atomic_flag;

template <ptrdiff_t LEAST_MAX_VALUE = ::std::_Semaphore_max>
using Semaphore = ::std::counting_semaphore<LEAST_MAX_VALUE>;
using BinarySemaphore = ::std::binary_semaphore;

template <typename T>
using Atomic = std::atomic<T>;

template <typename T>
struct Slot {
	T value;
	u32 generation = 1;
	bool occupied = false;
};

template <typename T>
class SlotPool {
public:
	struct Handle {
		SlotPool* pool = nullptr;
		u32 slot = 0;
		u32 generation = 0;
		
		operator T*() {
			if (!pool) return nullptr;
			return pool->get(slot, generation);
		}
		
		T* operator->() {
			if (!pool) return nullptr;
			return pool->get(slot, generation);
		}
		
		operator RID() const {
			return {slot, generation};
		}
	};

	class iterator {
	public:
		using iterator_category = ::std::forward_iterator_tag;
		using value_type = std::pair<RID, T>;
		using difference_type = ::std::ptrdiff_t;
		using pointer = std::pair<RID, T*>;
		using reference = std::pair<RID, std::reference_wrapper<T>>;

		iterator() = default;
		iterator(typename Vector<Slot<T>>::iterator current, typename Vector<Slot<T>>::iterator end)
			: current_(current), begin_(current), end_(end) {
			advance_to_occupied_();
		}

		pointer operator*() const { return {RID{static_cast<u32>(std::distance(begin_, current_)), current_->generation}, &current_->value}; }
		pointer operator->() const { return {RID{static_cast<u32>(std::distance(begin_, current_)), current_->generation}, &current_->value}; }

		iterator& operator++() {
			++current_;
			advance_to_occupied_();
			return *this;
		}

		iterator operator++(int) {
			iterator temp = *this;
			++*this;
			return temp;
		}

		bool operator==(const iterator& other) const { return current_ == other.current_; }
		bool operator!=(const iterator& other) const { return !(*this == other); }

	private:
		void advance_to_occupied_() {
			while (current_ != end_ && !current_->occupied) {
				++current_;
			}
		}

		typename Vector<Slot<T>>::iterator current_{};
		typename Vector<Slot<T>>::iterator begin_{};
		typename Vector<Slot<T>>::iterator end_{};
	};

	class const_iterator {
	public:
		using iterator_category = ::std::forward_iterator_tag;
		using value_type = const std::pair<RID, const T>;
		using difference_type = ::std::ptrdiff_t;
		using pointer = const std::pair<RID, const T*>;
		using reference = const std::pair<RID, std::reference_wrapper<const T>>;

		const_iterator() = default;
		const_iterator(typename Vector<Slot<T>>::const_iterator current, typename Vector<Slot<T>>::const_iterator end)
			: current_(current), begin_(current), end_(end) {
			advance_to_occupied_();
		}

		reference operator*() const { return {RID{static_cast<u32>(std::distance(begin_, current_)), current_->generation}, current_->value}; }
		pointer operator->() const { return {RID{static_cast<u32>(std::distance(begin_, current_)), current_->generation}, &current_->value}; }

		const_iterator& operator++() {
			++current_;
			advance_to_occupied_();
			return *this;
		}

		const_iterator operator++(int) {
			const_iterator temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const const_iterator& other) const { return current_ == other.current_; }
		bool operator!=(const const_iterator& other) const { return !(*this == other); }

	private:
		void advance_to_occupied_() {
			while (current_ != end_ && !current_->occupied) {
				++current_;
			}
		}

		typename Vector<Slot<T>>::const_iterator current_{};
		typename Vector<Slot<T>>::const_iterator begin_{};
		typename Vector<Slot<T>>::const_iterator end_{};
	};

	template <typename... TArgs>
	[[nodiscard]] Handle emplace(TArgs&&... args) {
		std::scoped_lock lock(mutex_);
		const u32 slot = acquire_slot_();
		Slot<T> &entry = slots_[slot];
		if constexpr (std::is_copy_assignable_v<T>)
			entry.value = T(std::forward<TArgs>(args)...);
		else
			new(&entry.value) T(std::forward<TArgs>(args)...);
		entry.occupied = true;
		return Handle{
			.pool = this,
			.slot = slot,
			.generation = entry.generation
		};
	}

	[[nodiscard]] T* get(const u32 slot, const u32 generation) {
		assert(slot < slots_.size());
		Slot<T>& entry = slots_[slot];
		assert(entry.occupied && entry.generation == generation);
		return &entry.value;
	}
	
	[[nodiscard]] T* get(const RID rid) {
		return get(rid.upper, rid.lower);
	}

	[[nodiscard]] const T* get(const u32 slot, const u32 generation) const {
		if (slot >= slots_.size()) return nullptr;
		const Slot<T>& entry = slots_[slot];
		if (!entry.occupied || entry.generation != generation) return nullptr;
		return &entry.value;
	}
	
	[[nodiscard]] T const * const get(const RID rid) const {
		return get(rid.upper, rid.lower);
	}
	
	[[nodiscard]] bool contains(const u32 slot, const u32 generation) const {
		return contains(RID{slot, generation});
	}
	
	[[nodiscard]] bool contains(const RID rid) const {
		const Slot<T> &slot = slots_[rid.upper];
		return slot.occupied && slot.generation == rid.lower;
	}
	
	[[nodiscard]] bool tryGet(const u32 slot, const u32 generation, T*& out) {
		std::scoped_lock lock(mutex_);
		if (slot >= slots_.size()) return false;
		Slot<T>& entry = slots_[slot];
		if (!entry.occupied || entry.generation != generation) return false;
		out = &entry.value;
		return true;
	}
	
	[[nodiscard]] bool tryGet(const RID rid, T*& out) {
		return tryGet(rid.upper, rid.lower, out);
	}

	[[nodiscard]] bool erase(const RID rid) {
		return erase(rid.upper, rid.lower);
	}

	[[nodiscard]] bool erase(const u32 slot, const u32 generation) {
		std::scoped_lock lock(mutex_);
		if (slot >= slots_.size()) return false;
		Slot<T>& entry = slots_[slot];
		if (!entry.occupied || entry.generation != generation) return false;
		entry.occupied = false;
		std::memset(&entry.value, 0, sizeof(T));
		++entry.generation;
		free_slots_.push_back(slot);
		return true;
	}

	[[nodiscard]] bool is_alive(const u32 slot, const u32 generation) const {
		return get(slot, generation) != nullptr;
	}
	
	[[nodiscard]] bool is_alive(const RID rid) const {
		return is_alive(rid.upper, rid.lower);
	}
	
	[[nodiscard]] bool empty() const {
		return slots_.empty();
	}

	[[nodiscard]] size_t size() const {
		return slots_.size() - free_slots_.size();
	}
	
	void clear() {
		std::scoped_lock lock(mutex_);
		slots_.clear();
		free_slots_.clear();
	}

	iterator begin() { return iterator(slots_.begin(), slots_.end()); }
	iterator end() { return iterator(slots_.end(), slots_.end()); }
	const_iterator begin() const { return const_iterator(slots_.begin(), slots_.end()); }
	const_iterator end() const { return const_iterator(slots_.end(), slots_.end()); }
	const_iterator cbegin() const { return begin(); }
	const_iterator cend() const { return end(); }

private:
	[[nodiscard]] u32 acquire_slot_() {
		if (!free_slots_.empty()) {
			const u32 slot = free_slots_.back();
			free_slots_.pop_back();
			return slot;
		}
		slots_.emplace_back(std::move(Slot<T>()));
		return static_cast<u32>(slots_.size() - 1);
	}

public:
	Vector<Slot<T>> slots_;
	Vector<u32> free_slots_;
	Mutex mutex_;
};

enum Error {
	OK = 0,
	FAILED,
	ERR_UNAVAILABLE,
	ERR_UNCONFIGURED,
	ERR_UNAUTHORIZED,
	ERR_OUT_OF_RANGE,
	ERR_OUT_OF_MEMORY,
	ERR_FILE_NOT_FOUND,
	ERR_FILE_BAD_DRIVE,
	ERR_FILE_BAD_PATH,
	ERR_FILE_NO_PERMISSION, // (10)
	ERR_FILE_ALREADY_IN_USE,
	ERR_FILE_CANT_OPEN,
	ERR_FILE_CANT_WRITE,
	ERR_FILE_CANT_READ,
	ERR_FILE_UNRECOGNIZED, // (15)
	ERR_FILE_CORRUPT,
	ERR_FILE_MISSING_DEPENDENCIES,
	ERR_FILE_EOF,
	ERR_CANT_OPEN, ///< Can't open a resource/socket/file
	ERR_CANT_CREATE, // (20)
	ERR_QUERY_FAILED,
	ERR_ALREADY_IN_USE,
	ERR_LOCKED, ///< resource is locked
	ERR_TIMEOUT,
	ERR_CANT_CONNECT, // (25)
	ERR_CANT_RESOLVE,
	ERR_CONNECTION_ERROR,
	ERR_CANT_ACQUIRE_RESOURCE,
	ERR_CANT_FORK,
	ERR_INVALID_DATA, ///< Data passed is invalid (30)
	ERR_INVALID_PARAMETER, ///< Parameter passed is invalid
	ERR_ALREADY_EXISTS, ///< When adding, item already exists
	ERR_DOES_NOT_EXIST, ///< When retrieving/erasing, if item does not exist
	ERR_DATABASE_CANT_READ, ///< database is full
	ERR_DATABASE_CANT_WRITE, ///< database is full (35)
	ERR_COMPILATION_FAILED,
	ERR_METHOD_NOT_FOUND,
	ERR_LINK_FAILED,
	ERR_SCRIPT_FAILED,
	ERR_CYCLIC_LINK, // (40)
	ERR_INVALID_DECLARATION,
	ERR_DUPLICATE_SYMBOL,
	ERR_PARSE_ERROR,
	ERR_BUSY,
	ERR_SKIP, // (45)
	ERR_HELP, ///< user requested help!!
	ERR_BUG, ///< a bug in the software certainly happened, due to a double check failing or unexpected behavior.
	ERR_PRINTER_ON_FIRE, /// the parallel port printer is engulfed in flames
	ERR_MAX, // Not being returned, value represents the number of errors
};

constexpr char const *to_string(Error const e) noexcept {
	switch (e) {
		case OK: return "OK";
		case FAILED: return "FAILED";
		case ERR_UNAVAILABLE: return "UNAVAILABLE";
		case ERR_UNCONFIGURED: return "UNCONFIGURED";
		case ERR_UNAUTHORIZED: return "UNAUTHORIZED";
		case ERR_OUT_OF_RANGE: return "OUT OF RANGE";
		case ERR_OUT_OF_MEMORY: return "OUT OF MEMORY";
		case ERR_FILE_NOT_FOUND: return "FILE NOT FOUND";
		case ERR_FILE_BAD_DRIVE: return "FILE BAD DRIVE";
		case ERR_FILE_BAD_PATH: return "FILE BAD PATH";
		case ERR_FILE_NO_PERMISSION: return "FILE NO PERMISSION";
		case ERR_FILE_ALREADY_IN_USE: return "FILE ALREADY IN USE";
		case ERR_FILE_CANT_OPEN: return "FILE CANT OPEN";
		case ERR_FILE_CANT_WRITE: return "FILE CANT WRITE";
		case ERR_FILE_CANT_READ: return "FILE CANT READ";
		case ERR_FILE_UNRECOGNIZED: return "FILE UNRECOGNIZED";
		case ERR_FILE_CORRUPT: return "FILE CORRUPT";
		case ERR_FILE_MISSING_DEPENDENCIES: return "FILE MISSING DEPENDENCIES";
		case ERR_FILE_EOF: return "FILE EOF";
		case ERR_CANT_OPEN: return "CANT OPEN";
		case ERR_CANT_CREATE: return "CANT CREATE";
		case ERR_QUERY_FAILED: return "QUERY FAILED";
		case ERR_ALREADY_IN_USE: return "ALREADY IN USE";
		case ERR_LOCKED: return "LOCKED";
		case ERR_TIMEOUT: return "TIMEOUT";
		case ERR_CANT_CONNECT: return "CAN'T CONNECT";
		case ERR_CANT_RESOLVE: return "CAN'T RESOLVE";
		case ERR_CONNECTION_ERROR: return "CONNECTION ERROR";
		case ERR_CANT_ACQUIRE_RESOURCE: return "CAN'T ACQUIRE RESOURCE";
		case ERR_CANT_FORK: return "CAN'T FORK";
		case ERR_INVALID_DATA: return "INVALID DATA";
		case ERR_INVALID_PARAMETER: return "INVALID PARAMETER";
		case ERR_ALREADY_EXISTS: return "ALREADY EXISTS";
		case ERR_DOES_NOT_EXIST: return "DOES NOT EXIST";
		case ERR_DATABASE_CANT_READ: return "DATABASE CANT READ";
		case ERR_DATABASE_CANT_WRITE: return "DATABASE CANT WRITE";
		case ERR_COMPILATION_FAILED: return "COMPILATION FAILED";
		case ERR_METHOD_NOT_FOUND: return "METHOD NOT FOUND";
		case ERR_LINK_FAILED: return "LINK FAILED";
		case ERR_SCRIPT_FAILED: return "SCRIPT FAILED";
		case ERR_CYCLIC_LINK: return "CYCLIC LINK";
		case ERR_INVALID_DECLARATION: return "INVALID DECLARATION";
		case ERR_DUPLICATE_SYMBOL: return "DUPLICATE SYMBOL";
		case ERR_PARSE_ERROR: return "PARSE ERROR";
		case ERR_BUSY: return "BUSY";
		case ERR_SKIP: return "SKIP";
		case ERR_HELP: return "HELP";
		case ERR_BUG: return "BUG";
		case ERR_PRINTER_ON_FIRE: return "PRINTER ON FIRE";
		case ERR_MAX: return "MAX";
	}
	return "unknown";
}


/*
constexpr _STD uint32_t hash(_STD wstring_view const szStr) {
	_STD string_view narrow_str(reinterpret_cast<char const *>(szStr.data()), szStr.size() * 2);
	return detail::crc32(narrow_str.data(), narrow_str.length()) ^ 0xFFFFFFFF;
}
*/

template <typename T = void>
class Result {
	Error error_;
	int failed_at_ = 0;
	bool has_value_;

	static constexpr bool is_reference_wrapped = false;
	
	_STD optional<T> value_;

public:
	Result(Error const e = FAILED, int const line = 0) noexcept : error_(e), failed_at_(line), has_value_(false) {}
	Result(T const &v) noexcept : error_(OK), has_value_(true), value_(v) {}
	Result(T &&v) noexcept : error_(OK), has_value_(true), value_(_STD move(v))  {}

	_NODISCARD bool has_value() const { return has_value_; }
	_NODISCARD bool is_null() const { return !has_value_; }

	_NODISCARD T value() noexcept { if constexpr(is_reference_wrapped) return value_.value().get(); else return value_.value(); }
	_NODISCARD Error error() const noexcept { return error_; }

	// ReSharper disable once CppNonExplicitConversionOperator
	_NODISCARD operator T() noexcept { return value(); }
};

template <>
class Result<void> {
	Error error_;
	int failed_at_ = 0;
	bool has_value_;
	public:
	Result() : error_(OK), has_value_(true) {}
	Result(Error const e, int const line = 0) noexcept : error_(e), failed_at_(line), has_value_(false) {}
	_NODISCARD bool has_value() const { return has_value_; }
	_NODISCARD Error error() const noexcept { return error_; }
	explicit _NODISCARD operator bool() const noexcept { return has_value_; }
};

template <typename T>
class Result<T &> {
	Error error_;
	int failed_at_ = 0;
	bool has_value_;

	static constexpr bool is_reference_wrapped = true;
	
	_STD optional<_STD reference_wrapper<T>> value_;

public:
	Result(Error const e = FAILED, int const line = 0) noexcept : error_(e), failed_at_(line), has_value_(false) {}
	Result(T &v) noexcept : error_(OK), has_value_(true), value_(std::ref(v)) {}
	Result(T &&v) noexcept : error_(OK), has_value_(true), value_(_STD move(v))  {}

	_NODISCARD bool has_value() const { return has_value_; }
	_NODISCARD bool is_null() const { return !has_value_; }

	_NODISCARD T &value() noexcept { if constexpr(is_reference_wrapped) return value_.value().get(); else return value_.value(); }
	_NODISCARD Error error() const noexcept { return error_; }

	// ReSharper disable once CppNonExplicitConversionOperator
	_NODISCARD operator T &() noexcept { return value(); }
};

class NoCopy {
public:
	NoCopy() = default;
	NoCopy(NoCopy &) = delete;
	NoCopy &operator=(NoCopy &) = delete;
	NoCopy(NoCopy &&) = delete;
	NoCopy &operator=(NoCopy &&) = delete;
	virtual ~NoCopy() = default;
};

template <typename T>
concept ReferenceCounted = requires(T v)
{
	{v.users()} -> _STD convertible_to<u32>;
	{v.incrementReference()};
	{v.decrementReference()};
	{v.valid()} -> _STD convertible_to<bool>;
	//{v.reset(())}
};

template <typename ...TArgs>
[[noreturn]] inline void reportError(Error const error, String const &message, TArgs... format_args) {
	fprintf_s(stderr, message.c_str(), format_args...);
	std::exit(error);
}

template <typename Tl, typename Tr, typename Re> requires (std::is_enum_v<Tl> && std::is_convertible_v<std::underlying_type_t<Tl>, Re> 
	&& std::is_enum_v<Tr> && std::is_convertible_v<std::underlying_type_t<Tr>, Re>)
[[nodiscard]] constexpr Re operator|(Tl const l, Tr const r) noexcept {
	return static_cast<Re>(static_cast<std::underlying_type_t<Tl>>(l) | static_cast<std::underlying_type_t<Tr>>(r));
}

template <typename Tl, typename Tr> requires (std::is_enum_v<Tl> && std::is_enum_v<Tr>)
[[nodiscard]] constexpr bool operator&(Tl const l, Tr const r) noexcept {
	using left = std::underlying_type_t<Tl>;
	using right = std::underlying_type_t<Tr>;
	return (static_cast<left>(l) & static_cast<right>(r)) == static_cast<left>(r);
}

#define HELIX_ERR_PRINT(MESSAGE, ...) fprintf_s(stderr, MESSAGE "\n", ##__VA_ARGS__)

#define HELIX_STRINGIFY(BLAH) #BLAH

#ifdef _DEBUG
#define HELIX_ASSUME(CONDITION, MESSAGE, ...) if (!(CONDITION)) _UNLIKELY { HELIX_ERR_PRINT("[HELIX ENGINE] Assertion failed: " ##MESSAGE## " at Line " HELIX_STRINGIFY(__LINE__) " in " HELIX_STRINGIFY(_FILE__) "\n", ##__VA_ARGS__); }
#else
#define HELIX_ASSUME(CONDITION, MESSAGE) __assume(CONDITION);
#endif