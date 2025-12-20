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

	enum class PathKind
	{
		Virtual,
		Relative,
		Absolute,
		Invalid
	};

	struct MountHandle
	{
		String protocol;
		IFileBackend* backend; // For identity match
	};

	ENGINE_API static FileSystem* Instance();

	ENGINE_API void Init();

	ENGINE_API MountHandle Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority = 0);
	ENGINE_API void UnMount(const MountHandle& Handle);
	ENGINE_API bool Exists(PathView FilePath);
	ENGINE_API SharedPtr<IFileStream> Open(PathView FilePath, FileMode Mode);
	
	ENGINE_API ByteBuffer ReadAll(PathView FilePath);
	ENGINE_API void WriteAll(PathView FilePath, const ByteBuffer& Data);

	ENGINE_API Path GetAbsolutPath(PathView FilePath);

protected:
	FileSystem();
	~FileSystem();

	bool ParseVirtualPath(PathView FilePath, Protocol& outProtocol, Path& outRelativePath);

	PathKind GetPathKind(PathView FilePath);
	void WriteAllVirtual(PathView FilePath, const ByteBuffer& Data);
	void WriteAllAbsolute(PathView Path, const ByteBuffer& Data);

	ByteBuffer ReadAllVirtual(PathView FilePath);
	ByteBuffer ReadAllAbsolute(PathView FilePath);
private:
	static FileSystem* sInstance;

	Path mRootPath;
	Vector<MountPoint> mMounts;
	OwnedPtr<NativeFileBackend> mAbsoluteFileBackend;
};