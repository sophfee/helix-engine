// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
// ReSharper disable CppCStyleCast
// ReSharper disable CppUseAuto
// ReSharper disable CppTooWideScopeInitStatement
#include "filesystem.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>

#include "util.hpp"

_STD shared_ptr<FileSystem> FileSystem::instance = nullptr;

FileSystem::FileSystem()
	: m_overlapped(), m_hDirectory(NULL), m_hFileAliveLock(NULL) {
	createDirectoryHandle();
	createLockHandle();
	this->m_thread = _STD jthread([&] {
		threadProc();
	});
}

#define CloseHandleSafely(M_HANDLE) if ((M_HANDLE) != NULL && (M_HANDLE) != INVALID_HANDLE_VALUE) CloseHandle(M_HANDLE)
FileSystem::~FileSystem() {
	close();
	CloseHandleSafely(this->m_hFileAliveLock);
	CloseHandleSafely(this->m_overlapped.hEvent);
	CloseHandleSafely(this->m_hDirectory);
}

void FileSystem::createListener(_STD string_view const file, _STD function<FNotifyFileUpdate> const &callback) {
	printf("FileSystem::createListener(%s = %u)\n", _STD string (file.begin(), file.end()).c_str(), hash(file));
	this->m_dtFileNotifications[hash(file)] = callback;
}

void FileSystem::removeListener(_STD string_view const file) {
	this->m_dtFileNotifications.erase(hash(file));
}

void FileSystem::process() {
	if (m_queueTasks.empty()) return;
	_STD lock_guard mtx(m_mutex);
	while (!m_queueTasks.empty()) {
		auto &task = m_queueTasks.front();
		(*task)(); m_queueTasks.pop();
	}
}

void FileSystem::close() {
	DeleteFile(this->m_szTempFilePath.c_str()); // just try
	m_thread.join();
}

void FileSystem::createLockHandle() {
	_STD filesystem::path fsPath;
	{ // drop szPath because its useless allocation after getting module name
		WCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		assert(GetCurrentDirectoryW(MAX_PATH, szPath) != MAX_PATH);
		fsPath = szPath;
	}
	this->m_hFileAliveLock = CreateFileW(
		(fsPath / TEXT(".file-watcher-lock")).c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN,
		NULL);
	this->m_szTempFilePath = fsPath / TEXT(".file-watcher-lock");
	assert(this->m_hFileAliveLock != INVALID_HANDLE_VALUE);
	CloseHandle(this->m_hFileAliveLock);
	this->m_hFileAliveLock = INVALID_HANDLE_VALUE;
}

void FileSystem::createDirectoryHandle() {
	_STD filesystem::path fsPath;
	{ // drop szPath because its useless allocation after getting module name
		WCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, MAX_PATH * sizeof(WCHAR));
		assert(GetCurrentDirectoryW(MAX_PATH, szPath) != MAX_PATH);
		fsPath = szPath;
	}
	
	this->m_hDirectory = CreateFileW(
		fsPath.wstring().c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	wprintf(L"Path: %s\n", fsPath.parent_path().wstring().c_str());
	assert(this->m_hFileAliveLock != INVALID_HANDLE_VALUE);
}

void FileSystem::threadProc() {
	this->m_overlapped.hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
	WORD dwBuffer[1024];
	BOOL bIsRunning = TRUE;
	while (bIsRunning) {
		printf("Let's go again\n");
		BOOL const bResult = ReadDirectoryChangesW(this->m_hDirectory,
			dwBuffer,
			1024,
			TRUE,
			
			FILE_NOTIFY_CHANGE_FILE_NAME  |
			FILE_NOTIFY_CHANGE_DIR_NAME   |
			FILE_NOTIFY_CHANGE_LAST_WRITE,
			NULL,
			&m_overlapped,
			NULL);

		if (!bResult)
			__debugbreak();

		DWORD const dwStatus = WaitForSingleObject(m_overlapped.hEvent, INFINITE);
		if (dwStatus != WAIT_OBJECT_0) __debugbreak();

		DWORD dwBytesTransferred = 0;
		GetOverlappedResult(this->m_hDirectory, &m_overlapped, &dwBytesTransferred, FALSE);
		PFILE_NOTIFY_INFORMATION pFileInfo = (PFILE_NOTIFY_INFORMATION)dwBuffer;
		
		while (true) {
			if (pFileInfo == nullptr) break;

			_STD wstring wszFileName((LPWSTR)pFileInfo->FileName, pFileInfo->FileNameLength / sizeof(WCHAR));
			_STD string szFileName = wstringToString(wszFileName);
			
			if (hash(szFileName) == hash(".file-watcher-lock")) {
				if (pFileInfo->Action == FILE_ACTION_REMOVED) {
					bIsRunning = FALSE;
					break;
				}
			}

			wprintf(L"Event occurred on file -> %s (%hs) (%u)\n", wszFileName.c_str(), to_string((EFileAction)pFileInfo->Action), hash(szFileName));

			if (m_dtFileNotifications.contains(hash(szFileName))) {
				_STD unordered_map<EFileAction, _STD vector<u32>> processed_files;
				wprintf(L"CALLABLE event occurred on file -> %s\n", wszFileName.c_str());
				_STD lock_guard mtx(m_mutex);
				switch (pFileInfo->Action) {
					case FILE_ACTION_ADDED: {
						_STD ranges::borrowed_iterator_t<_STD vector<unsigned> &> result;
						result = _STD ranges::find(processed_files[EFileAction::Added], hash(szFileName));
						if (result == processed_files[EFileAction::Added].end()) { // not found
							processed_files[EFileAction::Added].push_back(hash(szFileName));
							m_queueTasks.push(_STD make_shared<_STD packaged_task<void()>>(_STD bind(m_dtFileNotifications[hash(szFileName)], EFileAction::Added)));
						}
						break;
					}
					case FILE_ACTION_REMOVED: {
						_STD ranges::borrowed_iterator_t<_STD vector<unsigned> &> result;
						result = _STD ranges::find(processed_files[EFileAction::Removed], hash(szFileName));
						if (result == processed_files[EFileAction::Removed].end()) {
							// not found
							processed_files[EFileAction::Removed].push_back(hash(szFileName));
							m_queueTasks.push(_STD make_shared<_STD packaged_task<void()>>(_STD bind(m_dtFileNotifications[hash(szFileName)], EFileAction::Removed)));
						}
						break;
					}
					case FILE_ACTION_MODIFIED: {
						_STD ranges::borrowed_iterator_t<_STD vector<unsigned> &> result;
						result = _STD ranges::find(processed_files[EFileAction::Modified], hash(szFileName));
						if (result == processed_files[EFileAction::Modified].end()) {
							// not found
							processed_files[EFileAction::Modified].push_back(hash(szFileName));
							m_queueTasks.push(_STD make_shared<_STD packaged_task<void()>>(_STD bind(m_dtFileNotifications[hash(szFileName)], EFileAction::Modified)));
						}
						break;
					}
					case FILE_ACTION_RENAMED_NEW_NAME: {
						_STD ranges::borrowed_iterator_t<_STD vector<unsigned> &> result;
						result = _STD ranges::find(processed_files[EFileAction::RenamedNewName], hash(szFileName));
						if (result == processed_files[EFileAction::RenamedNewName].end()) {
							// not found
							processed_files[EFileAction::RenamedNewName].push_back(hash(szFileName));
							m_queueTasks.push(_STD make_shared<_STD packaged_task<void()>>(_STD bind(m_dtFileNotifications[hash(szFileName)], EFileAction::RenamedNewName)));
						}
						break;
					}
					case FILE_ACTION_RENAMED_OLD_NAME: {
						_STD ranges::borrowed_iterator_t<_STD vector<unsigned> &> result;
						result = _STD ranges::find(processed_files[EFileAction::RenamedOldName], hash(szFileName));
						if (result == processed_files[EFileAction::RenamedOldName].end()) {
							// not found
							processed_files[EFileAction::RenamedOldName].push_back(hash(szFileName));
							m_queueTasks.push(_STD make_shared<_STD packaged_task<void()>>(_STD bind(m_dtFileNotifications[hash(szFileName)], EFileAction::RenamedOldName)));
						}
						break;
					}
					default:
						break;
				}
			}

			if (pFileInfo->NextEntryOffset)
				*(u8**)&pFileInfo+=pFileInfo->NextEntryOffset;
			else
				break;
		}
	}
}

#undef CloseHandleSafely