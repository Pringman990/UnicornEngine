#include "FileSystem/FileWatcherSubsystem.h"

FileWatcherSubsystem* FileWatcherSubsystem::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(FileWatcherSubsystem)

FileWatcherSubsystem::FileWatcherSubsystem()
{
#ifdef _DEBUG
	ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
	sInstance = this;
};

FileWatcherSubsystem::~FileWatcherSubsystem()
{
	for (auto& [path, notifier] : mWatched)
		notifier.RemoveAll();

	mWatched.clear();

	if (sInstance == this)
		sInstance = nullptr;
}

ENGINE_API FileWatcherSubsystem* FileWatcherSubsystem::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
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