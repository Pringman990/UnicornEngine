#pragma once
#include "CameraController.h"

class Player
{
public:
    void Init(World& world);

    void Tick(World& world, const FrameData& frameData);

private:
    CameraController mCameraController{};
};
