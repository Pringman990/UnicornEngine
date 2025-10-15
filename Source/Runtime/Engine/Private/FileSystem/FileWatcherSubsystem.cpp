#include "FileSystem/FileWatcherSubsystem.h"

REGISTER_ENGINE_SUBSYSTEM(FileWatcherSubsystem)

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
		LOG_CRITICAL("Failed to init filewatcher backend");
		return false;
	}

	mBackend->Watch(Root, [&](FileWatchInfo info)
		{
			mWatched[info.file].Notify(info); 
		});

	return true;
}

void FileWatcherSubsystem::Watch(const std::string& Path, std::function<void(const FileWatchInfo&)> Callback)
{
	auto& entry = mWatched[Path];
	entry.AddLambda(Callback);
}

void FileWatcherSubsystem::Update()
{
	mBackend->Update();
}