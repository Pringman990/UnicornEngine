#pragma once
#include <filesystem>

typedef std::filesystem::path Path;

class FileWatcher
{
public:
	FileWatcher();
	~FileWatcher();

	Path TryAssetsGetPathW(const std::wstring& aPath);
	Path TryAssetsGetPathA(const std::string& aPath);

private:

};