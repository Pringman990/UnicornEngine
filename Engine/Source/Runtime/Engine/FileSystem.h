#pragma once

class FileSystem
{
	INIT_SERVICE(FileSystem)

public:
	struct ReadError
	{
		enum class Code
		{
			FileNotFound,
			PermissionDenied,
			ReadFailure,
			InvalidSize,
			OutOfMemory
		} code;

		String message;
		String filePath;
	};

	struct WriteError
	{
		enum class Code
		{
			InvalidPath,
			PermissionDenied,
			WriteFailure,
		} code;

		String message;
		String filePath;
	};
public:

	FileSystem();
	~FileSystem();

	NODISC Expected<ByteBuffer, FileSystem::ReadError> ReadAll(const Path& filePath);
	Expected<void, FileSystem::WriteError> WriteAll(const Path& filePath, const ByteBuffer& data);

	void AddMount(const String& mountPoint, const String& relativePath);

private:
	static String GetMount(const String& path);
	static String StripMount(const String& path);

	NODISC static Path GetExecutablePath();
private:
	UnorderedMap<String, Path> mMountToRoot;
	Path mRoot;
};
