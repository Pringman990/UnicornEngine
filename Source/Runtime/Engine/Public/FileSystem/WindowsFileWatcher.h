#pragma once
#include <EngineDefines.h>
#include "IFileWatcherBackend.h"
#include <filesystem>
#include <Windows.h>

class WindowsFileWatcher : public IFileWatcherBackend
{
	friend class FileWatcherBackendFactory;
public:
	ENGINE_API bool Init(const String& RootPath) override;
	ENGINE_API virtual void Watch(const String& RootPath, FuncType Callback) override;

	ENGINE_API WindowsFileWatcher();
	ENGINE_API ~WindowsFileWatcher();
private:
	FileWatchInfo ProcessDirectoryChange(FILE_NOTIFY_INFORMATION* NotifyInformation);
private:
private:
	HANDLE mMonitoredDirectory;
	OVERLAPPED mOverlapped;
	DWORD mBytesReturned;
	char mBuffer[4096];

	std::filesystem::path mRootDirectory;
};