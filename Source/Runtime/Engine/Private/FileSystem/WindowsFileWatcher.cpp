#include "FileSystem/WindowsFileWatcher.h"

#include <Logger/Logger.h>
#include <Utility/Utility.h>

#include <windows.h>

WindowsFileWatcher::WindowsFileWatcher()
	:
	mMonitoredDirectory(0),
	mOverlapped({ 0 }),
	mBytesReturned(0),
	mBuffer(),
	mRootDirectory("")
{
}

WindowsFileWatcher::~WindowsFileWatcher()
{
	if (mMonitoredDirectory != INVALID_HANDLE_VALUE) {
		CloseHandle(mMonitoredDirectory);
		mMonitoredDirectory = INVALID_HANDLE_VALUE;
	}
	if (mOverlapped.hEvent != NULL) {
		CloseHandle(mOverlapped.hEvent);
		mOverlapped.hEvent = NULL;
	}
}

bool WindowsFileWatcher::Init(const String& RootPath)
{
	mRootDirectory = RootPath;
	if (!std::filesystem::is_directory(mRootDirectory))
	{
		LOG_ERROR("Failed to init WindowsFileWatcher, Directory does not exist: %s", RootPath);
		return false;
	}

	mMonitoredDirectory = CreateFile(mRootDirectory.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

	if (mMonitoredDirectory == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("Failed to init WindowsFileWatcher, Monitored directory couldn't be opened for I/O");
		CloseHandle(mMonitoredDirectory);
		return false;
	}

	return true;
}

void WindowsFileWatcher::Watch(const String& RootPath, FuncType Callback)
{
	FileWatchInfo info;

	BOOL result = GetOverlappedResult(mMonitoredDirectory, &mOverlapped, &mBytesReturned, FALSE);
	if (result)
	{
		FILE_NOTIFY_INFORMATION* notifyInformation = (FILE_NOTIFY_INFORMATION*)mBuffer;
		while (notifyInformation)
		{
			info = ProcessDirectoryChange(notifyInformation);
			if (notifyInformation->NextEntryOffset)
			{
				notifyInformation = (FILE_NOTIFY_INFORMATION*)((BYTE*)notifyInformation + notifyInformation->NextEntryOffset);
			}
			else
			{
				notifyInformation = NULL;
			}
		}
	}

	result = ReadDirectoryChangesW(
		mMonitoredDirectory,
		mBuffer,
		sizeof(mBuffer),
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		&mBytesReturned,
		&mOverlapped,
		NULL
	);

	if (!result)
	{
		String lastError = std::to_string(GetLastError());
		LOG_ERROR("ReadDirectoryChangesW failed, Error Code: " + lastError);
		CloseHandle(mMonitoredDirectory);
		return;
	}

	Callback(info);
}

FileWatchInfo WindowsFileWatcher::ProcessDirectoryChange(FILE_NOTIFY_INFORMATION* NotifyInformation)
{
	FileWatchInfo info;

	std::wstring stringPath = mRootDirectory.wstring();
	stringPath.append(NotifyInformation->FileName, NotifyInformation->FileNameLength / sizeof(WCHAR));

	info.file = WStringToString(stringPath);

	switch (NotifyInformation->Action)
	{
	case FILE_ACTION_ADDED:
		LOG_INFO("File added: {}", WStringToString(stringPath));
		info.state = FileWatchState::Added;
		break;
	case FILE_ACTION_REMOVED:
		LOG_INFO("File removed {}", WStringToString(stringPath));
		info.state = FileWatchState::Removed;
		break;
	case FILE_ACTION_MODIFIED:
		LOG_INFO("File modified: {}", WStringToString(stringPath));
		info.state = FileWatchState::Modified;
		break;
	case FILE_ACTION_RENAMED_OLD_NAME:
		LOG_INFO("File renamed from: {}", WStringToString(stringPath));
		info.state = FileWatchState::NameChange;
		break;
	case FILE_ACTION_RENAMED_NEW_NAME:
		LOG_INFO("File renamed to: {}", WStringToString(stringPath));
		info.state = FileWatchState::NameChange;
		break;
	default:
		LOG_INFO("Unknown action: {}", WStringToString(stringPath));
		info.state = FileWatchState::Unknown;
		break;
	}

	return info;
}
