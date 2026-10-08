#pragma once
#include "PlayerController.h"
#include "../../Engine/Source/Runtime/Engine/FrameData.h"
#include "ECS/SystemManager.h"
#include "ECS/World.h"
#include "Game/EngineComponents.h"

class Sandbox
{
public:
    Sandbox() = default;
    ~Sandbox() = default;

    void Init();
    void Tick(World& world, const FrameData& frameData);

private:
    // Scene-owned resources outlive the components which borrow them.
    Skeleton mShamanSkeleton;
    AnimationClip mShamanIdle;
    AnimationClip mShamanRun;
    AnimationClip mShamanAttack;
    SkinnedMesh mShamanMesh;
};
