#include "pch.h"
#include "FileWatcher/FileWatcher.h"

#include <Logger/Logger.h>
#include <Utility/Utility.h>

#include <windows.h>

FileWatcher::FileWatcher()
	:
	mMonitoredDirectory(0),
	mOverlapped({ 0 }),
	mBytesReturned(0),
	mBuffer(),
	mRootDirectory("")
{
}

FileWatcher::~FileWatcher()
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

bool FileWatcher::Init()
{
	WCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	mRootDirectory = path;
	if (!std::filesystem::is_directory(mRootDirectory))
	{
		mRootDirectory.remove_filename();
		mRootDirectory = mRootDirectory.parent_path().parent_path().parent_path();
		//mRootDirectory = mRootDirectory / "Content";
		if (!std::filesystem::is_directory(mRootDirectory))
		{
			_LOG_CORE_ERROR("Failed to init FileWatcher, Directory does not exist: %s", WStringToString(path));
			return false;
		}
	}

	mMonitoredDirectory = CreateFile(mRootDirectory.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

	if (mMonitoredDirectory == INVALID_HANDLE_VALUE)
	{
		_LOG_CORE_ERROR("Failed to init FileWatcher, Monitored directory couldn't be opened for I/O");
		CloseHandle(mMonitoredDirectory);
		return false;
	}

	return true;
}

void FileWatcher::Watch()
{
	BOOL result = GetOverlappedResult(mMonitoredDirectory, &mOverlapped, &mBytesReturned, FALSE);
	if (result)
	{
		FILE_NOTIFY_INFORMATION* notifyInformation = (FILE_NOTIFY_INFORMATION*)mBuffer;
		while (notifyInformation)
		{
			ProcessDirectoryChange(notifyInformation);
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
		std::string lastError = std::to_string(GetLastError());
		_LOG_CORE_ERROR("ReadDirectoryChangesW failed, Error Code: " + lastError);
		CloseHandle(mMonitoredDirectory);
		return;
	}
}

void FileWatcher::ProcessDirectoryChange(FILE_NOTIFY_INFORMATION* aNotifyInformation)
{
	std::wstring stringPath = mRootDirectory;
	stringPath.append(aNotifyInformation->FileName, aNotifyInformation->FileNameLength / sizeof(WCHAR));

	std::filesystem::path notifiedPath(stringPath);

#ifdef _DEBUG
	switch (aNotifyInformation->Action)
	{
	case FILE_ACTION_ADDED:
		_LOG_CORE_INFO("File added: %s", WStringToString(stringPath));
		break;
	case FILE_ACTION_REMOVED:
		_LOG_CORE_INFO("File removed %s", WStringToString(stringPath));
		break;
	case FILE_ACTION_MODIFIED:
		_LOG_CORE_INFO("File modified: %s", WStringToString(stringPath));
		break;
	case FILE_ACTION_RENAMED_OLD_NAME:
		_LOG_CORE_INFO("File renamed from: %s", WStringToString(stringPath));
		break;
	case FILE_ACTION_RENAMED_NEW_NAME:
		_LOG_CORE_INFO("File renamed to: %s", WStringToString(stringPath));
		break;
	default:
		_LOG_CORE_INFO("Unknown action: %s", WStringToString(stringPath));
		break;
	}
#endif // _DEBUG

	std::filesystem::path directoryToCheck = notifiedPath.parent_path();

	std::filesystem::directory_entry entry(notifiedPath);
	entry;
	//for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryToCheck))
	//{
	//	SetFileAttributes(entry.path().c_str(), GetFileAttributes(entry.path().c_str()) & ~FILE_ATTRIBUTE_READONLY);
	//
	//}
}

const std::string FileWatcher::GetContentPath() const
{
	return (mRootDirectory / "Content").string();
}