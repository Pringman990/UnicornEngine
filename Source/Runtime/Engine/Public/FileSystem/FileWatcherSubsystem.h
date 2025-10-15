#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include "IFileWatcherBackend.h"
#include "EventDispatcher/Notifier.h"

/*
* Engine subsystem
*/
class FileWatcherSubsystem
{
	friend struct subsystem::SubsystemDescriptor;
public:
	ENGINE_API bool Init(const String& Root, SharedPtr<IFileWatcherBackend> Backend);

	ENGINE_API void Watch(const String& Path, Func<void(const FileWatchInfo&)> Callback);

	template<typename T>
	void WatchRaw(const String& Path, T* Owner, void (T::* Method)(const FileWatchInfo&))
	{
		auto& entry = mWatched[Path];
		entry.AddRaw(Owner, Method);
	}

	ENGINE_API void Update();

private:
	FileWatcherSubsystem();
	~FileWatcherSubsystem();

private:
	SharedPtr<IFileWatcherBackend> mBackend;
	UnorderedMap<String, MultiNotifierArgs<FileWatchInfo>> mWatched;
};