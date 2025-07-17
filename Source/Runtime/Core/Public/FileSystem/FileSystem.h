#pragma once
#include "FileSystemCommon.h"

#define ROOTPATH String("../../")

class FileSystem
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

	static void Init();

	static MountHandle Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority = 0);
	static void UnMount(const MountHandle& Handle);
	static bool Exists(const String& VirtualPath);
	static SharedPtr<IFileStream> Open(const String& VirtualPath, FileMode Mode);
	static ByteBuffer ReadAll(const String& VirtualPath);
	static void WriteAll(const String& VirtualPath, const ByteBuffer& Data);

	static String GetAbsolutPath(const String& VirtualPath);

private:
	FileSystem();
	~FileSystem();

	static Vector<MountPoint>& GetMounts()
	{
		static Vector<MountPoint> mounts;
		return mounts;
	}

	static bool ParseVirtualPath(const String& Path, Protocol& outProtocol, String& outRelativePath);
private:
	static const String sRootPath;
};