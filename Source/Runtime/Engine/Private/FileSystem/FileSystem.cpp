#include "FileSystem/FileSystem.h"

#include "FileSystem/NativeFileBackend.h"

REGISTER_ENGINE_SUBSYSTEM(FileSystem)

FileSystem::FileSystem()
    :
    mRootPath("../../")
{

}

FileSystem::~FileSystem()
{

}

void FileSystem::Init()
{
    mDefaultFileBackend = MakeOwned<NativeFileBackend>("");

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

bool FileSystem::Exists(const Path& VirtualPath)
{
    String proto;
    Path relativePath;
    
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
        return false;
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return true;
    }

    return false;
}

SharedPtr<IFileStream> FileSystem::Open(const Path& VirtualPath, FileMode Mode)
{
    String proto;
    Path relativePath;
   
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
        return nullptr;
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return mount.backend->Open(relativePath, Mode);
    }
    return nullptr;
}

ByteBuffer FileSystem::ReadAll(const Path& VirtualPath)
{
    String proto;
    Path relativePath;
   
    if (!ParseVirtualPath(VirtualPath, proto, relativePath)) 
        return {};
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return mount.backend->ReadAll(relativePath);
    }
    return {};
}

ENGINE_API ByteBuffer FileSystem::ReadAllNonVirtual(const Path& Path)
{
    return mDefaultFileBackend->ReadAll(Path);
}

void FileSystem::WriteAll(const Path& VirtualPath, const ByteBuffer& Data)
{
    String proto;
    Path relativePath;
    
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
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

Path FileSystem::GetAbsolutPath(const Path& VirtualPath)
{
    String proto;
    Path relativePath;

    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
    {
        LOG_ERROR("Virtual path did not exist: {}", VirtualPath);
        return String();
    }

    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto)
        {
            return mount.backend->GetRoot() + relativePath;
        }
    }

    LOG_ERROR("Virtual path mount did not exist: {}", VirtualPath);
    return Path();
}

bool FileSystem::ParseVirtualPath(const Path& VirtualPath, Protocol& outProtocol, Path& outRelativePath)
{
    size_t pos = VirtualPath.find("://");
    if (pos == String::npos)
    {
        return false;
    }

    outProtocol = VirtualPath.substr(0, pos);
    outRelativePath = VirtualPath.substr(pos + 3);

    return true;
}
