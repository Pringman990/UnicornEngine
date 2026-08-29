#pragma once
#include "ServiceRegistry.h"
#include "UniqueID128.h"
#include "Defines.h"
#include "Helpers.h"

namespace Refl
{
    using TypeID = UniqueID128;

    template <typename T>
    void Construct(void* dst)
    {
        new(dst) T();
    }

    template <typename T>
    void Destroy(void* obj)
    {
        static_cast<T*>(obj)->~T();
    }

    template <typename T>
    void Copy(void* dst, void* src)
    {
        new(dst) T(*static_cast<T*>(src));
    }

    template <typename T>
    void Move(void* dst, void* src)
    {
        new(dst) T(std::move(*static_cast<T*>(src)));
    }

    template <typename T>
    void TrivialRelocate(void* dst, void* src)
    {
        memcpy(dst, src, sizeof(T));
    }

    template <typename T>
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
    };

    template <typename T>
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

        String name;

        size_t size{};
        std::ptrdiff_t offset{};
        size_t alignment{};
    };

    struct Type
    {
        TypeID id{};

        String name;
        String displayName;

        size_t size{};
        size_t alignment{};

        List<Property> properties;

        TypeOps ops;
    };

    class ReflectionRegistry
    {
        INIT_SERVICE(ReflectionRegistry)
    public:
        ReflectionRegistry() = default;
        ~ReflectionRegistry() = default;

        template <typename T>
        void RegisterType(Type& type)
        {
            auto it = mIDToTypeIndex.find(T::ID);

            OwnedPtr<Type> typePtr = MakeOwned<Type>(std::move(type));
            for (auto& prop : typePtr->properties)
            {
                prop.type = typePtr.get();
            }

            mTypes.push_back(std::move(typePtr));
            mIDToTypeIndex.insert({T::ID, mTypes.size() - 1});
        }

        template <typename T>
        const Type& GetType()
        {
            auto it = mIDToTypeIndex.find(T::ID);
            ASSERT(it != mIDToTypeIndex.end(), "Type must be registered before access");

            return *mTypes[it->second];
        }

        const Type& GetType(Refl::TypeID id)
        {
            auto it = mIDToTypeIndex.find(id);
            ASSERT(it != mIDToTypeIndex.end(), "Type must be registered before access");

            return *mTypes[it->second];
        }

        const List<OwnedPtr<Type>>& GetTypes() const { return mTypes; }

    private:
        List<OwnedPtr<Type>> mTypes;
        UnorderedMap<TypeID, uint32> mIDToTypeIndex;
    };

    template <typename T>
    class ClassBuilder
    {
    public:
        ClassBuilder(const String& displayName)
        {
            mType.id = T::ID;
            mType.displayName = displayName;
            mType.name = typeid(T).name();
            mType.size = sizeof(T);
            mType.alignment = alignof(T);
            RegisterDefaultOperations<T>(mType.ops);
        }

        template <typename PT>
        ClassBuilder& Property(const String& name, PT T::* typeProperty)
        {
            Refl::Property prop;
            prop.name = name;
            prop.size = sizeof(PT);
            prop.alignment = alignof(PT);
            prop.offset = OffsetOf(typeProperty);

            mType.properties.push_back(std::move(prop));
            return *this;
        }

        Type& Build()
        {
            return mType;
        }

    private:
        Type mType;
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

#define REFL_ID(UUID) \
    constexpr static Refl::TypeID ID = UniqueID128(UUID);
