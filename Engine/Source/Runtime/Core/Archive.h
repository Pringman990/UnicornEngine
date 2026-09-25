#pragma once

enum class ArchiveContextType
{
	Object,
	Sequence
};

class Archive
{
public:
	Archive() = default;
	virtual ~Archive() = default;

	virtual OwnedPtr<Archive> CreateSubArchive(const String& Key) = 0;
	virtual void WriteToFile(const String& FilePath) = 0;

	//--------------- Writing ---------------
	virtual bool WriteInt32(int32 Value, const String& Key) = 0;
	virtual bool WriteInt64(int64 Value, const String& Key) = 0;
	virtual bool WriteUInt32(uint32 Value, const String& Key) = 0;
	virtual bool WriteUInt64(uint64 Value, const String& Key) = 0;
	virtual bool WriteFloat(f32 Value, const String& Key) = 0;
	virtual bool WriteDouble(f64 Value, const String& Key) = 0;
	virtual bool WriteBool(bool Value, const String& Key) = 0;
	virtual bool WriteString(const String& Value, const String& Key) = 0;
	virtual bool WriteStringLiteral(const char* Value, const String& Key) = 0;

	//Array
	virtual bool BeginWriteArray(const String& Key) = 0;
	virtual void EndWriteArray() = 0;

	//Map
	virtual bool BeginWriteObject(const String& Key) = 0;
	virtual void EndWriteObject() = 0;

	//--------------- Reading ---------------
	virtual bool HasKey(const String& Key) = 0;

	virtual bool ReadInt32(int32& OutValue, const String& Key) = 0;
	virtual bool ReadInt64(int64& OutValue, const String& Key) = 0;
	virtual bool ReadUInt32(uint32& OutValue, const String& Key) = 0;
	virtual bool ReadUInt64(uint64& OutValue, const String& Key) = 0;
	virtual bool ReadFloat(f32& OutValue, const String& Key) = 0;
	virtual bool ReadDouble(f64& OutValue, const String& Key) = 0;
	virtual bool ReadBool(bool& OutValue, const String& Key) = 0;
	virtual bool ReadString(String& OutValue, const String& Key) = 0;
	virtual bool ReadStringLiteral(const char* OutValue, const String& Key) = 0;

	//Array
	virtual bool BeginReadArray(const String& Key) = 0;
	virtual void EndReadArray() = 0;
	virtual uint32 GetArraySize() = 0;

	/**
	* Used for array reads. Jumps the current array index by 1.
	*/
	virtual void Next() = 0;

	//Map
	virtual bool BeginReadObject(const String& Key) = 0;
	virtual void EndReadObject() = 0;
	virtual bool ReadObjectKey(String& OutKey) = 0;

private:

};