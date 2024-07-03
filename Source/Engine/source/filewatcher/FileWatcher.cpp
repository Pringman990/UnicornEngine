#include "EnginePch.h"
#include "FileWatcher.h"

FileWatcher::FileWatcher()
{

}

FileWatcher::~FileWatcher()
{

}

Path FileWatcher::TryGetAssetsPathW(const std::wstring& aPath)
{
	Path path = aPath;
	if (std::filesystem::exists(path))
	{
		return path;
	}

	return Path();
}

Path FileWatcher::TryGetAssetsPathA(const std::string& aPath)
{
	Path path = aPath;
	if (std::filesystem::exists(path))
	{
		return path;
	}

	return Path();
}
