//
// Created on 2026-07-12.
//
#include "../pch.h"
#include "WorldManager.h"

WorldManager::WorldManager()
    :
    mActiveWorld(0)
{
}

WorldManager::~WorldManager()
{
}

void WorldManager::ClearWorlds()
{
    mWorlds.clear();
}

WorldId WorldManager::CreateWorld()
{
    mWorlds.emplace_back();
    return static_cast<WorldId>(mWorlds.size() - 1);
}

World& WorldManager::GetWorld(const WorldId id)
{
    return mWorlds[id];
}

World& WorldManager::GetActiveWorld()
{
    return mWorlds[mActiveWorld];
}

void WorldManager::SetActiveWorld(const WorldId id)
{
    mActiveWorld = id;
}
