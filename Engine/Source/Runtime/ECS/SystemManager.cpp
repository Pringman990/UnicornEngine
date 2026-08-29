//
// Created on 2026-07-11.
//

#include "../pch.h"
#include "SystemManager.h"

SystemManager::SystemManager()
{
}

SystemManager::~SystemManager()
{
}

void SystemManager::RegisterSystem(System&& system)
{
    mSystems[system.stage].push_back(std::move(system));
}

void SystemManager::TickSystems(World& world, const FrameData& frameData)
{
    for (uint32 stage = 0; stage < SS_Count; stage++)
    {
        for (const auto& system : mSystems[static_cast<SystemStage>(stage)])
        {
            system.Tick(world, frameData);
        }
    }
}
