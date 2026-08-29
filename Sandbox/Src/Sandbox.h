#pragma once
#include "FrameData.h"
#include "ECS/SystemManager.h"
#include "ECS/World.h"

class Sandbox
{
public:
    Sandbox() = default;
    ~Sandbox() = default;

    void Init();

    void Tick(SystemManager& systemManager, const FrameData& frameData, World& world);

private:
};
