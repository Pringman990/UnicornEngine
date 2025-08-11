#pragma once
#include "FileSystemCommon.h"

#define ROOTPATH String("../../")

class FileSystem : public EngineSubsystem<FileSystem>
{
	using Protocol = String;

	struct MountPoint
	{
		Protocol protocol;
		SharedPtr<IFileBackend> backend;
		int32 priority;
	};

public:
	struct MountHandle
	{
		std::string protocol;
		IFileBackend* backend; // For identity match
	};

	FileSystem();
	~FileSystem();

	void Init();

	MountHandle Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority = 0);
	void UnMount(const MountHandle& Handle);
	bool Exists(const String& VirtualPath);
	SharedPtr<IFileStream> Open(const String& VirtualPath, FileMode Mode);
	ByteBuffer ReadAll(const String& VirtualPath);
	void WriteAll(const String& VirtualPath, const ByteBuffer& Data);

	String GetAbsolutPath(const String& VirtualPath);

private:

	bool ParseVirtualPath(const String& Path, Protocol& outProtocol, String& outRelativePath);
private:
	String mRootPath;
	Vector<MountPoint> mMounts;
};