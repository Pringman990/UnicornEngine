#pragma once
#include "FileSystem/FileSystemCommon.h"

class NativeFileStream : public IFileStream
{
public:
	NativeFileStream(const String& Path, FileMode Mode)
	{
		std::ios_base::openmode iosMode = std::ios::failbit;
		switch (Mode)
		{
		case FileMode::Read: iosMode = std::ios::in | std::ios::binary; break;
		case FileMode::Write: iosMode = std::ios::out | std::ios::binary | std::ios::trunc; break;
		case FileMode::Append: iosMode = std::ios::out | std::ios::binary | std::ios::app; break;
		default:
			assert(false); //Unimplemented mode
			break;
		}

		mStream.open(Path, iosMode);
		mStream.seekg(0, std::ios::end);
		mLength = mStream.tellg();
		mStream.seekg(0, std::ios::beg);
	}

	virtual size_t Read(void* OutBuffer, size_t Size) override
	{
		mStream.read(reinterpret_cast<char*>(OutBuffer), Size);
		return mStream.gcount();
	}

	virtual size_t Write(const void* Buffer, size_t Size) override
	{
		mStream.write(reinterpret_cast<const char*>(Buffer), Size);
		return Size;
	}

	virtual void Seek(size_t Pos) override
	{
		mStream.seekg(Pos);
		mStream.seekp(Pos);
	}

	virtual size_t Tell() override
	{
		return static_cast<size_t>(mStream.tellg());
	}

	virtual size_t Lenght() override
	{
		return mLength;
	}
};

class NativeFileBackend : public IFileBackend
{
public:
	NativeFileBackend() = default;
	NativeFileBackend(const String& Root) : mRoot(Root) {};

	virtual bool Exists(const String& Path) override
	{
		return std::filesystem::exists(mRoot + Path);
	}

	virtual SharedPtr<IFileStream> Open(const String& Path, FileMode Mode) override
	{
		return MakeShared<NativeFileStream>(mRoot + Path, Mode);
	}

	virtual ByteBuffer ReadAll(const String& Path) override
	{
		SharedPtr<IFileStream> stream = Open(Path, FileMode::Read);
		assert(stream);
		
		ByteBuffer data(stream->Lenght());
		stream->Read(data.data(), data.size());
		return data;
	}

	virtual void WriteAll(const String& Path, const ByteBuffer& Data) override
	{
		SharedPtr<IFileStream> stream = Open(Path, FileMode::Write);
		assert(stream);
		stream->Write(Data.data(), Data.size());
	}

private:
	String mRoot;
};