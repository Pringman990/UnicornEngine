#pragma once
#include "Archive.h"
#include "ServiceRegistry.h"
#include "UniqueID128.h"
#include "Defines.h"
#include "Helpers.h"

namespace refl
{
    using TypeID = UniqueID128;

    template<typename T>
    using Serialize = Func<void(const T* obj, Archive& archive, const String& key)>;

    template<typename T>
    using Deserialize = Func<bool(T* obj, Archive& archive, const String& key)>;

    template<typename T>
    void Construct(void* dst)
    {
        new(dst) T();
    }

    template<typename T>
    void Destroy(void* obj)
    {
        static_cast<T*>(obj)->~T();
    }

    template<typename T>
    void Copy(void* dst, void* src)
    {
        new(dst) T(*static_cast<T*>(src));
    }

    template<typename T>
    void Move(void* dst, void* src)
    {
        new(dst) T(std::move(*static_cast<T*>(src)));
    }

    template<typename T>
    void TrivialRelocate(void* dst, void* src)
    {
        memcpy(dst, src, sizeof(T));
    }

    template<typename T>
    void MoveRelocate(void* dst, void* src)
    {
        Move<T>(dst, src);
        Destroy<T>(src);
    }

    struct TypeOps
    {
        void (*constructor)(void* obj) = nullptr;
        void (*destroy)(void* obj) = nullptr;
        void (*copy)(void* dst, void* src) = nullptr;
        void (*move)(void* dst, void* src) = nullptr;

        void (*relocate)(void* dst, void* src) = nullptr;

        Func<void(const void* obj, Archive& archive, const String& key)> serialize = nullptr;
        Func<bool(void* obj, Archive& archive, const String& key)> deserialize = nullptr;
    };

    template<typename T>
    void RegisterDefaultOperations(TypeOps& ops)
    {
        ops.constructor = &Construct<T>;
        ops.destroy = &Destroy<T>;
        ops.move = &Move<T>;

        if constexpr (std::is_copy_constructible_v<T>)
            ops.copy = &Copy<T>;
        else
            ops.copy = nullptr;

        if constexpr (std::is_trivially_copyable_v<T>)
            ops.relocate = &TrivialRelocate<T>;
        else
            ops.relocate = &MoveRelocate<T>;
    }

    struct Property
    {
        const struct Type* type = nullptr;

        String displayName;

        size_t size{};
        std::ptrdiff_t offset{};
        size_t alignment{};
    };

    struct Type
    {
        TypeID id{};

        String name;
        String displayName;
        String category;

        size_t size{};
        size_t alignment{};

        List<Property> properties;

        TypeOps ops;
    };

    template<typename T>
    Serialize<T> DefaultSerialize(const Type& type)
    {
        return [&type](const T* obj, Archive& archive, const String& key)
        {
            archive.BeginWriteObject(key);
            for (const auto& property : type.properties)
            {
                const void* propertyObj = reinterpret_cast<const byte*>(obj) + property.offset;
                property.type->ops.serialize(propertyObj, archive, property.displayName);
                archive.Next();
            }
            archive.EndWriteObject();
        };
    }

    template<typename T>
    Deserialize<T> DefaultDeserialize(const Type& type)
    {
        return [&type](T* obj, Archive& archive, const String& key) -> bool
        {
            archive.BeginReadObject(key);
            for (const auto& property : type.properties)
            {
                void* propertyObj = reinterpret_cast<byte*>(obj) + property.offset;
                property.type->ops.deserialize(propertyObj, archive, property.displayName);
            }
            archive.EndReadObject();
            return true;
        };
    }

    class ReflectionRegistry
    {
        INIT_SERVICE(ReflectionRegistry)
    public:
        ReflectionRegistry() = default;
        ~ReflectionRegistry() = default;

        static void RegisterStandardTypes();

        template<typename T>
        Type& GetOrCreate()
        {
            const TypeIndex idx = typeid(T);
            const auto it = mTypeIndexToIndex.find(idx);
            if (it != mTypeIndexToIndex.end())
                return *mTypes[it->second];

            OwnedPtr<Type> typePtr = MakeOwned<Type>();

            typePtr->displayName = String("NON REGISTERED_") + idx.name();
            typePtr->name = idx.name();
            typePtr->size = sizeof(T);
            typePtr->alignment = alignof(T);
            RegisterDefaultOperations<T>(typePtr->ops);

            auto* type = typePtr.get();
            typePtr->ops.serialize = [type](const void* obj, Archive& archive, const String& key)
            {
                DefaultSerialize<T>(*type)(static_cast<const T*>(obj), archive, key);
            };

            typePtr->ops.deserialize = [type](void* obj, Archive& archive, const String& key) -> bool
            {
                return DefaultDeserialize<T>(*type)(static_cast<T*>(obj), archive, key);
            };

            mTypes.push_back(std::move(typePtr));
            mTypeIndexToIndex.insert({typeid(T), mTypes.size() - 1});
            return *mTypes.back();
        }

        template<typename T>
        const Type& GetType()
        {
            return GetOrCreate<T>();
        }

        const Type& GetType(const TypeID& id)
        {
            const auto it = mTypeIdToIndex.find(id);
            if (it != mTypeIdToIndex.end())
                return *mTypes[it->second];

            FATAL("Type must be registered before GetType with ID can be used.");
        }

        NODISC const List<OwnedPtr<Type>>& GetTypes() const { return mTypes; }

        const List<TypeID>* GetAllTypesOfCategory(const String& type)
        {
            auto it = mCategoryToTypeID.find(type);
            if (it != mCategoryToTypeID.end())
                return &mCategoryToTypeID[type];

            return nullptr;
        }

        template<typename T>
        void RegisterTypeData(const TypeID& id, const String& category)
        {
            const TypeIndex idx = typeid(T);
            const auto it = mTypeIndexToIndex.find(idx);
            if (it == mTypeIndexToIndex.end())
                FATAL("Trying to register TypeID for a type that has not been created");

            mTypeIdToIndex.insert({id, it->second});

            if (!category.empty())
                mCategoryToTypeID[category].push_back(id);
        }

    private:
        List<OwnedPtr<Type>> mTypes;
        UnorderedMap<TypeIndex, uint32> mTypeIndexToIndex;
        UnorderedMap<TypeID, uint32> mTypeIdToIndex;
        UnorderedMap<String, List<TypeID>> mCategoryToTypeID;
    };

    template<typename T>
    class ClassRegistrator
    {
    public:
        ClassRegistrator(const String& displayName, const TypeID& id)
        {
            mType = &Services::Get<ReflectionRegistry>().GetOrCreate<T>();
            mType->id = id;
            mType->displayName = displayName;
        }

        template<typename Owner, typename PT>
        ClassRegistrator& Property(const String& displayName, PT Owner::* typeProperty)
        {
            refl::Property prop;
            prop.type = &Services::Get<ReflectionRegistry>().GetOrCreate<PT>();
            prop.displayName = displayName;
            prop.size = sizeof(PT);
            prop.alignment = alignof(PT);
            prop.offset = OffsetOf(typeProperty);

            mType->properties.push_back(std::move(prop));
            return *this;
        }

        ClassRegistrator& Category(const String& category)
        {
            mType->category = category;
            return *this;
        }

        ClassRegistrator& Serialize(const Serialize<T>& func)
        {
            mType->ops.serialize = [func](const void* obj, Archive& archive, const String& key)
            {
                func(static_cast<const T*>(obj), archive, key);
            };
            return *this;
        }

        ClassRegistrator& Deserialize(const Deserialize<T>& func)
        {
            mType->ops.deserialize = [func](void* obj, Archive& archive, const String& key) -> bool
            {
                return func(static_cast<T*>(obj), archive, key);
            };
            return *this;
        }

        void Finish() const
        {
            auto& reg = Services::Get<ReflectionRegistry>();
            reg.RegisterTypeData<T>(mType->id, mType->category);
        }

    private:
        Type* mType;
    };

    inline ReflectionRegistry& GetRegistry()
    {
        return Services::Get<ReflectionRegistry>();
    }

    inline ReflectionRegistry* TryGetRegistry()
    {
        return Services::TryGet<ReflectionRegistry>();
    }
}
