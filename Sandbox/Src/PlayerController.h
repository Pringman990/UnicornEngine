#pragma once
#include "CameraController.h"
#include "Game/GameContext.h"
#include "Game/IController.h"

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

    NODISC const auto& GetSelectedEntities() const { return mSelectedEntities; }

private:
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
};
