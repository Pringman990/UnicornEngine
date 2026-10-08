#pragma once
#include "CameraController.h"
#include "Game/GameContext.h"
#include "Game/IController.h"
#include "Engine/RayIntersect.h"

struct Selection
{
    bool active = false;

    glm::vec3 start{};
    glm::vec3 end{};
};

class PlayerController : public IController
{
public:
    void Init(GameContext& context) override;

    void Tick(GameContext& context) override;
    void Destroy(GameContext& context) override;

    NODISC const auto& GetSelectedEntities() const { return mSelectedEntities; }

private:
    void UpdateSelection(World& world, glm::vec2 mouse);
    void IssueCommand(World& world, glm::vec2 mouse);
    void StopSelectedUnits(World& world);
    ray::Ray GetMouseRay(World& world, f32 mouseX, f32 mouseY);
    Entity GetMouseEnemy(World& world, const ray::Ray& mouseRay);

    bool GetMouseWorldPosition(
    World& world,
    float mouseX,
    float mouseY,
    glm::vec3& outPosition);
private:
    Entity mPlayerEntity = InvalidEntity;

    CameraController mCameraController{};

    bool mIsSelecting = false;
    glm::vec3 mSelectionStart;
    glm::vec3 mSelectionEnd;

    List<Entity> mSelectedEntities;
    uint32 mStopButton = 0;
    uint32 mStopIcon = 0;
};
