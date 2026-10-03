#include "pch.h"
#include "PlayerController.h"

#include "AIComponents.h"
#include "Selectable.h"
#include "Core/Logging/Logs.h"
#include "Game/EngineComponents.h"
#include "Engine/RayIntersect.h"
#include "Engine/SimpleInput.h"

void PlayerController::Init(GameContext& context)
{
    refl::ClassRegistrator<Selection>("Selection", refl::TypeID("050a04a7-f199-4976-a491-955d11d50415"))
            .Category("Component")
            .Property("Active", &Selection::active)
            .Property("Start", &Selection::start)
            .Property("End", &Selection::end)
            .Finish();

    mPlayerEntity = context.world.CreateEntity("Player");
    context.world.AddComponent<Selection>(mPlayerEntity);

    auto rig = context.world.CreateEntity("CameraRig");
    context.world.AddComponent<Transform>(rig);

    auto entity = context.world.CreateEntity("Camera");
    auto trans = context.world.AddComponent<Transform>(entity);
    trans->position = glm::vec3(0.f, 20.f, 20.f);

    glm::vec3 direction = glm::normalize(glm::vec3(0.f) - trans->position);
    trans->rotation = glm::quatLookAt(direction, glm::vec3(0.f, 1.f, 0.f));

    context.world.AddComponent<Camera>(entity);

    auto relationship = context.world.GetComponent<Relationship>(entity);
    relationship->parent = rig;

    mCameraController.Init(context.world, rig, entity);
}

void PlayerController::Tick(GameContext& context)
{
    mCameraController.Tick(context);

    auto selection = context.world.GetComponent<Selection>(mPlayerEntity);

    if (simpleInput::LeftMouseButtonPressed)
    {
        f32 x, y;
        SDL_GetMouseState(&x, &y);

        glm::vec3 worldPosition;

        if (GetMouseWorldPosition(context.world, x, y, worldPosition))
        {
            mSelectionStart = worldPosition;
            mSelectionEnd = worldPosition;
            mIsSelecting = true;
            selection->active = true;
            selection->start = worldPosition;
        }
    }

    if (mIsSelecting && simpleInput::LeftMouseButtonDown)
    {
        f32 x, y;
        SDL_GetMouseState(&x, &y);

        glm::vec3 worldPosition;

        if (GetMouseWorldPosition(context.world, x, y, worldPosition))
        {
            mSelectionEnd = worldPosition;
            selection->end = worldPosition;
        }
    }

    if (mIsSelecting && simpleInput::LeftMouseButtonReleased)
    {
        selection->active = false;
        mIsSelecting = false;
    }

    glm::vec3 min = glm::min(mSelectionStart, mSelectionEnd);
    glm::vec3 max = glm::max(mSelectionStart, mSelectionEnd);

    if (mIsSelecting)
    {
        for (auto& entity : mSelectedEntities)
        {
            context.world.GetComponent<Selectable>(entity)->selected = false;
        }
        mSelectedEntities.clear();
    }

    for (const auto& [entity, transform, selectable]
        : context.world.Query<Transform, Selectable>())
    {
        if (!mIsSelecting)
            break;

        if (transform.position.x < min.x ||
            transform.position.x > max.x ||
            transform.position.z < min.z ||
            transform.position.z > max.z
        )
            continue;

        mSelectedEntities.push_back(entity);
        selectable.selected = true;
    }

    if (simpleInput::RightMouseButtonPressed)
    {
        f32 x, y;
        SDL_GetMouseState(&x, &y);

        glm::vec3 worldPosition;

        if (GetMouseWorldPosition(context.world, x, y, worldPosition))
        {
            for (auto& entity : mSelectedEntities)
            {
                auto target = context.world.GetComponent<MoveTarget>(entity);
                if (!target)
                    continue;

                target->position = worldPosition;
                target->active = true;
            }
        }
    }
}

bool PlayerController::GetMouseWorldPosition(
    World& world,
    float mouseX,
    float mouseY,
    glm::vec3& outPosition)
{
    auto info = Services::Get<Application>().GetInfo();

    auto& camera = *world.GetComponent<Camera>(mCameraController.GetCameraEntity());

    auto& cameraTransform = *world.GetComponent<Transform>(mCameraController.GetCameraEntity());

    glm::mat4 worldMatrix = cameraTransform.world;
    glm::mat4 view = glm::inverse(worldMatrix);

    glm::mat4 proj = glm::perspective(
        camera.fov,
        camera.aspect,
        camera.nearPlane,
        camera.farPlane);

    ray::Ray ray = ray::ScreenPointToRay(
        mouseX,
        mouseY,
        info.viewportWidth,
        info.viewportHeight,
        view,
        proj);

    return ray::RayPlaneIntersect(
        ray,
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        outPosition);
}
