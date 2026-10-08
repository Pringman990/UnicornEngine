#include "pch.h"
#include "ControllerManager.h"

ControllerManager::ControllerManager()
{
}

ControllerManager::~ControllerManager()
{

}

void ControllerManager::RegisterController(OwnedPtr<IController> controller)
{
    mControllers.push_back(std::move(controller));
}

void ControllerManager::InitControllers(GameContext& context) const
{
    for (auto& controller : mControllers)
    {
        controller->Init(context);
    }
}

void ControllerManager::TickControllers(GameContext& context) const
{
    for (auto& controller : mControllers)
    {
        controller->Tick(context);
    }
}

void ControllerManager::DestroyControllers(GameContext& context)
{
    for (auto& controller : mControllers)
        controller->Destroy(context);
    mControllers.clear();
}
