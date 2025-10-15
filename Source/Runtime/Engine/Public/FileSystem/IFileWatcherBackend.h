#pragma once
#include "StandardTypes/StandardTypes.h"
#include <functional>

enum class FileWatchState
{
	Added,
	Removed,
	Modified,
	NameChange,
	Unknown
};

struct FileWatchInfo
{
	String file;
	FileWatchState state;

	//Only relevent on NameChange state
	String oldName;
	String newName;
};

class IFileWatcherBackend
{
public:
	using FuncType = Func<void(FileWatchInfo)>;

	IFileWatcherBackend() {};
	virtual ~IFileWatcherBackend() = default;

	virtual bool Init(const String& RootPath) = 0;

	virtual void Watch(const String& RootPath, FuncType Callback) = 0;

	//For backends that needs polling
	virtual void Update() {};

private:

};