#include "pch.h"
#include "CameraController.h"

#include "Core/Logging/Logs.h"
#include "Engine/EngineComponents.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "Engine/SimpleInput.h"
#include "glm/gtx/quaternion.hpp"

void CameraController::Init(World& world, const Entity cameraRig, const Entity camera)
{
    mCameraRig = cameraRig;
    mCamera = camera;
}

void CameraController::Tick(World& world, const FrameData& frameData) const
{
    auto rigTransform = world.GetComponent<Transform>(mCameraRig);
    auto cameraTransform = world.GetComponent<Transform>(mCamera);

    glm::vec2 input(0.0f);

    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_W])
        input.y += 1.0f;
    if (keys[SDL_SCANCODE_S])
        input.y -= 1.0f;
    if (keys[SDL_SCANCODE_A])
        input.x -= 1.0f;
    if (keys[SDL_SCANCODE_D])
        input.x += 1.0f;

    f32 x, y;
    SDL_GetMouseState(&x, &y);

    const glm::vec2 mouse = {x,y};
    const glm::vec2 viewport = {Services::Get<Application>().GetInfo().viewportWidth, Services::Get<Application>().GetInfo().viewportHeight};

    if (mouse.x <= mEdgeSize)
        input.x -= 1.0f;

    if (mouse.x >= viewport.x - mEdgeSize)
        input.x += 1.0f;

    if (mouse.y <= mEdgeSize)
        input.y += 1.0f;

    if (mouse.y >= viewport.y - mEdgeSize)
        input.y -= 1.0f;

    if (glm::length2(input) > 0.0f)
        input = glm::normalize(input);

    rigTransform->position.x += input.x * mMoveSpeed * frameData.deltaTime;
    rigTransform->position.z -= input.y * mMoveSpeed * frameData.deltaTime;

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

    //TODO: add zoom.
    const f32 scroll = simpleInput::MouseWheelDelta;

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
