#pragma once
#include "Engine/Layer.h"
#include "ECS/SystemManager.h"
#include "ControllerManager.h"

class GameplayLayer : public Layer
{
public:
    GameplayLayer(World& world, SystemManager& systems) : mContext{world, mInput}, mSystems(systems) {}
    GameplayLayer(const GameplayLayer&) = delete;
    GameplayLayer& operator=(const GameplayLayer&) = delete;

    void RegisterController(OwnedPtr<IController> controller) { mControllers.RegisterController(std::move(controller)); }
    void Init() { mControllers.InitControllers(mContext); }
    void Destroy() { mInput.Clear(); mControllers.DestroyControllers(mContext); }
    void BeginFrame() override { mInput.BeginFrame(); }
    bool OnInput(const InputEvent& event) override { return mInput.OnInput(event); }
    void ResetInput() override { mInput.Reset(); }
    void Tick(f32 deltaTime) override
    {
        mContext.deltaTime = deltaTime;
        mContext.world.SwapEventBuffers();
        mControllers.TickControllers(mContext);
        mSystems.TickSystems(mContext);
    }

private:
    InputMapper mInput;
    GameContext mContext;
    SystemManager& mSystems;
    ControllerManager mControllers;
};
