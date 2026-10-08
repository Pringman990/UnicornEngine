#pragma once
#include "GameContext.h"

class IController
{
public:
    virtual ~IController() = default;
    virtual void Init(GameContext& context) = 0;
    virtual void Tick(GameContext& context) = 0;
    virtual void Destroy(GameContext& context) {}

};
