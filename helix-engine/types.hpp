#pragma once

#include <cstdint>
#include <cmath>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = std::float_t;
using f64 = std::double_t;

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
constexpr const char *to_string(Error e) {
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
