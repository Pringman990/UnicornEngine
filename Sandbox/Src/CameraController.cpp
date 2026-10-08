#include "pch.h"
#include "CameraController.h"

#include "Core/Logging/Logs.h"
#include "Game/EngineComponents.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "GameActions.h"
#include "glm/gtx/quaternion.hpp"

void CameraController::Init(World& world, const Entity cameraRig, const Entity camera)
{
    mCameraRig = cameraRig;
    mCamera = camera;
}

void CameraController::Tick(GameContext& context) const
{
    auto rigTransform = context.world.GetComponent<Transform>(mCameraRig);
    auto cameraTransform = context.world.GetComponent<Transform>(mCamera);

    glm::vec2 input = context.input.Value(GameActions::CameraMove);
    const glm::vec2 mouse = context.input.MousePosition();
    const glm::vec2 viewport = {Services::Get<Application>().GetInfo().viewportWidth,
                                Services::Get<Application>().GetInfo().viewportHeight};
    if (context.input.PointerAvailable())
    {
        if (mouse.x <= mEdgeSize) input.x -= 1.0f;
        if (mouse.x >= viewport.x - mEdgeSize) input.x += 1.0f;
        if (mouse.y <= mEdgeSize) input.y += 1.0f;
        if (mouse.y >= viewport.y - mEdgeSize) input.y -= 1.0f;
    }

    if (glm::length2(input) > 0.0f)
        input = glm::normalize(input);

    rigTransform->position.x += input.x * mMoveSpeed * context.deltaTime;
    rigTransform->position.z -= input.y * mMoveSpeed * context.deltaTime;

    rigTransform->position.x =
        glm::clamp(
            rigTransform->position.x,
            mMinX,
            mMaxX);

    rigTransform->position.z =
        glm::clamp(
            rigTransform->position.z,
            mMinZ,
            mMaxZ);

    const f32 scroll = context.input.Value(GameActions::CameraZoom).y;

    if (scroll != 0.0f)
    {
        f32 zoom = cameraTransform->position.y;

        zoom -= scroll * mZoomSpeed;

        zoom = glm::clamp(
            zoom,
            mMinZoom,
            mMaxZoom);

        cameraTransform->position = glm::vec3(0.0f, zoom, zoom);
    }
}
