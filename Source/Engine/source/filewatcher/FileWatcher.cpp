#include "EnginePch.h"
#include "FileWatcher.h"

FileWatcher::FileWatcher()
{

}

FileWatcher::~FileWatcher()
{

}

Path FileWatcher::TryAssetsGetPathW(const std::wstring& aPath)
{
	Path path = aPath;
	if (std::filesystem::exists(path))
	{
		return path;
	}

	return Path();
}

Path FileWatcher::TryAssetsGetPathA(const std::string& aPath)
{
	Path path = aPath;
	if (std::filesystem::exists(path))
	{
		return path;
	}

	return Path();
}
