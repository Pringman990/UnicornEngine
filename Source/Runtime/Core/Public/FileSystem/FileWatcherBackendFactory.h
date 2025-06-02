#pragma once
#include "FileSystem/WindowsFileWatcher.h"

class FileWatcherBackendFactory
{
public:
	static SharedPtr<IFileWatcherBackend> Create()
	{
#if defined(_WIN64) || defined(_WIN32)
		return MakeShared<WindowsFileWatcher>();
#else
		return nullptr;
#endif
	}

private:
	FileWatcherBackendFactory() {};
	~FileWatcherBackendFactory() {};
};