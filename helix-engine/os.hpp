#pragma once

//
// OS related things. I work on Windows 10 and Windows 11, which for some reason treat lots of things differently
//

#ifdef _WIN32
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "types.hpp"
#include <functional>

#ifdef WINVER
#if WINVER == 0x0A00
#define WINDOWS_10
#define OS_TYPE "WINDOWS"
#define OS_VER 10
#elif  WINVER == 0x0A01
#define WINDOWS_11
#define OS_TYPE "WINDOWS"
#define OS_VER 11
#endif
#else
#define OS_TYPE "UNKNOWN"
#define OS_VER 0
#endif

namespace os {
	extern _STD wstring get_environment_variable(_STD wstring_view const name);
	extern _STD wstring get_current_directory();
	extern _STD vector<u8> read_file_to_bytes(_STD wstring_view const path);
	enum FileAttributes : u32 {
		eReadOnly = 1 << 0,
		eHidden = 1 << 1,
		eSystem = 1 << 2,
		eDirectory = 1 << 4,
		eArchive = 1 << 5,
		eDevice = 1 << 6,
		eNormal = 1 << 7,
		eTemporary = 1 << 8,
		eSparseFile = 1 << 9,
		eReparsePoint = 1 << 10,
		eCompressed = 1 << 11,
		eOffline = 1 << 12,
		eNotContentIndexed = 1 << 13,
		eEncrypted = 1 << 14,
		eIntegrityStream = 1 << 15,
		eVirtual = 1 << 16,
		eNoScrubData = 1 << 17,
		eEa = 1 << 18,
		ePinned = 1 << 19,
		eUnpinned = 1 << 20,
		eRecallOnOpen = 1 << 21,
		eRecallOnDataAccess = 1 << 22,
	};
	struct FileMetadata {
		u64 size;
		_STD chrono::time_point<_STD chrono::file_clock> creation_time;
		_STD chrono::time_point<_STD chrono::file_clock> last_access_time;
		_STD chrono::time_point<_STD chrono::file_clock> last_write_time;
		FileAttributes attributes;
	};
	extern Result<FileMetadata> file_metadata(_STD wstring_view const path);
	using FFileChangedCallback = void(*)(_STD wstring_view const path);
	enum DirectoryWatchEventFilter : u32 {
		eFileName = 1 << 0,
		eDirectoryName  = 1 << 1,
		eAttributes = 1 << 2,
		eSize = 1 << 3,
		eLastWrite = 1 << 4,
		eLastAccess = 1 << 5,
		eCreation = 1 << 6,
		eSecurity = 1 << 7,
	};
	//extern void watch(_STD wstring_view const path, _STD function<FFileChangedCallback> callback);
	//extern void unwatch(_STD wstring_view const path);
	extern void init_directory_watcher();
	extern void print_last_error();
}

#endif // _WIN32