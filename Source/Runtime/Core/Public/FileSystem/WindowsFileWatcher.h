#pragma once
#include "IFileWatcherBackend.h"
#include <filesystem>

class WindowsFileWatcher : public IFileWatcherBackend
{
	friend class FileWatcherBackendFactory;
public:
	virtual bool Init(const String& RootPath) override;
	virtual void Watch(const String& RootPath, FuncType Callback) override;

	WindowsFileWatcher();
	~WindowsFileWatcher();
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