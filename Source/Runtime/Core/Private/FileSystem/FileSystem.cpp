#include "FileSystem/FileSystem.h"

#include "FileSystem/NativeFileBackend.h"

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
    SharedPtr<NativeFileBackend> native = MakeShared<NativeFileBackend>(mRootPath + "Content/");
    Mount("engine", native);
    Mount("game", native);

    SharedPtr<NativeFileBackend> shader = MakeShared<NativeFileBackend>(mRootPath + "Binaries/Shaders/");
    Mount("shader", shader);
}

FileSystem::MountHandle FileSystem::Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority)
{
    MountPoint mp{ Protocol, std::move(Backend), Priority };

    auto& mounts = mMounts;
    mounts.push_back(std::move(mp));

    std::sort(mounts.begin(), mounts.end(), [](const MountPoint& a, const MountPoint& b)
        {
            return a.priority > b.priority;
        });

    return MountHandle{Protocol, mp.backend.get()};
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

bool FileSystem::Exists(const String& VirtualPath)
{
    String proto, relativePath;
    
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
        return false;
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return true;
    }

    return false;
}

SharedPtr<IFileStream> FileSystem::Open(const String& VirtualPath, FileMode Mode)
{
    String proto, relativePath;
   
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
        return nullptr;
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return mount.backend->Open(relativePath, Mode);
    }
    return nullptr;
}

ByteBuffer FileSystem::ReadAll(const String& VirtualPath)
{
    String proto, relativePath;
   
    if (!ParseVirtualPath(VirtualPath, proto, relativePath)) 
        return {};
    
    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto && mount.backend->Exists(relativePath))
            return mount.backend->ReadAll(relativePath);
    }
    return {};
}

void FileSystem::WriteAll(const String& VirtualPath, const ByteBuffer& Data)
{
    String proto, relativePath;
    
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

String FileSystem::GetAbsolutPath(const String& VirtualPath)
{
    String proto, relativePath;
    if (!ParseVirtualPath(VirtualPath, proto, relativePath))
    {
        _LOG_CORE_ERROR("Virtual path did not exist: {}", VirtualPath);
        return String();
    }

    for (auto& mount : mMounts)
    {
        if (mount.protocol == proto)
        {
            return mRootPath + relativePath;
        }
    }

    _LOG_CORE_ERROR("Virtual path mount did not exist: {}", VirtualPath);
    return String();
}

bool FileSystem::ParseVirtualPath(const String& Path, Protocol& outProtocol, String& outRelativePath)
{
    size_t pos = Path.find("://");
    if (pos == String::npos)
    {
        return false;
    }

    outProtocol = Path.substr(0, pos);
    outRelativePath = Path.substr(pos + 3);

    return true;
}
