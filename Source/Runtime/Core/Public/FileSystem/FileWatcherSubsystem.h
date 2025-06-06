#pragma once
#include <EngineSubsystem.h>
#include "IFileWatcherBackend.h"
#include "EventDispatcher/Notifier.h"

class FileWatcherSubsystem : public EngineSubsystem<FileWatcherSubsystem>
{
	friend class EngineSubsystem<FileWatcherSubsystem>;
public:
	bool Init(const String& Root, SharedPtr<IFileWatcherBackend> Backend);

	void Watch(const String& Path, Func<void(const FileWatchInfo&)> Callback);

	template<typename T>
	void WatchRaw(const String& Path, T* Owner, void (T::* Method)(const FileWatchInfo&))
	{
		auto& entry = mWatched[Path];
		entry.AddRaw(Owner, Method);
	}

	void Update();

private:
	FileWatcherSubsystem();
	~FileWatcherSubsystem();

private:
	SharedPtr<IFileWatcherBackend> mBackend;
	UnorderedMap<String, MultiNotifierArgs<FileWatchInfo>> mWatched;
};