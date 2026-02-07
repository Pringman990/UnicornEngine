#pragma once
#include <Core.h>
#include "Archive.h"
#include "YAML/YamlHelper.h"

class YamlArchive : public Archive
{

public:
	ENGINE_API YamlArchive();
	ENGINE_API YamlArchive(const ByteBuffer& Buffer);
	ENGINE_API YamlArchive(YAML::Node Node);
	ENGINE_API ~YamlArchive() override;

	ENGINE_API virtual OwnedPtr<Archive> CreateSubArchive(const String& Key) override;
	ENGINE_API virtual void WriteToFile(PathView FilePath) override;

	//--------------- Writing ---------------
	ENGINE_API virtual bool WriteInt32(int32 Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteInt64(int64 Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteUInt8(uint8 Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteUInt32(uint32 Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteUInt64(uint64 Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteLong(long Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteFloat(float Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteDouble(double Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteBool(bool Value, const String& Key = "") override;
	ENGINE_API virtual bool WriteString(const String& Value, const String& Key = "") override;

	//Array
	ENGINE_API virtual bool BeginWriteArray(const String& Key = "") override;
	ENGINE_API virtual void EndWriteArray() override;

	//Map
	ENGINE_API virtual bool BeginWriteObject(const String& Key = "") override;
	ENGINE_API virtual void EndWriteObject() override;

	//--------------- Reading ---------------
	ENGINE_API virtual bool HasKey(std::string_view Key) override;

	ENGINE_API virtual bool ReadInt32(int32& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadInt64(int64& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadUInt8(uint8& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadUInt32(uint32& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadUInt64(uint64& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadLong(long& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadFloat(float& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadDouble(double& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadBool(bool& OutValue, const String& Key = "") override;
	ENGINE_API virtual bool ReadString(String& OutValue, const String& Key = "") override;

	//Array
	ENGINE_API virtual bool BeginReadArray(const String& Key = "") override;
	ENGINE_API virtual void EndReadArray() override;
	ENGINE_API virtual uint32 GetArraySize() override;
	/**
	* Used for array reads. Jumps the current array index by 1.
	*/
	ENGINE_API virtual void Next() override;

	//Map
	ENGINE_API virtual bool BeginReadObject(const String& Key = "") override;
	ENGINE_API virtual void EndReadObject() override;
	ENGINE_API virtual bool ReadObjectKey(String& OutKey) override;

private:

	/**
	* A default writer function for standard types like int/uint/float etc...
	*/
	template<typename T>
	bool WriteStandardTypes(const T& Value, std::string_view Key)
	{
		if (mStack.empty())
		{
			LOG_ERROR("No active container");
			return false;
		}

		auto& ctx = mStack.back();

		if (ctx.type == ArchiveContextType::Object)
		{
			if (Key.empty())
			{
				LOG_ERROR("Missing key for object value");
				return false;
			}

			ctx.node[Key] = Value;
		}
		else
		{
			ctx.node.push_back(Value);
		}

		return true;
	}

	/**
	* A default reader function for standard types like int/uint/float etc...
	*/
	template<typename T>
	bool ReadStandardTypes(T& Value, std::string_view Key)
	{
		if (mStack.empty())
		{
			LOG_ERROR("Can't read with no active container");
			return false;
		}

		auto& ctx = mStack.back();

		try
		{
			if (ctx.type == ArchiveContextType::Object)
			{
				Value = ctx.node[Key].as<T>();
			}
			else
			{
				Value = ctx.node[ctx.currentArrayIndex].as<T>();
			}
			return true;
		}
		catch (YAML::ParserException& e)
		{
			LOG_WARNING("Failed to get yaml, message: ", e.msg);
			return false;
		}

		return false;
	}
private:
	struct Context
	{
		ArchiveContextType type;
		YAML::Node node;

		//Used for reads with the Next function.
		uint32 currentArrayIndex = 0;

		YAML::const_iterator mapIt;
		YAML::const_iterator mapEnd;
	};

	Vector<Context> mStack;
};