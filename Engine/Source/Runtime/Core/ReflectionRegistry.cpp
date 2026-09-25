//
// Created on 2026-07-18.
//
#include "../pch.h"
#include "ReflectionRegistry.h"

void RegisterMathTypes();

void refl::ReflectionRegistry::RegisterStandardTypes()
{
    refl::ClassRegistrator<uint8>("uint8", refl::TypeID("40010ea8-b285-4a9c-8596-217454a8b423"))
            .Serialize([](const uint8* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt32(*obj, key);
            })
            .Deserialize([](uint8* obj, Archive& archive, const String& key) -> bool
            {
                uint32 value{};

                if (!archive.ReadUInt32(value, key))
                    return false;

                if (value > UINT8_MAX)
                    return false;

                *obj = static_cast<uint8>(value);
                return true;
            })
            .Finish();

    refl::ClassRegistrator<uint16>("uint16", refl::TypeID("27966a74-82f5-4186-9b9a-1071d66b8217"))
            .Serialize([](const uint16* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt32(*obj, key);
            })
            .Deserialize([](uint16* obj, Archive& archive, const String& key) -> bool
            {
                uint32 value{};

                if (!archive.ReadUInt32(value, key))
                    return false;

                if (value > UINT16_MAX)
                    return false;

                *obj = static_cast<uint16>(value);
                return true;
            })
            .Finish();

    refl::ClassRegistrator<uint32>("uint32", refl::TypeID("3f74b1b5-8e9e-4a5b-83d7-b8e08cdb5b19"))
            .Serialize([](const uint32* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt32(*obj, key);
            })
            .Deserialize([](uint32* obj, Archive& archive, const String& key) -> bool
            {
                uint32 value{};

                if (!archive.ReadUInt32(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<uint64>("uint64", refl::TypeID("2321a469-b425-452c-8d44-2b4d7bc5ae1d"))
            .Serialize([](const uint64* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt64(*obj, key);
            })
            .Deserialize([](uint64* obj, Archive& archive, const String& key) -> bool
            {
                uint64 value{};

                if (!archive.ReadUInt64(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<int8>("int8", refl::TypeID("d76a2a22-4f3a-4cb6-8fb2-72b2c0656d79"))
            .Serialize([](const int8* obj, Archive& archive, const String& key)
            {
                archive.WriteInt32(*obj, key);
            })
            .Deserialize([](int8* obj, Archive& archive, const String& key) -> bool
            {
                int32 value{};

                if (!archive.ReadInt32(value, key))
                    return false;

                if (value > INT8_MAX)
                    return false;

                *obj = static_cast<int8>(value);
                return true;
            })
            .Finish();

    refl::ClassRegistrator<int16>("int16", refl::TypeID("05c097ee-836e-4cf8-ac7f-64287c90ffd3"))
            .Serialize([](const int16* obj, Archive& archive, const String& key)
            {
                archive.WriteInt32(*obj, key);
            })
            .Deserialize([](int16* obj, Archive& archive, const String& key) -> bool
            {
                int32 value{};

                if (!archive.ReadInt32(value, key))
                    return false;

                if (value > INT16_MAX)
                    return false;

                *obj = static_cast<int16>(value);
                return true;
            })
            .Finish();

    refl::ClassRegistrator<int32>("int32", refl::TypeID("bbfa432a-6c37-410c-8222-324a306ba738"))
            .Serialize([](const int32* obj, Archive& archive, const String& key)
            {
                archive.WriteInt32(*obj, key);
            })
            .Deserialize([](int32* obj, Archive& archive, const String& key) -> bool
            {
                int32 value{};

                if (!archive.ReadInt32(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<int64>("int64", refl::TypeID("b17e2fc7-0522-4c48-9ef8-235610c5faa3"))
            .Serialize([](const int64* obj, Archive& archive, const String& key)
            {
                archive.WriteInt64(*obj, key);
            })
            .Deserialize([](int64* obj, Archive& archive, const String& key) -> bool
            {
                int64 value{};

                if (!archive.ReadInt64(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<byte>("byte", refl::TypeID("bde53a28-8e17-4a61-9907-924dad90c0f5"))
            .Serialize([](const byte* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt32(*obj, key);
            })
            .Deserialize([](byte* obj, Archive& archive, const String& key) -> bool
            {
                uint32 value{};

                if (!archive.ReadUInt32(value, key))
                    return false;

                if (value > UINT8_MAX)
                    return false;

                *obj = static_cast<byte>(value);
                return true;
            })
            .Finish();

    refl::ClassRegistrator<f32>("f32", refl::TypeID("9424f940-8676-4bc2-a1f8-5d2a1353b757"))
            .Serialize([](const f32* obj, Archive& archive, const String& key)
            {
                archive.WriteFloat(*obj, key);
            })
            .Deserialize([](f32* obj, Archive& archive, const String& key) -> bool
            {
                f32 value{};

                if (!archive.ReadFloat(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<f64>("f64", refl::TypeID("0c5402b4-6826-4fe7-b061-f0785687a211"))
            .Serialize([](const f64* obj, Archive& archive, const String& key)
            {
                archive.WriteDouble(*obj, key);
            })
            .Deserialize([](f64* obj, Archive& archive, const String& key) -> bool
            {
                f64 value{};

                if (!archive.ReadDouble(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<usize>("usize", refl::TypeID("9c64ad7d-2901-46e5-8067-5696223082dc"))
            .Serialize([](const usize* obj, Archive& archive, const String& key)
            {
                archive.WriteUInt64(*obj, key);
            })
            .Deserialize([](usize* obj, Archive& archive, const String& key) -> bool
            {
                uint64 value{};

                if (!archive.ReadUInt64(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<String>("String", refl::TypeID("d570dd92-a1b5-4c7f-8b2a-085f525786f0"))
            .Serialize([](const String* obj, Archive& archive, const String& key)
            {
                archive.WriteString(*obj, key);
            })
            .Deserialize([](String* obj, Archive& archive, const String& key) -> bool
            {
                String value{};

                if (!archive.ReadString(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    refl::ClassRegistrator<const char*>("String Literal", refl::TypeID("d570ad92-a1b5-4c7f-8b2a-085f525786f0"))
            .Serialize([](const char* const* obj, Archive& archive, const String& key)
            {
                archive.WriteStringLiteral(*obj, key);
            })
            .Deserialize([](const char** obj, Archive& archive, const String& key) -> bool
            {
                const char* value{};

                if (!archive.ReadStringLiteral(value, key))
                    return false;

                *obj = value;
                return true;
            })
            .Finish();

    RegisterMathTypes();
}

void RegisterMathTypes()
{
    refl::ClassRegistrator<glm::vec2>("vec2", refl::TypeID("3067ed63-781c-4495-a165-ba1da6fce802"))
            .Property("X", &glm::vec2::x)
            .Property("Y", &glm::vec2::y)
            .Finish();

    refl::ClassRegistrator<glm::vec3>("vec3", refl::TypeID("dbd98bbc-4395-471c-8615-45ae372dcd65"))
            .Property("X", &glm::vec3::x)
            .Property("Y", &glm::vec3::y)
            .Property("Z", &glm::vec3::z)
            .Finish();

    refl::ClassRegistrator<glm::vec4>("vec4", refl::TypeID("eb709c4a-bc01-42b3-85d8-2dfa3c8d346f"))
            .Property("X", &glm::vec4::x)
            .Property("Y", &glm::vec4::y)
            .Property("Z", &glm::vec4::z)
            .Property("W", &glm::vec4::w)
            .Finish();

    refl::ClassRegistrator<glm::quat>("quat", refl::TypeID("cb2cb11a-c57d-45bd-b64c-592e2d100cd1"))
            .Property("X", &glm::quat::x)
            .Property("Y", &glm::quat::y)
            .Property("Z", &glm::quat::z)
            .Property("W", &glm::quat::w)
            .Finish();
}
