#include "FileSystem/FileWatcherSubsystem.h"

FileWatcherSubsystem::FileWatcherSubsystem()
{
};

FileWatcherSubsystem::~FileWatcherSubsystem()
{
	for (auto& [path, notifier] : mWatched)
		notifier.RemoveAll();

	mWatched.clear();
}

bool FileWatcherSubsystem::Init(const String& Root, SharedPtr<IFileWatcherBackend> Backend)
{
	if (Backend == nullptr)
	{
		assert(Backend);
		return false;
	}

	mBackend = Backend;
	if (!mBackend->Init(Root))
	{
		_LOG_CORE_CRITICAL("Failed to init filewatcher backend");
		return false;
	}

	mBackend->Watch(Root, [&](FileWatchInfo info)
		{
			mWatched[info.file].Notify(info);
		});

	return true;
}

void FileWatcherSubsystem::Watch(const std::string& path, std::function<void(const FileWatchInfo&)> callback)
{
	auto& entry = mWatched[path];
	entry.AddLambda(callback);
}

void FileWatcherSubsystem::Update()
{
	mBackend->Update();
}