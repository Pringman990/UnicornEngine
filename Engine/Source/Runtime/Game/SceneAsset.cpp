#include "pch.h"
#include "SceneAsset.h"

SceneAsset::SceneAsset()
{
}

SceneAsset::~SceneAsset()
{
}

void SceneAsset::Serialize(const SceneAsset* data, Archive& archive, const String& key)
{
    auto& reflection = refl::GetRegistry();

    archive.WriteString(key, "name");

    const auto& entities = data->mWorld->GetEntities();

    archive.BeginWriteArray("entities");
    for (const auto& entity : entities)
    {
        archive.BeginWriteObject("");
        archive.WriteString(std::to_string(entity), "id");

        const auto& components = data->mWorld->GetAllComponents(entity);
        archive.BeginWriteArray("components");
        for (const auto& [componentType, componentData] : components)
        {
            archive.BeginWriteObject("");

            const auto& type = reflection.GetType(componentType);

            archive.WriteString(type.id.ToString(), "uuid");

            type.ops.serialize(componentData, archive, type.displayName);

            archive.EndWriteObject();
        }
        archive.EndWriteArray();

        archive.EndWriteObject();
    }
    archive.EndWriteArray();
}

bool SceneAsset::Deserialize(SceneAsset* data, Archive& archive, const String& key)
{
    ASSERT(data->mWorld != nullptr, "World must exist before deserializing a scene asset");

    auto& reflection = refl::GetRegistry();

    archive.BeginReadArray("entities");
    uint32 entitiesCount = archive.GetArraySize();
    for (uint32 e = 0; e < entitiesCount; ++e)
    {
        archive.BeginReadObject("");

        Entity entity = data->mWorld->CreateEntity();

        archive.BeginReadArray("components");
        uint32 componentsCount = archive.GetArraySize();
        for (uint32 c = 0; c < componentsCount; ++c)
        {
            archive.BeginReadObject("");

            String outUUID{};
            archive.ReadString(outUUID, "uuid");

            const auto& type = reflection.GetType(refl::TypeID(outUUID));

            auto componentData = data->mWorld->AddComponent(entity, type.id);
            type.ops.deserialize(componentData, archive, type.displayName);

            archive.EndReadObject();
        }

        archive.Next();
            archive.EndReadObject();
    }
    archive.EndReadArray();

    return true;
}

void SceneAsset::SetWorld(World* world)
{
    mWorld = world;
}
