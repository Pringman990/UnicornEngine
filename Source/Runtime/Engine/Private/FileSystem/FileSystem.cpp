#include "FileSystem/FileSystem.h"

#include "FileSystem/NativeFileBackend.h"

FileSystem* FileSystem::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(FileSystem)

FileSystem::FileSystem()
    :
    mRootPath("../../")
{
#ifdef _DEBUG
    ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
    sInstance = this;
}

FileSystem::~FileSystem()
{
    if (sInstance == this)
        sInstance = nullptr;
}

ENGINE_API FileSystem* FileSystem::Instance()
{
#ifdef _DEBUG
    ASSERT(sInstance, "Filesystem registry was accessed before/after it was created/destroyed");
#endif
    return sInstance;
}

void FileSystem::Init()
{
    mAbsoluteFileBackend = MakeOwned<NativeFileBackend>("");

    SharedPtr<NativeFileBackend> root = MakeShared<NativeFileBackend>(mRootPath);
    Mount("root", root);

    SharedPtr<NativeFileBackend> native = MakeShared<NativeFileBackend>(mRootPath + "Content/");
    Mount("engine", native);
    Mount("game", native);

    SharedPtr<NativeFileBackend> shader = MakeShared<NativeFileBackend>(mRootPath + "Source/Runtime/Renderer/RawShaders/Public/");
    Mount("shaders", shader);
}

FileSystem::MountHandle FileSystem::Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority)
{
    MountPoint mp{ Protocol, std::move(Backend), Priority };

    auto& mounts = mMounts;
    mounts.push_back(std::move(mp));

    MountHandle mp2{ Protocol, mounts.back().backend.get()};

    std::sort(mounts.begin(), mounts.end(), [](const MountPoint& a, const MountPoint& b)
        {
            return a.priority > b.priority;
        });

    return mp2;
}

void FileSystem::UnMount(const MountHandle& Handle)
{
    auto& mounts = mMounts;
    mounts.erase(std::remove_if(mounts.begin(), mounts.end(), [&](const MountPoint& mp) 
        {
            return mp.protocol == Handle.protocol && mp.backend.get() == Handle.backend;
        }),
        mounts.end()
    );
}

bool FileSystem::Exists(PathView FilePath)
{
    FileSystem::PathKind kind = GetPathKind(FilePath);
    switch (kind)
    {
    case FileSystem::PathKind::Virtual:
    {
        String proto;
        Path relativePath;

        if (!ParseVirtualPath(FilePath, proto, relativePath))
            return false;

        for (auto& mount : mMounts)
        {
            if (mount.protocol == proto && mount.backend->Exists(relativePath))
                return true;
        }
        break;
    }
    case FileSystem::PathKind::Relative:
        THROW("Not Implemented!!!!");
        break;
    case FileSystem::PathKind::Absolute:
    {
        return std::filesystem::exists(FilePath);
    }
    default:
        break;
    }

    return false;
}

SharedPtr<IFileStream> FileSystem::Open(PathView FilePath, FileMode Mode)
{
    FileSystem::PathKind kind = GetPathKind(FilePath);
    switch (kind)
    {
    case FileSystem::PathKind::Virtual:
    {
        String proto;
        Path relativePath;

        if (!ParseVirtualPath(FilePath, proto, relativePath))
            return nullptr;

        for (auto& mount : mMounts)
        {
            if (mount.protocol == proto && mount.backend->Exists(relativePath))
                return mount.backend->Open(relativePath, Mode);
        }
        return nullptr;
    }
    case FileSystem::PathKind::Relative:
        THROW("Not Implemented!!!!");
        break;
    case FileSystem::PathKind::Absolute:
    {
        return mAbsoluteFileBackend->Open(FilePath, Mode);
    }
    default:
        break;
    }

    return nullptr;
}

ByteBuffer FileSystem::ReadAll(PathView FilePath)
{
    FileSystem::PathKind kind = GetPathKind(FilePath);
    switch (kind)
    {
    case FileSystem::PathKind::Virtual:
        return ReadAllVirtual(FilePath);
    case FileSystem::PathKind::Relative:
        THROW("Not Implemented!!!!");
        break;
    case FileSystem::PathKind::Absolute:
        return ReadAllAbsolute(FilePath);
    default:
        break;
    }

    return {};
}

void FileSystem::WriteAll(PathView FilePath, const ByteBuffer& Data)
{
    FileSystem::PathKind kind = GetPathKind(FilePath);
    switch (kind)
    {
    case FileSystem::PathKind::Virtual:
        WriteAllVirtual(FilePath, Data);
        break;
    case FileSystem::PathKind::Relative:
        THROW("Not Implemented!!!!");
        break;
    case FileSystem::PathKind::Absolute:
        WriteAllAbsolute(FilePath, Data);
        break;
    default:
        break;
    }
}

void FileSystem::WriteAllVirtual(PathView FilePath, const ByteBuffer& Data)
{
    String proto;
    Path relativePath;

    if (!ParseVirtualPath(FilePath, proto, relativePath))
        return;

    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto)
        {
            mount.backend->WriteAll(relativePath, Data);
            return;
        }
    }
}

void FileSystem::WriteAllAbsolute(PathView FilePath, const ByteBuffer& Data)
{
    mAbsoluteFileBackend->WriteAll(FilePath, Data);
}

ByteBuffer FileSystem::ReadAllVirtual(PathView FilePath)
{
    String proto;
    Path relativePath;

    if (!ParseVirtualPath(FilePath, proto, relativePath))
        return {};

    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return mount.backend->ReadAll(relativePath);
    }
    return {};
}

ByteBuffer FileSystem::ReadAllAbsolute(PathView FilePath)
{
    return mAbsoluteFileBackend->ReadAll(FilePath);
}

Path FileSystem::GetAbsolutPath(PathView FilePath)
{
    String proto;
    Path relativePath;

    if (!ParseVirtualPath(FilePath, proto, relativePath))
    {
        LOG_ERROR("Virtual path did not exist: {}", FilePath);
        return String();
    }

    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto)
        {
            return mount.backend->GetRoot() + relativePath;
        }
    }

    LOG_ERROR("Virtual path mount did not exist: {}", FilePath);
    return Path();
}

bool FileSystem::ParseVirtualPath(PathView FilePath, Protocol& outProtocol, Path& outRelativePath)
{
    size_t pos = FilePath.find("://");
    if (pos == String::npos)
    {
        return false;
    }

    outProtocol = FilePath.substr(0, pos);
    outRelativePath = FilePath.substr(pos + 3);

    return true;
}

FileSystem::PathKind FileSystem::GetPathKind(PathView FilePath)
{
    if (FilePath.find("://") != String::npos)
        return FileSystem::PathKind::Virtual;

    if (std::filesystem::path(FilePath).is_absolute())
        return FileSystem::PathKind::Absolute;

    if (!FilePath.empty())
        return FileSystem::PathKind::Relative;

    return FileSystem::PathKind::Invalid;
}
