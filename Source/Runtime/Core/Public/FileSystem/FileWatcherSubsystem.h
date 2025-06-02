#pragma once
#include <Singleton.h>
#include "IFileWatcherBackend.h"
#include "EventDispatcher/Notifier.h"

class FileWatcherSubsystem : public Singleton<FileWatcherSubsystem>
{
	friend class Singleton<FileWatcherSubsystem>;
public:
	bool Init(const String& Root, SharedPtr<IFileWatcherBackend> Backend);

	void Watch(const String& path, Func<void(const FileWatchInfo&)> callback);

	template<typename T>
	void WatchRaw(const String& path, T* owner, void (T::* method)(const FileWatchInfo&))
	{
		auto& entry = mWatched[path];
		entry.AddRaw(owner, method);
	}

	void Update();

private:
	FileWatcherSubsystem();
	~FileWatcherSubsystem();

private:
	SharedPtr<IFileWatcherBackend> mBackend;
	UnorderedMap<String, MultiNotifierArgs<FileWatchInfo>> mWatched;
};