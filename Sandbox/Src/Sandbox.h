#pragma once
#include "PlayerController.h"
#include "../../Engine/Source/Runtime/Engine/FrameData.h"
#include "ECS/SystemManager.h"
#include "ECS/World.h"

class Sandbox
{
public:
    Sandbox() = default;
    ~Sandbox() = default;

    void Init();
    void Tick(World& world, const FrameData& frameData);

private:

};
