#pragma once
#include <Singleton.h>
#include "IFileWatcherBackend.h"
#include "EventDispatcher/Notifier.h"

class FileWatcherSubsystem : public Singleton<FileWatcherSubsystem>
{
	friend class Singleton<FileWatcherSubsystem>;
public:
	bool Init(const String& Root, SharedPtr<IFileWatcherBackend> Backend);

	void Watch(const std::string& path, std::function<void(const FileWatchInfo&)> callback);

	template<typename T>
	void WatchRaw(const std::string& path, T* owner, void (T::* method)(const FileWatchInfo&))
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
	std::unordered_map<String, MultiNotifierArgs<FileWatchInfo>> mWatched;
};