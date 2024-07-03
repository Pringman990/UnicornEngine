#pragma once
#include <filesystem>

typedef std::filesystem::path Path;

class FileWatcher
{
public:
	FileWatcher();
	~FileWatcher();

	Path TryGetAssetsPathW(const std::wstring& aPath);
	Path TryGetAssetsPathA(const std::string& aPath);

private:

};