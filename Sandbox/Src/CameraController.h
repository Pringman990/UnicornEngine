#pragma once
#include "ECS/World.h"
#include "Engine/FrameData.h"

class CameraController
{
public:
    void Init(World& world, Entity cameraRig, Entity camera);

    void Tick(World& world, const FrameData& frameData) const;

    Entity GetCameraEntity() const {return mCamera;};
private:
    Entity mCameraRig{};
    Entity mCamera{};

    f32 mMoveSpeed = 20.0f;
    f32 mZoomSpeed = 5.0f;

    f32 mMinZoom = 5.0f;
    f32 mMaxZoom = 50.0f;

    f32 mEdgeSize = 20.0f;

    f32 mMinX = -100.0f;
    f32 mMaxX = 100.0f;
    f32 mMinZ = -100.0f;
    f32 mMaxZ = 100.0f;
};
