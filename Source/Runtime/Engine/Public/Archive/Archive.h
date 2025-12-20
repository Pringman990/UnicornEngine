#pragma once
#include "EngineDefines.h"
#include <Core.h>

enum class ArchiveContextType
{
	Object,
	Sequence
};

class Archive
{
public:
	ENGINE_API Archive() = default;
	ENGINE_API virtual ~Archive() = default;

	ENGINE_API virtual OwnedPtr<Archive> CreateSubArchive(const String& Key) = 0;
	ENGINE_API virtual void WriteToFile(PathView FilePath) = 0;

	//--------------- Writing ---------------
	ENGINE_API virtual bool WriteInt32(int32 Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteInt64(int64 Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteUInt32(uint32 Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteUInt64(uint64 Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteLong(long Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteFloat(float Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteDouble(double Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteBool(bool Value, const String& Key = "") = 0;
	ENGINE_API virtual bool WriteString(const String& Value, const String& Key = "") = 0;

	//Array
	ENGINE_API virtual bool BeginWriteArray(const String& Key = "") = 0;
	ENGINE_API virtual void EndWriteArray() = 0;

	//Map
	ENGINE_API virtual bool BeginWriteObject(const String& Key = "") = 0;
	ENGINE_API virtual void EndWriteObject() = 0;

	//--------------- Reading ---------------
	ENGINE_API virtual bool HasKey(std::string_view Key) = 0;

	ENGINE_API virtual bool ReadInt32(int32& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadInt64(int64& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadUInt32(uint32& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadUInt64(uint64& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadLong(long& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadFloat(float& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadDouble(double& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadBool(bool& OutValue, const String& Key = "") = 0;
	ENGINE_API virtual bool ReadString(String& OutValue, const String& Key = "") = 0;

	//Array
	ENGINE_API virtual bool BeginReadArray(const String& Key = "") = 0;
	ENGINE_API virtual void EndReadArray() = 0;
	ENGINE_API virtual uint32 GetArraySize() = 0;

	/**
	* Used for array reads. Jumps the current array index by 1.
	*/
	ENGINE_API virtual void Next() = 0;

	//Map
	ENGINE_API virtual bool BeginReadObject(const String& Key = "") = 0;
	ENGINE_API virtual void EndReadObject() = 0; 
	ENGINE_API virtual bool ReadObjectKey(String& OutKey) = 0; 

private:

};