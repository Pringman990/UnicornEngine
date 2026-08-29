#pragma once
#include "ComponentStorage.h"
#include "EntityManager.h"
#include "Core/UniqueID128.h"

struct Relationship
{
    REFL_ID("f2a2f98a-d618-4fb1-9d50-70f6c3c10f33")

    Entity parent = 0;
};

struct EventStorage
{
    void* data = nullptr;
    size_t size = 0;
    size_t count = 0;
};

struct PrefabComponent
{
    Refl::TypeID type;
    void* data;
};

struct PrefabEntity
{
    List<PrefabComponent> components;
    int32 parent = -1;
};

struct Prefab
{
    List<PrefabEntity> entities;
};

class PrefabBuilder
{
public:
    uint32 CreateRoot()
    {
        PrefabEntity entity;
        entity.parent = -1;
        mPrefab.entities.push_back(std::move(entity));
        return 0;
    }

    uint32 CreateEntity(uint32 parent)
    {
        uint32 nextIndex = mPrefab.entities.size();
        PrefabEntity entity;
        entity.parent = parent;
        mPrefab.entities.push_back(std::move(entity));
        return nextIndex;
    }

    template <typename T>
    void Add(uint32 entity, T component)
    {
        auto& reflReg = Refl::GetRegistry();
        const auto& type = reflReg.GetType<T>();

        PrefabComponent comp;
        comp.type = type.id;

        void* memory = ::operator new(type.size, static_cast<std::align_val_t>(type.alignment));
        type.ops.relocate(memory, &component);
        comp.data = memory;

        mPrefab.entities[entity].components.push_back(std::move(comp));
    }

    Prefab Build()
    {
        return mPrefab;
    }

private:
    Prefab mPrefab;
};

class World;

template <typename... Components>
class Query
{
public:
    explicit Query(World& world);

    class Iterator
    {
    public:
        Iterator(World& world, size_t index);

        auto operator*() const;

        Iterator& operator++();

        bool operator!=(const Iterator& other) const;

    private:
        void SkipInvalid();

    private:
        World& mWorld;
        size_t mIndex = 0;
    };

    Iterator begin();

    Iterator end();

private:
    World& mWorld;
};

class World
{
public:
    World() = default;
    ~World() = default;

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    World(World&&) = default;
    World& operator=(World&&) noexcept = default;

    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    template <typename T>
    T* AddComponent(Entity entity)
    {
        auto& reflReg = Refl::GetRegistry();

        const auto& type = reflReg.GetType<T>();
        void* component = AddComponent(entity, type.id);

        return static_cast<T*>(component);
    }

    void* AddComponent(Entity entity, Refl::TypeID typeId)
    {
        auto& reflReg = Refl::GetRegistry();

        const auto& type = reflReg.GetType(typeId);
        auto it = mStorages.find(typeId);
        if (it == mStorages.end())
        {
            auto storage = MakeOwned<ComponentStorage>();
            storage->allocator = MakeOwned<ObjectAllocator>(type, 100);
            mStorages.emplace(typeId, std::move(storage));
        }

        auto& storage = mStorages[typeId];

        size_t index = 0;
        if (!storage->freeIndexes.empty())
        {
            index = storage->freeIndexes.back();
            storage->freeIndexes.pop_back();
        }
        else
        {
            index = storage->entityToIndex.size();
        }

        if (storage->entityToIndex.size() == storage->allocator->GetCount())
        {
            auto newStorage = MakeOwned<ComponentStorage>();
            newStorage->allocator = MakeOwned<ObjectAllocator>(type, storage->allocator->GetCount() * 2);
            newStorage->entityToIndex = storage->entityToIndex;

            for (uint32 i = 0; i < storage->allocator->GetCount(); ++i)
            {
                void* component = storage->allocator->Get(i);
                void* newComponent = newStorage->allocator->Get(i);
                type.ops.relocate(newComponent, component);
            }

            mStorages[typeId] = std::move(newStorage);
        }

        storage->entityToIndex[entity] = index;

        void* memory = storage->allocator->Get(index);
        type.ops.constructor(memory);
        storage->allocator->SetAlive(index, true);

        return storage->allocator->Get(index);
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        auto& reflReg = Refl::GetRegistry();

        const auto& type = reflReg.GetType<T>();
        auto it = mStorages.find(reflReg.GetType<T>().id);
        if (it == mStorages.end())
            return;

        auto& storage = mStorages[type.id];
        if (!storage->entityToIndex.contains(entity))
            return;

        uint32 index = storage->entityToIndex[entity];

        void* mem = storage->allocator->Get(index);
        type.ops.destroy(mem);
        storage->allocator->SetAlive(index, false);

        storage->freeIndexes.push_back(index);
        storage->entityToIndex.erase(entity);
    }

    template <typename T>
    T* GetComponent(Entity entity)
    {
        auto& reflReg = Refl::GetRegistry();

        const auto& type = reflReg.GetType<T>();
        auto it = mStorages.find(reflReg.GetType<T>().id);
        if (it == mStorages.end())
            return nullptr;

        auto& storage = it->second;

        auto indexIt = storage->entityToIndex.find(entity);
        if (indexIt == storage->entityToIndex.end())
            return nullptr;

        return static_cast<T*>(storage->allocator->Get(indexIt->second));
    }

    template <typename... Components>
    ::Query<Components...> Query()
    {
        static_assert(sizeof...(Components) > 0);
        return ::Query<Components...>(*this);
    }

    NODISC const List<Entity>& GetEntities() const { return mEntityManager.GetEntities(); }

    template <typename T>
    void EmitEvent(T event)
    {
        using Event = std::remove_cvref_t<T>;

        static_assert(std::is_trivially_copyable_v<Event>);
        static_assert(std::is_trivially_destructible_v<Event>);

        const size_t typeSize = sizeof(Event);
        const uint32 id = GetEventId<Event>();
        if (!mEventStorages[mCurrentWriteEventStorage].contains(id))
        {
            const size_t size = typeSize * 100;

            for (uint32 i = 0; i < 2; ++i)
            {
                OwnedPtr<EventStorage> storage;
                storage->data = ::operator new(size, static_cast<std::align_val_t>(std::alignment_of_v<Event>));
                storage->size = size;
                mEventStorages[i].insert({id, std::move(storage)});
            }
        }

        auto& storage = mEventStorages[mCurrentWriteEventStorage][id];
        if ((storage->count + 1) * typeSize > storage->size)
        {
            void* oldData = storage->data;
            void* newData = ::operator new(storage->size * 2,
                                           static_cast<std::align_val_t>(std::alignment_of_v<Event>));
            memcpy_s(newData, storage->size * 2, storage->data, storage->count * typeSize);

            ::operator delete(oldData, static_cast<std::align_val_t>(std::alignment_of_v<Event>));

            storage->data = newData;
            storage->size = storage->size * 2;
        }

        byte* data = static_cast<byte*>(storage->data);
        data += typeSize * storage->count;

        const size_t offset = typeSize * storage->count;
        const size_t remaining = storage->size - offset;
        memcpy_s(data, remaining, &event, typeSize);

        ++storage->count;
    }

    template <typename T>
    Span<const std::remove_cvref_t<T>> GetEvents()
    {
        using Event = std::remove_cvref_t<T>;
        const uint32 id = GetEventId<Event>();

        const auto it = mEventStorages[mCurrentReadEventStorage].find(id);
        if (it == mEventStorages[mCurrentReadEventStorage].end())
            return {};

        auto& storage = it->second;

        return {static_cast<const Event*>(storage->data), storage->count};
    }

    void SwapEventBuffers()
    {
        std::swap(mCurrentWriteEventStorage, mCurrentReadEventStorage);

        for (auto& [id, storage] : mEventStorages[mCurrentWriteEventStorage])
        {
            storage->count = 0;
        }
    }

    Entity Instantiate(const Prefab& prefab)
    {
        auto& reflReg = Refl::GetRegistry();

        List<Entity> entityMap(prefab.entities.size());
        Entity root = 0;
        for (uint32 i = 0; i < prefab.entities.size(); i++)
        {
            const auto& [components, parent] = prefab.entities[i];

            const Entity entity = CreateEntity();
            entityMap[i] = entity;
            if (i == 0)
            {
                root = entity;
            }

            for (auto [typeId, component] : components)
            {
                const auto& type = reflReg.GetType(typeId);
                void* compData = AddComponent(entity, typeId);
                type.ops.copy(compData, component);
            }
        }

        for (uint32 i = 0; i < prefab.entities.size(); i++)
        {
            auto [components, parent] = prefab.entities[i];
            if (parent != -1)
            {
                SetParent(entityMap[i], entityMap[parent]);
            }
        }

        return root;
    }

    void SetParent(Entity entity, Entity parent)
    {
        auto* relationship = GetComponent<Relationship>(entity);
        relationship->parent = parent;
    }

private:
    template <typename T>
    uint32 GetEventId()
    {
        static const uint32 id = AllocateEventId();
        return id;
    }

    uint32 AllocateEventId()
    {
        const uint32 id = mNextEventId;
        mNextEventId++;
        return id;
    }

private:
    EntityManager mEntityManager;
    UnorderedMap<UniqueID128, OwnedPtr<ComponentStorage>> mStorages;

    UnorderedMap<uint32, OwnedPtr<EventStorage>> mEventStorages[2];
    uint32 mCurrentWriteEventStorage = 0;
    uint32 mCurrentReadEventStorage = 1;
    uint32 mNextEventId = 0;
};

template <typename... Components>
Query<Components...>::Query(World& world)
    : mWorld(world)
{
}

template <typename... Components>
Query<Components...>::Iterator::Iterator(World& world, size_t index)
    : mWorld(world), mIndex(index)
{
    SkipInvalid();
}

template <typename... Components>
auto Query<Components...>::Iterator::operator*() const
{
    Entity entity = mWorld.GetEntities()[mIndex];

    return std::tuple<Entity, Components&...>(entity, *mWorld.GetComponent<Components>(entity)...);
}

template <typename... Components>
typename Query<Components...>::Iterator& Query<Components...>::Iterator::operator++()
{
    ++mIndex;
    SkipInvalid();
    return *this;
}

template <typename... Components>
bool Query<Components...>::Iterator::operator!=(const Iterator& other) const
{
    return mIndex != other.mIndex;
}

template <typename... Components>
void Query<Components...>::Iterator::SkipInvalid()
{
    const auto& entities = mWorld.GetEntities();

    while (mIndex < entities.size())
    {
        Entity entity = entities[mIndex];

        if ((mWorld.GetComponent<Components>(entity) && ...))
            break;

        ++mIndex;
    }
}

template <typename... Components>
typename Query<Components...>::Iterator Query<Components...>::begin()
{
    return Iterator(mWorld, 0);
}

template <typename... Components>
typename Query<Components...>::Iterator Query<Components...>::end()
{
    return Iterator(mWorld, mWorld.GetEntities().size());
}
