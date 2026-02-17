#pragma once

//
// OS related things. I work on Windows 10 and Windows 11, which for some reason treat lots of things differently
//

#ifdef _WIN32
#include <string>

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
}

#endif // _WIN32