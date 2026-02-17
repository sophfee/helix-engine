#include "os.hpp"

#include <Windows.h>
#include <future>

std::wstring os::getEnvironmentVariable(std::wstring_view const name) {
	std::wstring const nt_name(name.data(), name.length());
	DWORD const length = GetEnvironmentVariable(nt_name.c_str(), nullptr, 0);
	if (length == 0) { return TEXT("failed"); }
	// returns length w/o null terminator
	auto const buffer = new WCHAR[length + 1];
	std::memset(buffer, 0, length + 1);
	GetEnvironmentVariable(nt_name.c_str(), buffer, length);
	std::wstring result(buffer, length);
	delete[] buffer;
	return result;
}

std::wstring os::getCurrentDirectory() {
	DWORD const length = GetCurrentDirectory(0, nullptr);
	if (length == 0) { return TEXT("failed"); }
	auto const buffer = new WCHAR[length];
	std::memset(buffer, 0, length);
	GetCurrentDirectory(length, buffer);
	std::wstring result(buffer, length);
	delete[] buffer;
	return result;
}

std::vector<u8> os::readFileToBytes(std::wstring_view const path) {
	std::wstring const nt_path(path.data(), path.length());
	HANDLE file = CreateFile(
		nt_path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (file == INVALID_HANDLE_VALUE) {
		return {};
	}
	DWORD file_size = GetFileSize(file, nullptr);
	std::vector<u8> buffer(file_size);
	DWORD bytes_read;
	BOOL read_result = ReadFile(file, buffer.data(), file_size, &bytes_read, nullptr);
	CloseHandle(file);
	if (!read_result || bytes_read != file_size) {
		return {};
	}
	return buffer;
}

CResult<os::FileMetadata_t> os::fileMetadata(std::wstring_view const path)
{
	os::FileMetadata_t metadata{};
	std::wstring const nt_path(path.data(), path.length());
	WIN32_FILE_ATTRIBUTE_DATA file_data{};
	if (GetFileAttributesEx(nt_path.c_str(), GetFileExInfoStandard, &file_data) == 0) {
		// Handle error
		return metadata;
	}
}

void os::initDirectoryWatcher() {

	struct DirectoryWatch_t {
		OVERLAPPED overlapped;
		char buffer[1024 << 4];
	};

	DirectoryWatch_t directory_watch{};

	std::async([&directory_watch] {
		directory_watch.overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);

		std::wstring wstr = getCurrentDirectory();
		HANDLE hDirectory = CreateFile(
			wstr.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL
		);
		if (hDirectory == INVALID_HANDLE_VALUE) return;

		BOOL bResult = ReadDirectoryChangesW(
			hDirectory,
			directory_watch.buffer,
			sizeof(directory_watch.buffer),
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY,
			NULL,
			&directory_watch.overlapped,
			NULL
		);

		if (!bResult) return;

		DWORD dwResult = WaitForSingleObject(directory_watch.overlapped.hEvent, 0);
		if (dwResult != WAIT_OBJECT_0) return;

		while (true) {
			FILE_NOTIFY_INFORMATION *pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(directory_watch.buffer);
			std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
			// Handle the file change event (e.g., print the file name)
			wprintf(L"File changed: %s\n", fileName.c_str());
			if (pNotify->NextEntryOffset == 0) break;
			pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(reinterpret_cast<BYTE *>(pNotify) + pNotify->NextEntryOffset);
			
			BOOL bResult = ReadDirectoryChangesW(
				hDirectory,
				directory_watch.buffer,
				sizeof(directory_watch.buffer),
				TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY,
				NULL,
				&directory_watch.overlapped,
				NULL
			);

			if (!bResult) return;

			DWORD dwResult = WaitForSingleObject(directory_watch.overlapped.hEvent, 0);
			if (dwResult != WAIT_OBJECT_0) return;
		}
	});
}

/*
void os::watch(std::wstring_view const path, FFileChangedCallback callback) {
	CDirectoryWatcher::watchers_[hash(path)] = callback;
}
*/
