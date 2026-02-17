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
	extern std::wstring getEnvironmentVariable(std::wstring_view const name);
	extern std::wstring getCurrentDirectory();
	extern std::vector<u8> readFileToBytes(std::wstring_view const path);
	enum FileAttributes_e : u32 {
		FILEATTRIBUTE_READONLY = 1 << 0,
		FILEATTRIBUTE_HIDDEN = 1 << 1,
		FILEATTRIBUTE_SYSTEM = 1 << 2,
		FILEATTRIBUTE_DIRECTORY = 1 << 4,
		FILEATTRIBUTE_ARCHIVE = 1 << 5,
		FILEATTRIBUTE_DEVICE = 1 << 6,
		FILEATTRIBUTE_NORMAL = 1 << 7,
		FILEATTRIBUTE_TEMPORARY = 1 << 8,
		FILEATTRIBUTE_SPARSE_FILE = 1 << 9,
		FILEATTRIBUTE_REPARSE_POINT = 1 << 10,
		FILEATTRIBUTE_COMPRESSED = 1 << 11,
		FILEATTRIBUTE_OFFLINE = 1 << 12,
		FILEATTRIBUTE_NOT_CONTENT_INDEXED = 1 << 13,
		FILEATTRIBUTE_ENCRYPTED = 1 << 14,
		FILEATTRIBUTE_INTEGRITY_STREAM = 1 << 15,
		FILEATTRIBUTE_VIRTUAL = 1 << 16,
		FILEATTRIBUTE_NO_SCRUB_DATA = 1 << 17,
		FILEATTRIBUTE_EA = 1 << 18,
		FILEATTRIBUTE_PINNED = 1 << 19,
		FILEATTRIBUTE_UNPINNED = 1 << 20,
		FILEATTRIBUTE_RECALL_ON_OPEN = 1 << 21,
		FILEATTRIBUTE_RECALL_ON_DATA_ACCESS = 1 << 22,
	};
	struct FileMetadata_t {
		u64 size;
		std::chrono::time_point<std::chrono::file_clock> creation_time;
		std::chrono::time_point<std::chrono::file_clock> last_access_time;
		std::chrono::time_point<std::chrono::file_clock> last_write_time;
		FileAttributes_e attributes;
	};

	extern CResult<FileMetadata_t> fileMetadata(std::wstring_view const path);

	using FFileChangedCallback = void(*)(std::wstring_view const path);

	enum DirectoryWatchEventFilter_e : u32 {
		FILTER_CHANGE_FILE_NAME = 1 << 0,
		FILTER_CHANGE_DIR_NAME  = 1 << 1,
		FILTER_CHANGE_ATTRIBUTES = 1 << 2,
		FILTER_CHANGE_SIZE = 1 << 3,
		FILTER_CHANGE_LAST_WRITE = 1 << 4,
		FILTER_CHANGE_LAST_ACCESS = 1 << 5,
		FILTER_CHANGE_CREATION = 1 << 6,
		FILTER_CHANGE_SECURITY = 1 << 7,
	};

	//extern void watch(std::wstring_view const path, std::function<FFileChangedCallback> callback);
	//extern void unwatch(std::wstring_view const path);
	

	extern void initDirectoryWatcher();

	class CDirectoryWatcher {
	public:

		/*static std::unordered_map<u32, std::function<FFileChangedCallback>> watchers_;
		static void watch(std::wstring_view const path, FFileChangedCallback callback);
		static void unwatch(std::wstring_view const path);*/
		
		//static void processEvents();
	};
}

#endif // _WIN32