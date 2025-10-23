#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include "FileSystemCommon.h"

#define ROOTPATH String("../../")

/*
* Engine subsystem
*/
class FileSystem 
{
	friend struct subsystem::SubsystemDescriptor;

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

	ENGINE_API void Init();

	ENGINE_API MountHandle Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority = 0);
	ENGINE_API void UnMount(const MountHandle& Handle);
	ENGINE_API bool Exists(const String& VirtualPath);
	ENGINE_API SharedPtr<IFileStream> Open(const String& VirtualPath, FileMode Mode);
	ENGINE_API ByteBuffer ReadAll(const String& VirtualPath);
	ENGINE_API void WriteAll(const String& VirtualPath, const ByteBuffer& Data);

	ENGINE_API String GetAbsolutPath(const String& VirtualPath);

protected:
	FileSystem();
	~FileSystem();

	bool ParseVirtualPath(const String& Path, Protocol& outProtocol, String& outRelativePath);
private:
	String mRootPath;
	Vector<MountPoint> mMounts;
};

#define GET_FILESYSTEM() SubsystemManager::Get<FileSystem>()