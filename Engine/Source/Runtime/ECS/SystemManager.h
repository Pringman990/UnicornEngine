#pragma once
#include "FrameData.h"
#include "Core/Types.h"
#include "World.h"

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

    void(*Tick)(World&, FrameData const&);

    SystemStage stage;
};

class SystemManager
{
    INIT_SERVICE(SystemManager)
public:


    SystemManager();
    ~SystemManager();

    void RegisterSystem(System&& system);

    void TickSystems(World& world, const FrameData& frameData);

private:

    UnorderedMap<SystemStage, List<System>> mSystems;
};