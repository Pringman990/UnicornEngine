#include "Reflection/ReflectionRegistry.h"

#include <YAML/YamlHelper.h>

#pragma region Primitive Registration
//C26444 is from the class builder getting create and destroyed directly without doing anything.
//But it actually still registers because its done in the Constructor and Destructor.
REFL_DEFINE(int)
{
	refl::ClassBuilder<int>(STRINGIFYEXP(int), "14645678-1234-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				int32* value = reinterpret_cast<int32*>(obj);
				archive.WriteInt32(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{	
				int32* value = reinterpret_cast<int32*>(obj);
				return archive.ReadInt32(*value, key);
			});
}
REFL_DEFINE(long)
{
	refl::ClassBuilder<long>(STRINGIFYEXP(long), "45678339-2345-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				long* value = reinterpret_cast<long*>(obj);
				archive.WriteLong(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				long* value = reinterpret_cast<long*>(obj);
				return archive.ReadLong(*value, key);
			});
}
REFL_DEFINE(uint32)
{
	refl::ClassBuilder<uint32>(STRINGIFYEXP(uint32), "12345678-1234-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				uint32* value = reinterpret_cast<uint32*>(obj);
				archive.WriteUInt32(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				uint32* value = reinterpret_cast<uint32*>(obj);
				return archive.ReadUInt32(*value, key);
			});
}
REFL_DEFINE(uint64)
{
	refl::ClassBuilder<uint64>(STRINGIFYEXP(uint64), "45678999-2345-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				uint64* value = reinterpret_cast<uint64*>(obj);
				archive.WriteUInt64(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				uint64* value = reinterpret_cast<uint64*>(obj);
				return archive.ReadUInt64(*value, key);
			});
}
REFL_DEFINE(float)
{
	refl::ClassBuilder<float>(STRINGIFYEXP(float), "23456789-2345-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				float* value = reinterpret_cast<float*>(obj);
				archive.WriteFloat(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				float* value = reinterpret_cast<float*>(obj);
				return archive.ReadFloat(*value, key);
			});
}
REFL_DEFINE(double)
{
	refl::ClassBuilder<double>(STRINGIFYEXP(double), "34567899-2345-49be-ae05-46c8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				double* value = reinterpret_cast<double*>(obj);
				archive.WriteDouble(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				double* value = reinterpret_cast<double*>(obj);
				return archive.ReadDouble(*value, key);
			});
}
REFL_DEFINE(bool)
{
	refl::ClassBuilder<bool>(STRINGIFYEXP(bool), "34567899-2345-49be-ae05-46f8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				bool* value = reinterpret_cast<bool*>(obj);
				archive.WriteBool(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				bool* value = reinterpret_cast<bool*>(obj);
				return archive.ReadBool(*value, key);
			});
}
REFL_DEFINE(String)
{
	refl::ClassBuilder<String>(STRINGIFYEXP(String), "34567869-2345-49be-ae05-46f8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				String* value = reinterpret_cast<String*>(obj);
				archive.WriteString(*value, key);

			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				String* value = reinterpret_cast<String*>(obj);
				return archive.ReadString(*value, key);
			});
}
REFL_DEFINE(Vector3)
{
	refl::ClassBuilder<Vector3>(STRINGIFYEXP(Vector3), "31567899-2345-49be-ae05-46f8cc74bd10")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				Vector3 value = *reinterpret_cast<Vector3*>(obj);
				archive.BeginWriteObject(key);
				archive.WriteFloat(value.x, "x");
				archive.WriteFloat(value.y, "y");
				archive.WriteFloat(value.z, "z");
				archive.EndWriteObject();
			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				bool succeded = true;
				Vector3* value = reinterpret_cast<Vector3*>(obj);
				archive.BeginReadObject(key);
				succeded &= archive.ReadFloat((*value).x, "x");
				succeded &= archive.ReadFloat((*value).y, "y");
				succeded &= archive.ReadFloat((*value).z, "z");
				archive.EndReadObject();

				return succeded;
			});
}

#pragma endregion

ENGINE_API refl::ReflectionRegistry::ReflectionRegistry()
{

}

ENGINE_API refl::ReflectionRegistry::~ReflectionRegistry()
{
}

ENGINE_API refl::ReflectionRegistry& refl::ReflectionRegistry::GetInstance()
{
	static ReflectionRegistry instance;
	return instance;
}

ENGINE_API void refl::ReflectionRegistry::RegisterType(Type& InType)
{
	UniqueID128 uuid = InType.uuid;
	std::type_index idx = InType.typeIndex;

	auto it = mRegisteredTypes.find(uuid);
	if (it != mRegisteredTypes.end())
	{
		mRegisteredTypes[uuid] = MakeOwned<Type>(std::move(InType));
		mIndexToUUID[idx] = uuid;
		return;
	}

	if (mIndexToUUID.contains(idx))
	{
		UniqueID128 invalidUUID = mIndexToUUID[idx];
		OwnedPtr<Type>& type = mRegisteredTypes[invalidUUID];
		*type = InType;

		mRegisteredTypes[uuid] = std::move(type);
		mIndexToUUID[idx] = uuid;

		mRegisteredTypes.erase(invalidUUID);
		return;
	}

	mRegisteredTypes.insert({ uuid, MakeOwned<Type>(std::move(InType)) });
	mIndexToUUID.insert({ idx, uuid });
}
