#pragma once
#include "Subsystem/EngineSubsystem.h"
#include "EventDispatcher/Notifier.h"
#include "Logger/Logger.h"

#include <Utility/Utility.h>
#include <UniqueID/UniqueID128.h>
#include <StandardTypes/StandardTypes.h>
#include <span>
#include <execution>

#include <YAML/YamlHelper.h>
#include <Archive/Archive.h>

#define _DEBUG_PRINT_REFLECTION_REGISTRY 1

#if _DEBUG_PRINT_REFLECTION_REGISTRY
#define REFLECTION_LOG_TYPE_INFO(INFO) \
        LOG_INFO("======================="); \
        LOG_INFO("Name: {}", INFO->name); \
        LOG_INFO("UUID: {}", INFO->uuid.ToString()); \
        LOG_INFO("Type Index: {}", INFO->typeIndex.name()); \
        LOG_INFO("Size: {}", INFO->size); \
		for(auto prop : INFO->properties) \
		{ \
			LOG_INFO("	----------------"); \
			LOG_INFO("	Property Name: {}", prop.name); \
			LOG_INFO("	Offset: {}", prop.offset); \
			LOG_INFO("	Type: {}", prop.type->name); \
			LOG_INFO("	Attributes:"); \
			for(auto att : prop.attributes) \
			{ \
				LOG_INFO("		"); \
			}\
		} \
        LOG_INFO("======================="); 
#else
#define REFLECTION_LOG_TYPE_INFO(INFO)
#endif

namespace refl
{
	constexpr size_t StackLimit = 256;

	using TypeSaveFunction = Func<void(void* obj, Archive& archive, String key)>;
	using TypeLoadFunction = Func<bool(void* obj, Archive& archive, String key)>;
	using TypeDeferedRegisterFunction = void(*)();
	
	struct VectorOps
	{
		size_t (*size)(void* vec);
		void   (*clear)(void* vec);
		void*  (*get)(void* vec, size_t index);
		void   (*push_back)(void* vec, void* element);
	};

	template<typename T>
	struct is_reflected : std::false_type {};

	template<typename T>
	concept IsReflected = refl::is_reflected<T>::value;

	template<typename T>
	struct is_component : std::false_type {};

	template<typename T>
	concept IsComponent = refl::is_component<T>::value && IsReflected<T>;

	template<typename T>
	struct is_vector : std::false_type {};

	template<typename T>
	struct is_vector<Vector<T>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_vector_v = is_vector<std::decay_t<T>>::value;

	template<typename T>
	concept IsVector = is_vector<Vector<T>>::value;

	template<typename T>
	struct is_primitive_type : std::false_type {};

	template<typename T>
	inline constexpr bool is_primitive_type_v = is_primitive_type<T>::value;

	template<typename T1>
	struct templated_reflection_1
	{
		static constexpr bool templated = false;
	};

	enum class Attribute
	{
		DisplayName,
		Private,
		EComponent,
	};

	using AttributeVariant = Variant<std::monostate, String>;

	enum class TypeTrait
	{
		Undefined,
		Primitive,
		Class,
		Array
	};

	struct Property
	{
		String name; //Property name for owning type. If you want the type name then first access the type pointer
		std::ptrdiff_t offset = 0;

		const struct Type* type = nullptr;

		UnorderedMap<Attribute, AttributeVariant> attributes;
	};

	struct TypeFunctions
	{
		Constructor constructor = nullptr;
		Deconstructor deconstructor = nullptr;
		Moveconstructor moveconstructor = nullptr;
		Copyconstructor copyconstructor = nullptr;

		TypeSaveFunction saveFunction = nullptr;
		TypeLoadFunction loadFunction = nullptr;

		void Nullify()
		{
			constructor = nullptr;
			deconstructor = nullptr;
			moveconstructor = nullptr;
			copyconstructor = nullptr;

			saveFunction = nullptr;
			loadFunction = nullptr;
		}
	};

	struct Type
	{
		String name;
		size_t size = 0;

		UniqueID128 uuid;
		std::type_index typeIndex = typeid(void);

		TypeTrait trait = TypeTrait::Undefined;

		Vector<Property> properties;
		UnorderedMap<Attribute, AttributeVariant> attributes;

		TypeFunctions functions;
		VectorOps vectorOps;

		bool isFullyReflected = false;
		bool isValid = false;
	};

	class ReflectionRegistry
	{
		enum class Phase
		{
			Runtime,
			Static
		};

	public:

		ENGINE_API static ReflectionRegistry* Instance();

		ENGINE_API void RegisterType(Type& InType);

		template<IsReflected T>
		void MarkInvalid()
		{
			std::type_index idx = typeid(T);
			auto it = mIndexToUUID.find(idx);
			if (it != mIndexToUUID.end())
			{
				mRegisteredTypes[it->second]->isValid = false;
				mRegisteredTypes[it->second]->functions.Nullify();
			}
		}

		template<typename T>
		const Type* GetOrCreateType()
		{
			std::type_index idx = typeid(T);
			auto it = mIndexToUUID.find(idx);
			if (it != mIndexToUUID.end())
			{
				return mRegisteredTypes[it->second].get();
			}
			
			UniqueID128 randomUUID = UniqueID128::FromRandom();

			OwnedPtr<Type> type = MakeOwned<Type>();
			type->name = String("NON REFLECTED_") + idx.name();
			type->uuid = randomUUID;
			type->typeIndex = idx;
			type->functions.saveFunction = RegisterDefaultSaveFunction(type.get());
			type->functions.loadFunction = RegisterDefaultLoadFunction(type.get());
			
			if constexpr (is_primitive_type_v<T>)
			{
				type->trait = TypeTrait::Primitive;
			}
			else if constexpr (is_vector_v<T>)
			{
				type->trait = TypeTrait::Array;

				using ElemT = typename T::value_type;

				const Type* elementType = GetOrCreateType<ElemT>();

				Property prop;
				prop.name = elementType->name;
				prop.offset = 0;
				prop.type = elementType;

				type->properties.push_back(std::move(prop));

				type->functions.saveFunction = RegisterDefaultVectorSaveFunction<T>(type.get());
				type->functions.loadFunction = RegisterDefaultVectorLoadFunction<T>(type.get());

				type->vectorOps.clear = [](void* vec)
					{
						return static_cast<T*>(vec)->clear();
					};

				type->vectorOps.get = [](void* vec, size_t index) -> void*
					{
						return &(*static_cast<T*>(vec))[index];
					};

				type->vectorOps.push_back = [](void* vec, void* element)
					{
						return static_cast<T*>(vec)->push_back(std::move(*static_cast<ElemT*>(element)));
					};

				type->vectorOps.size = [](void* vec) -> size_t
					{
						return static_cast<T*>(vec)->size();
					};
			}
			else if constexpr (std::is_class_v<T>)
			{
				type->trait = TypeTrait::Class;

			}
			else
			{
				type->trait = TypeTrait::Undefined;
			}


			mIndexToUUID[idx] = type->uuid;
			mRegisteredTypes[type->uuid] = std::move(type);
			return mRegisteredTypes[randomUUID].get();
		}

		template<typename T>
		const Type* GetOrNull()
		{
			std::type_index idx = typeid(T);
			auto it = mIndexToUUID.find(idx);
			if (it != mIndexToUUID.end())
			{
				return mRegisteredTypes[it->second].get();
			}

			return nullptr;
		}

		const Type* GetOrNull(const UniqueID128& UUID)
		{
			auto it = mRegisteredTypes.find(UUID);
			if (it != mRegisteredTypes.end())
			{
				return it->second.get();
			}

			return nullptr;
		}

		const Vector<const Type*> GetAllTypes() const
		{
			Vector<const Type*> types;
			types.reserve(mRegisteredTypes.size());

			for (auto& [uuid, type] : mRegisteredTypes)
			{
				types.emplace_back(type.get());
			}

			return types;
		}

		const Vector<const Type*> GetAllTypesWithAttribute(const Attribute& Att) const
		{
			Vector<const Type*> types;
			types.reserve(mRegisteredTypes.size());

			for (auto& [uuid, type] : mRegisteredTypes)
			{
				if (type->attributes.contains(Att))
					types.emplace_back(type.get());
			}

			return types;
		}

		static TypeSaveFunction RegisterDefaultSaveFunction(const Type* InType)
		{
			return [InType](void* obj, Archive& archive, String key)
				{
					const Type* type = ReflectionRegistry::Instance()->GetOrNull(InType->uuid);
					for (auto& prop : type->properties)
					{
						if (!prop.type->functions.saveFunction)
						{
							LOG_WARNING("Trying to save a type which has no save function. This is okay if the developer intended it");
							continue;
						}

						void* propPtr = (char*)obj + prop.offset;
						prop.type->functions.saveFunction(propPtr, archive, prop.name);
					}
				};
		}

		static TypeLoadFunction RegisterDefaultLoadFunction(const Type* InType)
		{
			return [InType](void* obj, Archive& archive, String key) -> bool
				{
					const Type* type = ReflectionRegistry::Instance()->GetOrNull(InType->uuid);
					for (auto& prop : type->properties)
					{
						if (!prop.type->functions.loadFunction)
						{
							LOG_WARNING("Trying to save a type which has no save function. This is okay if the developer intended it");
							continue;
						}

						void* propPtr = (char*)obj + prop.offset;
						prop.type->functions.loadFunction(propPtr, archive, prop.name);
					}

					return true;
				};
		}

		template<IsVector VecT>
		static TypeSaveFunction RegisterDefaultVectorSaveFunction(const Type* InType)
		{
			return [InType](void* obj, Archive& archive, String key)
				{
					VecT& vec = *static_cast<VecT*>(obj);
					const Type* elementType = InType->properties[0].type;

					archive.BeginWriteArray(key);

					for (auto& elem : vec)
					{
						elementType->functions.saveFunction(&elem, archive, "");
					}

					archive.EndWriteArray();
				};
		}

		template<IsVector VecT>
		static TypeLoadFunction RegisterDefaultVectorLoadFunction(const Type* InType)
		{
			return [InType](void* obj, Archive& archive, String key) -> bool
				{
					VecT& vec = *static_cast<VecT*>(obj);
					const Type* elementType = InType->properties[0].type;

					archive.BeginReadArray(key);
					uint32 arrSize = archive.GetArraySize();
					for (uint32 i = 0; i < arrSize; i++)
					{
						alignas(std::max_align_t) byte stackBuf[StackLimit];
						void* tempData = stackBuf;

						if (elementType->size > StackLimit)
							tempData = malloc(elementType->size);

						elementType->functions.constructor(tempData);
						
						elementType->functions.loadFunction(tempData, archive, "");
						InType->vectorOps.push_back(obj, tempData);
						
						elementType->functions.deconstructor(tempData);

						if (tempData != stackBuf)
							free(tempData);

						archive.Next();
					}

					archive.EndReadArray();

					return true;
				};
		}

	private:
		friend struct subsystem::SubsystemDescriptor;

		ENGINE_API ReflectionRegistry();
		ENGINE_API ~ReflectionRegistry();

	private:

		UnorderedMap<UniqueID128, OwnedPtr<Type>> mRegisteredTypes;
		UnorderedMap<std::type_index, UniqueID128> mIndexToUUID;
	};

	template<typename T>
	class ClassBuilderCommon
	{
	protected:
		Type mType;

		~ClassBuilderCommon()
		{
			if constexpr (is_component<T>::value)
			{
				mType.attributes[refl::Attribute::EComponent] = {};
			}

			mType.isFullyReflected = true;
			mType.isValid = true;
			ReflectionRegistry::Instance()->RegisterType(mType);
		}

		void Init(const String& Name, const String& UUID)
		{
			const Type* type = ReflectionRegistry::Instance()->GetOrCreateType<T>();

			mType = *type;

			mType.name = Name;
			mType.uuid = UniqueID128(UUID);
			mType.size = sizeof(T);
			mType.typeIndex = typeid(T);

			RegisterBasicTypeOps();
		}

		void RegisterBasicTypeOps()
		{
			if constexpr (std::is_default_constructible_v<T>)
				mType.functions.constructor = [](void* dst) { new (dst) T(); };

			if constexpr (std::is_trivially_destructible_v<T>)
				mType.functions.deconstructor = [](void* obj) { reinterpret_cast<T*>(obj)->~T(); };

			if constexpr (std::is_copy_constructible_v<T>)
				mType.functions.copyconstructor = [](void* dst, void* src) { new (dst) T(*(const T*)src); };

			if constexpr (std::is_move_constructible_v<T>)
				mType.functions.moveconstructor = [](void* dst, void* src) { new (dst) T(std::move(*(T*)src)); };
		}
	};

	template<typename T, bool IsClass = std::is_class_v<T>>
	class ClassBuilder : protected ClassBuilderCommon<T>
	{
	public:
		ClassBuilder() {};

		ClassBuilder(const String& Name, const String& UUID)
		{
			this->Init(Name, UUID);
		}

		~ClassBuilder()
		{

		}

		ClassBuilder(const refl::ClassBuilder<T>&) = delete;

		ClassBuilder& SaveFunction(TypeSaveFunction SaveFunction)
		{
			this->mType.functions.saveFunction = SaveFunction;
			return *this;
		}

		ClassBuilder& LoadFunction(TypeLoadFunction LoadFunction)
		{
			this->mType.functions.loadFunction = LoadFunction;
			return *this;
		}

	private:
	};

	template<typename T>
	class ClassBuilder<T, true> : protected ClassBuilderCommon<T>
	{
	public:
		ClassBuilder() {};

		ClassBuilder(const String& Name, const String& UUID)
		{
			this->Init(Name, UUID);
		}

		~ClassBuilder()
		{
		}

		ClassBuilder(const refl::ClassBuilder<T>&) = delete;

		ClassBuilder& SaveFunction(TypeSaveFunction SaveFunction)
		{
			this->mType.functions.saveFunction = SaveFunction;
			return *this;
		}

		ClassBuilder& LoadFunction(TypeLoadFunction LoadFunction)
		{
			this->mType.functions.loadFunction = LoadFunction;
			return *this;
		}

		template<typename PT>
		ClassBuilder& Property(const String& Name, PT T::* TypeProperty)
		{
			refl::Property prop;
			prop.name = Name;
			prop.offset = static_cast<std::ptrdiff_t>(
				reinterpret_cast<std::uintptr_t>(
					&(reinterpret_cast<const volatile T*>(0)->*TypeProperty)
					)
				);

			prop.type = ReflectionRegistry::Instance()->GetOrCreateType<PT>();

			this->mType.properties.push_back(std::move(prop));
			return *this;
		}

		ClassBuilder& Attribute(const String& PropertyName, refl::Attribute Attr, AttributeVariant Value)
		{
			Vector<refl::Property>::iterator it = std::find_if(this->mType.properties.begin(), this->mType.properties.end(), [PropertyName](const refl::Property& p) { return p.name == PropertyName;});
			if (it == this->mType.properties.end())
			{
				THROW("Property with name, {} doesn't exist", PropertyName);
			}

			it->attributes[Attr] = Value;

			return *this;
		}

		ClassBuilder& Attribute(const String& PropertyName, refl::Attribute Attr)
		{
			Vector<refl::Property>::iterator it = std::find_if(this->mType.properties.begin(), this->mType.properties.end(), [PropertyName](const refl::Property& p) { return p.name == PropertyName;});
			if (it == this->mType.properties.end())
			{
				THROW("Property with name, {} doesn't exist", PropertyName);
			}

			it->attributes[Attr] = {};

			return *this;
		}

		ClassBuilder& Attribute(refl::Attribute Attr)
		{
			this->mType->attributes[Attr] = {};
			return *this;
		}

	private:

	};

	// Header reflect
#define REFL_DECLARE(TYPE) \
    namespace refl { template<> struct is_reflected<TYPE> : std::true_type {}; }

// CPP reflect
#define REFL_DEFINE(TYPE)																			\
	static void TRUNCATE(Refl_Register_Function_, __LINE__)();										\
	namespace																						\
	{																								\
		struct TRUNCATE(Refl_Register_, __LINE__)													\
		{																							\
			TRUNCATE(Refl_Register_, __LINE__)()													\
			{																						\
				TRUNCATE(Refl_Register_Function_, __LINE__)();										\
			}																						\
			~TRUNCATE(Refl_Register_, __LINE__)()													\
			{																						\
				refl::ReflectionRegistry::Instance()->MarkInvalid<TYPE>();						    \
			}																						\
		};																							\
	}																								\
	static const TRUNCATE(Refl_Register_, __LINE__) TRUNCATE(Static_Refl_Register_, __LINE__);		\
	static void TRUNCATE(Refl_Register_Function_, __LINE__)()


#define REFL_DECLARE_COMPONENT(TYPE)												\
	namespace refl { template<> struct is_component<TYPE> : std::true_type {}; }	\
	REFL_DECLARE(TYPE)

#define REFL_DECLARE_PRIMITIVE(TYPE)													\
	namespace refl { template<> struct is_primitive_type<TYPE> : std::true_type {}; }	\
	REFL_DECLARE(TYPE)
}


#pragma region Primitive Registration
REFL_DECLARE_PRIMITIVE(int)
REFL_DECLARE_PRIMITIVE(long)
REFL_DECLARE_PRIMITIVE(uint32)
REFL_DECLARE_PRIMITIVE(uint64)
REFL_DECLARE_PRIMITIVE(float)
REFL_DECLARE_PRIMITIVE(double)
REFL_DECLARE_PRIMITIVE(bool)
REFL_DECLARE_PRIMITIVE(String)
REFL_DECLARE(Vector3)
REFL_DECLARE(UniqueID128)
#pragma endregion