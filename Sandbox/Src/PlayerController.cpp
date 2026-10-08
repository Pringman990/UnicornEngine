#include "pch.h"
#include "PlayerController.h"

#include "AIComponents.h"
#include "Selectable.h"
#include "Core/Logging/Logs.h"
#include "Game/EngineComponents.h"
#include "Engine/RayIntersect.h"
#include "GameActions.h"
#include "UI/UI.h"

#include <algorithm>

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

    auto& input = context.input;
    input.BindKey(GameActions::CameraMove, Key::W, {0.0f, 1.0f});
    input.BindKey(GameActions::CameraMove, Key::S, {0.0f, -1.0f});
    input.BindKey(GameActions::CameraMove, Key::A, {-1.0f, 0.0f});
    input.BindKey(GameActions::CameraMove, Key::D, {1.0f, 0.0f});
    input.BindScroll(GameActions::CameraZoom);
    input.BindPointer(GameActions::Pointer);
    input.BindButton(GameActions::Select, MouseButton::Left);
    input.BindButton(GameActions::Command, MouseButton::Right);
    input.BindKey(GameActions::Stop, Key::Space);

    input.OnAction(GameActions::Select, [this, &context](const ActionEvent& event)
    {
        auto* selection = context.world.GetComponent<Selection>(mPlayerEntity);
        if (event.phase == ActionPhase::Pressed)
        {
            glm::vec3 position;
            if (GetMouseWorldPosition(context.world, event.position.x, event.position.y, position))
            {
                mSelectionStart = position;
                mIsSelecting = true;
                selection->active = true;
                selection->start = position;
                UpdateSelection(context.world, event.position);
            }
        }
        else if (event.phase == ActionPhase::Released || event.phase == ActionPhase::Canceled)
        {
            if (mIsSelecting && event.phase == ActionPhase::Released)
                UpdateSelection(context.world, event.position);
            mIsSelecting = false;
            selection->active = false;
        }
        return true;
    });
    input.OnAction(GameActions::Pointer, [this, &context](const ActionEvent& event)
    {
        if (mIsSelecting)
            UpdateSelection(context.world, event.position);
        return mIsSelecting;
    });
    input.OnAction(GameActions::Command, [this, &context](const ActionEvent& event)
    {
        if (event.phase == ActionPhase::Pressed)
            IssueCommand(context.world, event.position);
        return true;
    });
    input.OnAction(GameActions::Stop, [this, &context](const ActionEvent& event)
    {
        if (event.phase == ActionPhase::Pressed)
            StopSelectedUnits(context.world);
        return true;
    });

    auto& ui = Services::Get<UI>();
    mStopButton = ui.CreateButton({16.0f, -60.0f, 44.0f, 44.0f}, {0.2f, 0.2f, 0.2f, 1.0f},
        [this, &context] { StopSelectedUnits(context.world); }, UIAnchor::BottomLeft);
    mStopIcon = ui.CreateRectangle({28.0f, -48.0f, 20.0f, 20.0f}, glm::vec4(1.0f), UIAnchor::BottomLeft);
}

void PlayerController::Tick(GameContext& context)
{
    mCameraController.Tick(context);
    std::erase_if(mSelectedEntities, [&](Entity entity)
    {
        return context.world.GetComponent<Selectable>(entity) == nullptr;
    });
}

void PlayerController::Destroy(GameContext& context)
{
    auto& ui = Services::Get<UI>();
    ui.Remove(mStopButton);
    ui.Remove(mStopIcon);
}

void PlayerController::StopSelectedUnits(World& world)
{
    for (const auto& [entity, selectable, attack, target, movement, animator] :
         world.Query<Selectable, Attack, MoveTarget, Movement, Animator>())
    {
        if (!selectable.selected) continue;
        attack.target = InvalidEntity;
        target.active = false;
        movement.velocity = glm::vec3(0.0f);
        animator.loop = true;
    }
}

void PlayerController::UpdateSelection(World& world, glm::vec2 mouse)
{
    if (!GetMouseWorldPosition(world, mouse.x, mouse.y, mSelectionEnd)) return;
    world.GetComponent<Selection>(mPlayerEntity)->end = mSelectionEnd;
    for (Entity entity : mSelectedEntities)
        if (auto* selectable = world.GetComponent<Selectable>(entity))
            selectable->selected = false;
    mSelectedEntities.clear();

    const glm::vec3 min = glm::min(mSelectionStart, mSelectionEnd);
    const glm::vec3 max = glm::max(mSelectionStart, mSelectionEnd);
    for (const auto& [entity, transform, selectable, team] : world.Query<Transform, Selectable, Team>())
    {
        if (team.id != Team::Player) continue;
        if (transform.position.x < min.x || transform.position.x > max.x ||
            transform.position.z < min.z || transform.position.z > max.z) continue;
        selectable.selected = true;
        mSelectedEntities.push_back(entity);
    }
}

void PlayerController::IssueCommand(World& world, glm::vec2 mouse)
{
    const ray::Ray mouseRay = GetMouseRay(world, mouse.x, mouse.y);
    glm::vec3 worldPosition;

    if (ray::RayPlaneIntersect(mouseRay, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), worldPosition))
    {
        List<Entity> units;
        f32 radius = 0.0f;
        f32 stoppingDistance = 0.0f;
        for (const Entity entity : mSelectedEntities)
        {
            const auto* movement = world.GetComponent<Movement>(entity);
            const auto* target = world.GetComponent<MoveTarget>(entity);
            if (!movement || !target)
                continue;

            units.push_back(entity);
            radius = std::max(radius, movement->radius);
            stoppingDistance = std::max(stoppingDistance, target->stoppingDistance);
        }

        if (units.empty())
            return;

        const Entity enemy = GetMouseEnemy(world, mouseRay);
        if (enemy != InvalidEntity)
        {
            for (const Entity unit : units)
            {
                auto* attack = world.GetComponent<Attack>(unit);
                if (!attack)
                    continue;

                attack->target = enemy;
                auto* target = world.GetComponent<MoveTarget>(unit);
                target->position = world.GetComponent<Transform>(enemy)->position;
                target->active = true;
                target->closestDistance = std::numeric_limits<f32>::max();
                target->timeWithoutProgress = 0.0f;
            }
            return;
        }

        const uint32 count = static_cast<uint32>(units.size());
        const uint32 columns = static_cast<uint32>(std::ceil(std::sqrt(static_cast<f32>(count))));
        const uint32 rows = (count + columns - 1) / columns;
        // Leave room to pass between occupied slots, including arrival tolerance.
        const f32 spacing = 4.0f * radius + 2.0f * stoppingDistance + 0.1f;
        for (uint32 i = 0; i < count; ++i)
        {
            const uint32 row = i / columns;
            const uint32 rowSize = std::min(columns, count - row * columns);
            const glm::vec3 offset(
                (static_cast<f32>(i % columns) - 0.5f * (rowSize - 1)) * spacing,
                0.0f,
                (static_cast<f32>(row) - 0.5f * (rows - 1)) * spacing);
            auto* target = world.GetComponent<MoveTarget>(units[i]);
            target->position = worldPosition + offset;
            target->active = true;
            target->closestDistance = std::numeric_limits<f32>::max();
            target->timeWithoutProgress = 0.0f;

            // A ground order cancels both the attack target and its current swing.
            if (auto* attack = world.GetComponent<Attack>(units[i]))
                attack->target = InvalidEntity;
            if (auto* animator = world.GetComponent<Animator>(units[i]))
                animator->loop = true;
        }
    }
}

bool PlayerController::GetMouseWorldPosition(
    World& world,
    float mouseX,
    float mouseY,
    glm::vec3& outPosition)
{
    return ray::RayPlaneIntersect(
        GetMouseRay(world, mouseX, mouseY),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        outPosition);
}

ray::Ray PlayerController::GetMouseRay(World& world, f32 mouseX, f32 mouseY)
{
    auto info = Services::Get<Application>().GetInfo();

    auto& camera = *world.GetComponent<Camera>(mCameraController.GetCameraEntity());

    auto& cameraTransform = *world.GetComponent<Transform>(mCameraController.GetCameraEntity());

    glm::mat4 worldMatrix = cameraTransform.world;
    glm::mat4 view = glm::inverse(worldMatrix);

    glm::mat4 proj = glm::perspective(
        camera.fov,
        static_cast<f32>(info.viewportWidth) / std::max(1, info.viewportHeight),
        camera.nearPlane,
        camera.farPlane);

    return ray::ScreenPointToRay(
        mouseX,
        mouseY,
        info.viewportWidth,
        info.viewportHeight,
        view,
        proj);
}

Entity PlayerController::GetMouseEnemy(World& world, const ray::Ray& mouseRay)
{
    Entity enemy = InvalidEntity;
    f32 nearestHit = std::numeric_limits<f32>::max();
    for (const auto& [entity, transform, team, health] : world.Query<Transform, Team, Health>())
    {
        if (team.id != Team::Enemy || health.current <= 0.0f)
            continue;

        // Pick a one-metre sphere around the body, rather than just the point at its feet.
        const glm::vec3 offset = mouseRay.origin - (transform.position + glm::vec3(0.0f, 1.0f, 0.0f));
        const f32 b = glm::dot(offset, mouseRay.direction);
        const f32 discriminant = b * b - (glm::dot(offset, offset) - 1.0f);
        if (discriminant < 0.0f)
            continue;

        const f32 hit = -b - std::sqrt(discriminant);
        if (hit >= 0.0f && hit < nearestHit)
        {
            nearestHit = hit;
            enemy = entity;
        }
    }
    return enemy;
}
