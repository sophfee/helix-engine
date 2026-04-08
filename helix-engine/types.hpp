#pragma once

#include <cstdint>
#include <cmath>
#include <array>
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

#include "gpu/opengl_enums.hpp"

using u8 = _STD uint8_t;
using u16 = _STD uint16_t;
using u32 = _STD uint32_t;
using u64 = _STD uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = _STD float_t;
using f64 = _STD double_t;

template <typename T> using Vec = _STD vector<T>;
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

template <typename T> using Optional = _STD optional<T>;
template <typename ...T> using Variant = _STD variant<T...>;

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

template <typename T>
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

	_NODISCARD T value() noexcept { if constexpr(is_reference_wrapped) return value_.value().get(); else return value_.value(); }
	_NODISCARD Error error() const noexcept { return error_; }

	// ReSharper disable once CppNonExplicitConversionOperator
	_NODISCARD operator T() noexcept { return value(); }
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

#define HELIX_ERR_PRINT(MESSAGE, ...) fprintf_s(stderr, MESSAGE "\n", ##__VA_ARGS__)

#define HELIX_STRINGIFY(BLAH) #BLAH

#ifdef _DEBUG
#define HELIX_ASSUME(CONDITION, MESSAGE, ...) if (!(CONDITION)) _UNLIKELY { HELIX_ERR_PRINT("[HELIX ENGINE] Assertion failed: " ##MESSAGE## " at Line " HELIX_STRINGIFY(__LINE__) " in " HELIX_STRINGIFY(_FILE__) "\n", ##__VA_ARGS__); }
#else
#define HELIX_ASSUME(CONDITION, MESSAGE) __assume(CONDITION);
#endif