#pragma once

#include <functional>
#include <future>
#include <Windows.h>

#include "types.hpp"

enum class EFileNotifyChange : u16 {
	LastAccess = FILE_NOTIFY_CHANGE_LAST_ACCESS,
	Attributes = FILE_NOTIFY_CHANGE_ATTRIBUTES,
	LastWrite = FILE_NOTIFY_CHANGE_LAST_WRITE,
	FileName = FILE_NOTIFY_CHANGE_FILE_NAME,
	Creation = FILE_NOTIFY_CHANGE_CREATION,
	Security = FILE_NOTIFY_CHANGE_SECURITY,
	DirName = FILE_NOTIFY_CHANGE_DIR_NAME,
	Size = FILE_NOTIFY_CHANGE_SIZE,
};

enum class EFileAction : u8 {
	Added = FILE_ACTION_ADDED,
	Removed = FILE_ACTION_REMOVED,
	Modified = FILE_ACTION_MODIFIED,
	RenamedOldName = FILE_ACTION_RENAMED_OLD_NAME,
	RenamedNewName = FILE_ACTION_RENAMED_NEW_NAME
};

constexpr auto to_string(EFileAction e) {
	switch (e) {
		case EFileAction::Added: return "Added";
		case EFileAction::Removed: return "Removed";
		case EFileAction::Modified: return "Modified";
		case EFileAction::RenamedOldName: return "RenamedOldName";
		case EFileAction::RenamedNewName: return "RenamedNewName";
	}
	return "unknown";
}

using FNotifyFileUpdate = void(EFileAction);

class FileSystem {
public:
	static _STD shared_ptr<FileSystem> instance;
	
	FileSystem();
	FileSystem(FileSystem const &) = delete;
	FileSystem& operator=(FileSystem const &) = delete;
	FileSystem(FileSystem&&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;
	~FileSystem();

	void createListener(_STD string_view file, _STD function<FNotifyFileUpdate> const &callback);
	void removeListener(_STD string_view file);
	void process();

	void close();

protected:

	void createLockHandle();
	void createDirectoryHandle();
	void threadProc();

private:
	_STD mutex m_mutex;
	_STD queue<_STD shared_ptr<_STD packaged_task<void()>>> m_queueTasks;
	_STD unordered_map<u32, _STD function<FNotifyFileUpdate>> m_dtFileNotifications;
	OVERLAPPED m_overlapped;
	HANDLE m_hDirectory;
	HANDLE m_hFileAliveLock;
	_STD wstring m_szTempFilePath;
	_STD jthread m_thread;
};