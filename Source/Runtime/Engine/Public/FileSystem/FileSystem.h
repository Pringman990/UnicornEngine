#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include "FileSystemCommon.h"

#define ROOTPATH Path("../../")

class NativeFileBackend;

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
		String protocol;
		IFileBackend* backend; // For identity match
	};

	ENGINE_API void Init();

	ENGINE_API MountHandle Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority = 0);
	ENGINE_API void UnMount(const MountHandle& Handle);
	ENGINE_API bool Exists(const Path& VirtualPath);
	ENGINE_API SharedPtr<IFileStream> Open(const Path& VirtualPath, FileMode Mode);
	ENGINE_API ByteBuffer ReadAll(const Path& VirtualPath);
	ENGINE_API ByteBuffer ReadAllNonVirtual(const Path& Path);
	ENGINE_API void WriteAll(const Path& VirtualPath, const ByteBuffer& Data);

	ENGINE_API Path GetAbsolutPath(const Path& VirtualPath);

protected:
	FileSystem();
	~FileSystem();

	bool ParseVirtualPath(const Path& VirtualPath, Protocol& outProtocol, Path& outRelativePath);
private:
	Path mRootPath;
	Vector<MountPoint> mMounts;
	OwnedPtr<NativeFileBackend> mDefaultFileBackend;
};

#define GET_FILESYSTEM() SubsystemManager::Get<FileSystem>()