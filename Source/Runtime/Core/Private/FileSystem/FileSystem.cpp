#include "FileSystem/FileSystem.h"

#include "FileSystem/NativeFileBackend.h"

const String FileSystem::sRootPath = "../../";

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

void FileSystem::Init()
{
    SharedPtr<NativeFileBackend> native = MakeShared<NativeFileBackend>(sRootPath + "Content/");
    Mount("engine", native);
    Mount("game", native);

    SharedPtr<NativeFileBackend> shader = MakeShared<NativeFileBackend>(sRootPath + "Binaries/Shaders/");
    Mount("shader", shader);
}

FileSystem::MountHandle FileSystem::Mount(const Protocol& Protocol, SharedPtr<IFileBackend> Backend, int32 Priority)
{
    MountPoint mp{ Protocol, std::move(Backend), Priority };

    auto& mounts = GetMounts();
    mounts.push_back(std::move(mp));

    std::sort(mounts.begin(), mounts.end(), [](const MountPoint& a, const MountPoint& b)
        {
            return a.priority > b.priority;
        });

    return MountHandle{Protocol, mp.backend.get()};
}

void FileSystem::UnMount(const MountHandle& Handle)
{
    auto& mounts = GetMounts();
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
    
    for (auto& mount : GetMounts()) 
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
    
    for (auto& mount : GetMounts())
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
    
    for (auto& mount : GetMounts()) 
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
   
    for (auto& mount : GetMounts()) 
    {
        if (mount.protocol == proto) 
        {
            mount.backend->WriteAll(relativePath, Data);
            return;
        }
    }
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