#pragma once
#include "World.h"

using WorldId = uint32;

class WorldManager
{
    INIT_SERVICE(WorldManager)
public:

    WorldManager();
    ~WorldManager();

    void ClearWorlds();

    NODISC WorldId CreateWorld();
    World& GetWorld(WorldId id);

    World& GetActiveWorld();
    void SetActiveWorld(WorldId id);

private:
    List<World> mWorlds;
    WorldId mActiveWorld;
};
