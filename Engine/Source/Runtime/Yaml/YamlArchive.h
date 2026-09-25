#pragma once
#include "../Core/Archive.h"
#include <yaml-cpp/yaml.h>

class YamlArchive : public Archive
{
public:
    YamlArchive();
    explicit YamlArchive(const ByteBuffer& Buffer);
    explicit YamlArchive(const YAML::Node& Node);
    virtual ~YamlArchive() override;

    virtual OwnedPtr<Archive> CreateSubArchive(const String& Key) override;
    virtual void WriteToFile(const String& FilePath) override;

    //--------------- Writing ---------------
    virtual bool WriteInt32(int32 Value, const String& Key) override;
    virtual bool WriteInt64(int64 Value, const String& Key) override;
    virtual bool WriteUInt32(uint32 Value, const String& Key) override;
    virtual bool WriteUInt64(uint64 Value, const String& Key) override;
    virtual bool WriteFloat(f32 Value, const String& Key) override;
    virtual bool WriteDouble(f64 Value, const String& Key) override;
    virtual bool WriteBool(bool Value, const String& Key) override;
    virtual bool WriteString(const String& Value, const String& Key) override;
    virtual bool WriteStringLiteral(const char* Value, const String& Key) override;

    //Array
    virtual bool BeginWriteArray(const String& Key) override;
    virtual void EndWriteArray() override;

    //Map
    virtual bool BeginWriteObject(const String& Key) override;
    virtual void EndWriteObject() override;

    //--------------- Reading ---------------
    virtual bool HasKey(const String& Key) override;

    virtual bool ReadInt32(int32& OutValue, const String& Key) override;
    virtual bool ReadInt64(int64& OutValue, const String& Key) override;
    virtual bool ReadUInt32(uint32& OutValue, const String& Key) override;
    virtual bool ReadUInt64(uint64& OutValue, const String& Key) override;
    virtual bool ReadFloat(f32& OutValue, const String& Key) override;
    virtual bool ReadDouble(f64& OutValue, const String& Key) override;
    virtual bool ReadBool(bool& OutValue, const String& Key) override;
    virtual bool ReadString(String& OutValue, const String& Key) override;
    virtual bool ReadStringLiteral(const char* OutValue, const String& Key) override;

    //Array
    virtual bool BeginReadArray(const String& Key) override;
    virtual void EndReadArray() override;
    virtual uint32 GetArraySize() override;

    /**
    * Used for array reads. Jumps the current array index by 1.
    */
    virtual void Next() override;

    //Map
    virtual bool BeginReadObject(const String& Key) override;
    virtual void EndReadObject() override;
    virtual bool ReadObjectKey(String& OutKey) override;

private:

    /**
    * A default writer function for standard types like int/uint/float etc...
    */
    template<typename T>
    bool WriteStandardTypes(const T& Value, std::string_view Key)
    {
        if (mStack.empty())
        {
            LOG_WARNING("No active container");
            return false;
        }

        auto& ctx = mStack.back();

        if (ctx.type == ArchiveContextType::Object)
        {
            if (Key.empty())
            {
                LOG_WARNING("Missing key for object value");
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
            LOG_WARNING("Can't read with no active container");
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

    List<Context> mStack;
};
