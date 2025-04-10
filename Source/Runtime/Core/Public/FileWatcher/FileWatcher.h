#pragma once
#include <Singleton.h>
#include <filesystem>

class FileWatcher : public Singleton<FileWatcher>
{
public:

	bool Init();
	void Watch();

	const std::string GetContentPath() const;

private:
	void ProcessDirectoryChange(FILE_NOTIFY_INFORMATION* aNotifyInformation);
private:
	friend class Singleton<FileWatcher>;
	FileWatcher();
	~FileWatcher();
private:
	HANDLE mMonitoredDirectory;
	OVERLAPPED mOverlapped;
	DWORD mBytesReturned;
	char mBuffer[4096];

	std::filesystem::path mRootDirectory;
};