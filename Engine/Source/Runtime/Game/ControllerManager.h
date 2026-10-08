#pragma once
#include "GameContext.h"
#include "IController.h"

class ControllerManager
{
public:
    ControllerManager();
    ~ControllerManager();

    void RegisterController(OwnedPtr<IController> controller);

    void InitControllers(GameContext& context) const;
    void TickControllers(GameContext& context) const;
    void DestroyControllers(GameContext& context);

private:
    List<OwnedPtr<IController>> mControllers;
};
