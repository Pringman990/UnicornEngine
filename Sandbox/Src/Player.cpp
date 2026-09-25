#include "pch.h"
#include "Player.h"

#include "Core/Logging/Logs.h"
#include "Engine/EngineComponents.h"
#include "Engine/RayIntersect.h"
#include "Engine/SimpleInput.h"

void Player::Init(World& world)
{
    auto rig = world.CreateEntity("CameraRig");
    world.AddComponent<Transform>(rig);

    auto entity = world.CreateEntity("Camera");
    auto trans = world.AddComponent<Transform>(entity);
    trans->position = glm::vec3(0.f, 20.f, 20.f);

    glm::vec3 direction = glm::normalize(glm::vec3(0.f) - trans->position);
    trans->rotation = glm::quatLookAt(direction, glm::vec3(0.f, 1.f, 0.f));

    world.AddComponent<Camera>(entity);

    auto relationship = world.AddComponent<Relationship>(entity);
    relationship->parent = rig;

    mCameraController.Init(world, rig, entity);
}

void Player::Tick(World& world, const FrameData& frameData)
{
    mCameraController.Tick(world, frameData);

    if (simpleInput::LeftMouseButtonPressed)
    {
        f32 x, y;
        SDL_GetMouseState(&x, &y);

        auto info = Services::Get<Application>().GetInfo();

        auto& camera = *world.GetComponent<Camera>(mCameraController.GetCameraEntity());
        auto& cameraTransform = *world.GetComponent<Transform>(mCameraController.GetCameraEntity());

        glm::mat4 worldMatrix = cameraTransform.world;
        glm::mat4 view = glm::inverse(worldMatrix);
        glm::mat4 proj = glm::perspective(camera.fov, camera.aspect, camera.nearPlane, camera.farPlane);

        ray::Ray ray = ray::ScreenPointToRay(x, y, info.viewportWidth, info.viewportHeight, view, proj);

        glm::vec3 worldHit{};
        if (ray::RayPlaneIntersect(ray, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), worldHit))
        {
            LOG_INFO("World Hit, x:{}, y:{}, z:{}", worldHit.x, worldHit.y, worldHit.z);

            auto& renderer = Services::Get<Renderer>();

            RenderScene scene{};

            renderer.DrawDebugLine(scene, cameraTransform.position, worldHit, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

            RenderView renderView{};
            renderView.viewport = {0, 0, info.viewportWidth, info.viewportHeight};
            renderView.projectionView = proj * view;
            renderer.Render(scene, renderView);
        }
    }
}
