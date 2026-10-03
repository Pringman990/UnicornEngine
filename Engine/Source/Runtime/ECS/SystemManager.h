#pragma once
#include "../Engine/FrameData.h"
#include "Core/Types.h"
#include "World.h"
#include "Game/GameContext.h"

enum SystemStage : uint32
{
    SS_PreTick,
    SS_Tick,
    SS_PostTick,

    SS_Count
};

struct System
{
    const char* name;

    void(*Tick)(GameContext& gameContext);

    SystemStage stage;
};

class SystemManager
{
    INIT_SERVICE(SystemManager)
public:


    SystemManager();
    ~SystemManager();

    void RegisterSystem(System system);

    void TickSystems(GameContext& gameContext);

private:

    UnorderedMap<SystemStage, List<System>> mSystems;
};