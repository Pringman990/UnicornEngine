#pragma once
#include "ECS/World.h"

class SceneAsset
{
public:
    SceneAsset();
    ~SceneAsset();

    static void Serialize(const SceneAsset* data, Archive& archive, const String& key);
    static bool Deserialize(SceneAsset* data, Archive& archive, const String& key);

    void SetWorld(World* world);

private:
    World* mWorld;
};
