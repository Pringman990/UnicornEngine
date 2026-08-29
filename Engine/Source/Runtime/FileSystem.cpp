#include "pch.h"
#include "FileSystem.h"

#if defined(_WIN32)
#include <windows.h>
#endif

FileSystem::FileSystem()
{
 	mRoot = GetExecutablePath();
}

FileSystem::~FileSystem() = default;

Expected<ByteBuffer, FileSystem::ReadError> FileSystem::ReadAll(const Path& filePath)
{
	const String rawPath = filePath.string();

	const String mount = GetMount(rawPath);

	Path path;
	auto it = mMountToRoot.find(mount);
	if (it != mMountToRoot.end())
	{
		path /= it->second;
	}
	else
	{
		path /= mRoot / filePath;
	}

	path += StripMount(rawPath);

	if (!std::filesystem::exists(path))
	{
		return Unexpected(FileSystem::ReadError({
			.code = FileSystem::ReadError::Code::FileNotFound,
			.message = "File does not exist",
			.filePath = path.string(),
		}));
	}

	// ifstream constructor with path and ios modes runs ifstream::open(). So no need for manual call to open().
	std::ifstream stream(path, std::ios::binary | std::ios::ate);
	if (!stream)
	{
		return Unexpected(FileSystem::ReadError({
			.code = FileSystem::ReadError::Code::PermissionDenied,
			.message = "Failed to open file stream",
			.filePath = path.string(),
		}));
	}

	// We use ios::ate which puts us at the end so no need to seek before.
	const std::streampos end = stream.tellg();
	if (end < 0)
	{
		return Unexpected(FileSystem::ReadError({
			.code = FileSystem::ReadError::Code::InvalidSize,
			.message = "tellg() failed",
			.filePath = path.string(),
		}));
	}

	const auto size = static_cast<size_t>(end);
	stream.seekg(0, std::ios::beg);

	ByteBuffer buffer;
	try
	{
		buffer.resize(size);
	}
	catch (...)
	{
		return Unexpected(FileSystem::ReadError({
			.code = FileSystem::ReadError::Code::OutOfMemory,
			.message = "Memory allocation failed",
			.filePath = path.string(),
		}));
	}

	stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));

	if (!stream || stream.gcount() != size)
	{
		return Unexpected(FileSystem::ReadError({
			.code = FileSystem::ReadError::Code::ReadFailure,
			.message = "Failed while reading file",
			.filePath = path.string(),
		}));
	}

	return buffer;
}

Expected<void, FileSystem::WriteError> FileSystem::WriteAll(const Path& filePath, const ByteBuffer& data)
{
	const String rawPath = filePath.string();

	String mount = GetMount(rawPath);

	Path path;
	auto it = mMountToRoot.find(mount);
	if (it != mMountToRoot.end())
	{
		path /= it->second;
	}

	path += StripMount(rawPath);

	auto parent = path.parent_path();
	if (!parent.empty() && !std::filesystem::exists(parent))
	{
		return Unexpected(FileSystem::WriteError({
			.code = FileSystem::WriteError::Code::InvalidPath,
			.message = "Parent directory does not exist",
			.filePath = path.string(),
		}));
	}

	std::ofstream stream(path, std::ios::binary | std::ios::trunc);
	if (!stream)
	{
		return Unexpected(FileSystem::WriteError({
			.code = FileSystem::WriteError::Code::PermissionDenied,
			.message = "Failed to open file stream",
			.filePath = path.string(),
		}));
	}

	stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

	if (!stream)
	{
		return Unexpected(FileSystem::WriteError({
			.code = FileSystem::WriteError::Code::WriteFailure,
			.message = "Failed while writing to file",
			.filePath = path.string(),
		}));
	}

	return {};
}

void FileSystem::AddMount(const String& mountPoint, const String& relativePath)
{
	auto it = mMountToRoot.find(mountPoint);
	if (it != mMountToRoot.end())
	{
		LOG_WARNING("MountPoint {} already exists (ignored)", mountPoint);
		return;
	}

	mMountToRoot[mountPoint] = mRoot / relativePath;
}

String FileSystem::GetMount(const String& path)
{
	auto pos = path.find("://");
	if (pos == String::npos)
		return {};

	// remove "://" as pos is character before ":".
	return path.substr(0, pos + 3);
}

String FileSystem::StripMount(const String& path)
{
	auto pos = path.find("://");
	if (pos == String::npos)
		return "";

	// skip "://"
	return path.substr(pos + 3);
}

Path FileSystem::GetExecutablePath()
{
#if defined(_WIN32)
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);

	// We step path back to the parent three times to land on root: Root/Binaries/Debug/.exe
	return Path(buffer).parent_path().parent_path().parent_path();
#else
	char result[1024];
	ssize_t count = readlink("/proc/self/exe", result, sizeof(result));
	return Path(String(result, count)).parent_path().parent_path().parent_path();
#endif

}
