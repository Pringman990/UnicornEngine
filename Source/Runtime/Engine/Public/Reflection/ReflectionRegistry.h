#pragma once
#include "EventDispatcher/Notifier.h"
#include "Logger/Logger.h"

#include <Utility/Utility.h>
#include <UniqueID/UniqueID128.h>
#include <StandardTypes/StandardTypes.h>
#include <span>

#define _DEBUG_PRINT_REFLECTION_REGISTRY 0

#if _DEBUG_PRINT_REFLECTION_REGISTRY
#define REFLECTION_LOG_TYPE_INFO(INFO) \
        LOG_INFO("======================="); \
        LOG_INFO("Name: {}", INFO->schema.name); \
        LOG_INFO("UUID: {}", INFO->schema.uuid.ToString()); \
        LOG_INFO("Flag: {}", INFO->schema.flags); \
        LOG_INFO("Size: {}", INFO->schema.size); \
		for(auto field : INFO->schema.fields) \
		{ \
			LOG_INFO("----------------"); \
			LOG_INFO("Field Name: {}", field.name); \
			LOG_INFO("Type: {}", field.type); \
			LOG_INFO("UUID: {}", field.uuid.ToString()); \
			LOG_INFO("Offset: {}", field.offset); \
			LOG_INFO("Size: {}", field.size); \
		} \
        LOG_INFO("======================="); 
#else
#define REFLECTION_LOG_TYPE_INFO(INFO)
#endif

namespace Reflect
{
	/*
	* Compile time reflect field info
	*/
	struct FieldInfoCompile
	{
		std::string_view name;
		std::string_view type;
		UniqueID128 uuid;
		size_t offset;
		size_t size;

		static constexpr FieldInfoCompile MakeInfo(std::string_view Name, std::string_view Type, UniqueID128 UUID, size_t Offset, size_t Size)
		{
			return {
				Name,
				Type,
				UUID,
				Offset,
				Size
			};
		}
	};

	/*
	* Compile time reflect type schema
	*/
	struct TypeSchemaCompile
	{
		std::string_view name{};
		std::string_view flags{};
		UniqueID128 uuid{};
		size_t size{};
		std::span<const FieldInfoCompile> fields{};

		ConstructFunc constructFunc = nullptr;
		DestroyFunc destroyFunc = nullptr;
		CopyFunc copyFunc = nullptr;
		MoveFunc moveFunc = nullptr;

		constexpr TypeSchemaCompile() noexcept = default;

		constexpr TypeSchemaCompile(
			std::string_view n,
			std::string_view f,
			UniqueID128 u,
			size_t s,
			std::span<const FieldInfoCompile> m,
			ConstructFunc construct,
			DestroyFunc destroy,
			CopyFunc copy,
			MoveFunc move
		)
			:
			name(n),
			uuid(u),
			size(s),
			fields(m),
			flags(f),
			constructFunc(construct),
			destroyFunc(destroy),
			copyFunc(copy),
			moveFunc(move)
		{
		}

		template<typename T, typename R>
		static constexpr TypeSchemaCompile MakeInfo(std::string_view Name, std::string_view Flags, UniqueID128 UUID)
		{
			return {
				Name,
				Flags,
				UUID,
				sizeof(T),
				std::span{R::_fields},

				//Default Contructor
				[](void* Dst)
				{
					new (Dst) T();
				},
				
				//Destructor
				[](void* Obj)
				{
					reinterpret_cast<T*>(Obj)->~T();
				},

				//Copy Contructor
				[](void* Dst, const void* Src)
				{
					new (Dst) T(*reinterpret_cast<const T*>(Src));
				},

				//Move Contructor
				[](void* Dst, void* Src)
				{
					new (Dst) T(std::move(*reinterpret_cast<T*>(Src)));
				}
			};
		}
	};

	/*
	* Runtime time reflect field info
	*/
	struct FieldInfo
	{
		String name;
		String type;
		UniqueID128 uuid;
		size_t offset;
		size_t size;

		/*
		* Makes a deep copy of compile time field info.
		* Used to move from dll memory to engine memory for safety.
		*/
		static FieldInfo CopyCompileToRuntime(const FieldInfoCompile& Compile)
		{
			FieldInfo runtime;
			runtime.name = String(Compile.name);
			runtime.type = String(Compile.type);
			runtime.uuid = UniqueID128(Compile.uuid);
			runtime.offset = Compile.offset;
			runtime.size = Compile.size;

			return runtime;
		}
	};

	/*
	* Runtime time reflect type schema
	*/
	struct TypeSchema
	{
		String name{};
		String flags{};
		UniqueID128 uuid{};
		size_t size{};
		size_t hash{};
		Vector<FieldInfo> fields{};

		/*
		* These are not safe when dll unloads, these point directly to dll memory.
		* Always check if they are valid before calling
		*/
		ConstructFunc constructFunc = nullptr;
		DestroyFunc destroyFunc = nullptr;
		CopyFunc copyFunc = nullptr;
		MoveFunc moveFunc = nullptr;

		static TypeSchema CopyCompileToRuntime(const TypeSchemaCompile* Compile)
		{
			TypeSchema runtime;
			runtime.name = String(Compile->name);
			runtime.flags = String(Compile->flags);
			runtime.uuid = UniqueID128(Compile->uuid);
			runtime.size = Compile->size;

			for (const auto& field : Compile->fields)
			{
				FieldInfo runtimeField;
				runtimeField = FieldInfo::CopyCompileToRuntime(field);
				runtime.fields.push_back(runtimeField);
			}

			runtime.constructFunc = Compile->constructFunc;
			runtime.destroyFunc = Compile->destroyFunc;
			runtime.copyFunc = Compile->copyFunc;
			runtime.moveFunc = Compile->moveFunc;

			runtime.hash = GetSchemaHash(runtime);

			return runtime;
		}

		static size_t GetSchemaHash(const TypeSchema& Schema)
		{
			size_t hash = 0;
			//TODO: make a hash from fields to later check against when reloading dll
			for (const auto& field : Schema.fields)
			{
				hash_combine(hash, field.uuid);
			}

			return hash;
		}

		/*
		* Used when unloading owning module
		*/
		static void Invalidate(TypeSchema& Schema)
		{
			Schema.constructFunc = nullptr;
			Schema.destroyFunc = nullptr;
			Schema.copyFunc = nullptr;
			Schema.moveFunc = nullptr;
		}
	};

	struct ReflectTypeInfo
	{
		TypeSchema schema;
		bool loaded = false;
	};
}

template<typename T>
constexpr bool IsTypeReflected = false;

namespace __Reflection
{
	ENGINE_API void RegisterType(const Reflect::TypeSchemaCompile* Schema);
	ENGINE_API void NullifyType(UniqueID128 ID);
	ENGINE_API Vector<const Reflect::ReflectTypeInfo*> GetAllTypes();
	ENGINE_API const Reflect::ReflectTypeInfo* TryGetInfo(UniqueID128 ID);

	template<typename T>
	ENGINE_API const Reflect::ReflectTypeInfo* TryGetInfo();
}

class ReflectionRegistry
{
public:
	static const Reflect::ReflectTypeInfo* TryGetInfo(UniqueID128 ID)
	{
		return __Reflection::TryGetInfo(ID);
	}

	template<typename T>
	static const Reflect::ReflectTypeInfo* TryGetInfo()
	{
		return __Reflection::TryGetInfo<T>();
	}

	static Vector<const Reflect::ReflectTypeInfo*> GetAllInfos()
	{
		return __Reflection::GetAllTypes();
	}

	//static MultiNotifierArgs<const Reflect::ReflectTypeInfo*> OnTypeRegistered;

private:
	ReflectionRegistry();
	~ReflectionRegistry();

	template<typename T>
	static UniqueID128 GetUUID() { return UniqueID128(); };

private:

};

#define REFLECT_SCHEMA(REFLECT, TYPE, Flags, Uuid, ...) \
	static constexpr auto _fields = std::to_array<Reflect::FieldInfoCompile>({__VA_ARGS__ }); \
	static constexpr Reflect::TypeSchemaCompile _schema = Reflect::TypeSchemaCompile::MakeInfo<TYPE, REFLECT>(#TYPE, Flags, Uuid);

#define REFLECT_FIELD(OWNER, FIELD, TYPE, Uuid) \
	Reflect::FieldInfoCompile::MakeInfo(#FIELD, TYPE, UniqueID128(Uuid), offsetof(OWNER, FIELD), sizeof(decltype(OWNER::FIELD)))

#define DEFINE_GETUUID_TYPEINFO(TYPE) \
	template<> \
	constexpr UniqueID128 ReflectionRegistry::GetUUID<TYPE>();

#define DECLARE_GETUUID_TYPEINFO(REFLECT, TYPE) \
	template<> \
	constexpr UniqueID128 ReflectionRegistry::GetUUID<TYPE>() { return REFLECT::_schema.uuid; }

#define DEFINE_TYPE_UUID(ID) \
	static constexpr UniqueID128 _uuid = UniqueID128(ID);


//exe static types register
//load dll
//module loader tells reflection that we want to register a manager and all its types
//dll types register with a lazy manager per dll
//module loader tells reflection that we are done with this module and it can go back to default

namespace refl
{
	//using CastFunc = void* (*)(void*);

	//struct ClassData
	//{
	//	Vector<TypeView> baseTypes; //Classes this class inherits from
	//	Vector<TypeView> derivedTypes; //Classes that inherits from this class
	//	Vector<CastFunc> conversions; //Conversion function for converting this class to another (e.g, int->float, customClass->otherCustomClass)
	//	Vector<Property> properties; //Properties owned by this class
	//	Vector<Method> methods; //Methods declared in this class

	//	//Types defined in this class (e.g, 
	//	// class Garden
	//	// { 
	//	// public: 
	//	//		enum GardenType 
	//	//		{
	//	//			Grass, Gravel
	//	//		}
	//	// }
	//	//)
	//	Vector<TypeView> nestedTypes;

	//	Vector<Constructor> ctors; //Constructors
	//	Destructor dtor; //Deconstructor
	//};

	//struct TypeData
	//{
	//	String name;
	//	size_t size;

	//	ClassData classData;
	//};

	//template<typename Class>
	//class ClassBuilder
	//{
	//public:
	//	ClassBuilder() {};
	//	~ClassBuilder() {};

	//	template<typename... Args>
	//	ClassBuilder& Constructor();

	//	template<typename T>
	//	ClassBuilder& Property(std::string_view Name, T P);

	//	template<typename T, typename G, typename S = std::nullptr_t>
	//	ClassBuilder& Property(std::string_view Name, T P, G Getter, S Setter);

	//	template<typename F>
	//	ClassBuilder& Method(std::string_view Name, F Function);

	//private:
	//	Vector<std::pair<const void*, Func<void(void)>>> mList;
	//};

	//class Variant
	//{
	//public:
	//	Variant();
	//	~Variant();

	//private:

	//};

	//class PropertyDataBase
	//{
	//public:
	//	PropertyDataBase(std::string_view PropertyName) {};
	//	virtual ~PropertyDataBase() {};

	//	virtual bool IsValid() const noexcept { return false; } ;
	//	virtual Variant GetValue()const { return Variant(); } ;

	//	virtual bool SetValue(Argument Value) const { return false; };

	//private:
	//	std::string_view mName;
	//};

	//template<typename Getter, typename Setter>
	//class PropertyData;

	//template<typename Getter, typename Setter>
	//class PropertyData<Getter, Setter> : public PropertyDataBase
	//{
	//public:
	//	PropertyDataGetterSetter(std::string_view PropertyName) : PropertyDataBase(PropertyName){};
	//	virtual ~PropertyDataGetterSetter() {};

	//	virtual bool IsValid() const noexcept override { return false; };
	//	virtual Variant GetValue() const override { return Variant(); };
	//
	//	bool SetValue(Argument Value) const override { return true; };

	//private:
	//	Getter mGetter;
	//	Setter mSetter;
	//};

	//template<typename Getter>
	//class PropertyData<Getter, void> : public PropertyDataBase
	//{
	//public:
	//	PropertyDataGetterSetter(std::string_view PropertyName) : PropertyDataBase(PropertyName) {};
	//	virtual ~PropertyDataGetterSetter() {};

	//	virtual bool IsValid() const noexcept override { return false; };
	//	virtual Variant GetValue() const override { return Variant(); };

	//	bool SetValue(Argument Value) const override { return false; };

	//private:
	//	Getter mGetter;
	//};

	//class Property
	//{
	//public:
	//	Property();
	//	~Property();



	//private:
	//	const PropertyDataBase* mData;
	//};
}