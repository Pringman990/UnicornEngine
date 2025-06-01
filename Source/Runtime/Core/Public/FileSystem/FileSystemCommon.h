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
	virtual ~IFileStream() = default;
	virtual size_t Read(void* OutBuffer, size_t Size) = 0;
	virtual size_t Write(const void* Buffer, size_t Size) = 0;
	virtual void Seek(size_t Pos) = 0;
	virtual size_t Tell() = 0;
	virtual size_t Lenght() = 0;
protected:
	std::fstream mStream;
	size_t mLength = 0;
};

class IFileBackend
{
public:
	virtual ~IFileBackend() = default;
	virtual bool Exists(const String& Path) = 0;
	virtual SharedPtr<IFileStream> Open(const String& Path, FileMode Mode) = 0;
	virtual ByteBuffer ReadAll(const String& Path) = 0;
	virtual void WriteAll(const String& Path, const ByteBuffer& Data) = 0;
};