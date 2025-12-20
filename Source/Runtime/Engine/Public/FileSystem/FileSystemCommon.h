#pragma once
#include "StandardTypes/StandardTypes.h"
#include <fstream>
#include <filesystem>

enum class FileMode
{
	Read,
	Write,
	Append
};

class IFileStream
{
public:
	ENGINE_API virtual ~IFileStream() = default;
	ENGINE_API virtual size_t Read(void* OutBuffer, size_t Size) = 0;
	ENGINE_API virtual size_t Write(const void* Buffer, size_t Size) = 0;
	ENGINE_API virtual void Seek(size_t Pos) = 0;
	ENGINE_API virtual size_t Tell() = 0;
	ENGINE_API virtual size_t Length() = 0;
protected:
	std::fstream mStream;
	size_t mLength = 0;
};

class ENGINE_API IFileBackend
{
public:
	virtual ~IFileBackend() = default;
	virtual bool Exists(PathView FilePath) = 0;
	virtual SharedPtr<IFileStream> Open(PathView FilePath, FileMode Mode) = 0;
	virtual ByteBuffer ReadAll(PathView FilePath) = 0;
	virtual void WriteAll(PathView FilePath, const ByteBuffer& Data) = 0;
	virtual const Path& GetRoot() const = 0;
};