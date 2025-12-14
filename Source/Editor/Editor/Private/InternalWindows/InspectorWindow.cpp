#include "InspectorWindow.h"

#include <Scene/SceneManager.h>
#include <EWorld.h>

#include <Components/ETransform.h>
#include <Components/EStaticMesh.h>

InspectorWindow::InspectorWindow(Editor* EditorPtr)
    :
    EditorWindow(EditorPtr)
{
}

InspectorWindow::~InspectorWindow()
{
}

bool InspectorWindow::Init()
{
    mSceneManager = SubsystemManager::Get<SceneManager>();
    return true;
}

void InspectorWindow::Render()
{
    if (mEditor->GetSelectedItem().type != SelectedItemType::Entity)
    {
        return;
    }

    EWorld& world = mSceneManager->GetActiveScene()->GetWorld();

    EEntity entity = std::get<EEntity>(mEditor->GetSelectedItem().item);
    ETransform* transform = world.GetComponent<ETransform>(entity);
    if (transform)
    {
        ImGui::SeparatorText("Transform");

        Vector3 entityPosition = transform->position;
        if(ImGui::DragFloat3("Position", (float*)&entityPosition, mEditor->GetToolSettings().translationSpeed))
            transform->position = entityPosition;

        Vector3 eularDeg = transform->rotation;
        if (ImGui::DragFloat3("Rotation", (float*)&eularDeg, mEditor->GetToolSettings().rotationSpeed))
            transform->rotation = eularDeg;

        Vector3 entityScale = transform->scale;
        if (ImGui::DragFloat3("Scale", (float*)&entityScale, mEditor->GetToolSettings().scaleSpeed, 0.001f))
            transform->scale = entityScale;
    }

    EStaticMesh* staticMeshComponent = world.GetComponent<EStaticMesh>(entity);
    if (staticMeshComponent)
    {
        ImGui::SeparatorText("Static Mesh");

        AssetRegistry* assetReg = SubsystemManager::Get<AssetRegistry>();
        Mesh* mesh = assetReg->GetAsset(staticMeshComponent->mesh);
        if (mesh)
        {
            ImGui::DrawCopyableText("", mesh->GetName());
            ImGui::SameLine();

            static bool listOpened = false;
            if (ImGui::Button(ICON_FA_LEMON))
            {
                listOpened = true;
            }

            if (listOpened)
            {
                if (ImGui::BeginListBox("##meshesList"))
                {
                    auto meshAssets = assetReg->GetAssetsOfType<Mesh>("Mesh");
                    for (uint32 i = 0; i < meshAssets.size(); i++)
                    {
                        if (ImGui::Selectable(meshAssets[i]->GetName().c_str(), false))
                        {
                            staticMeshComponent->mesh = assetReg->GetAssetFromUUID<Mesh>(meshAssets[i]->GetUUID());
                            listOpened = false;
                        }
                    }

                    ImGui::EndListBox();
                }
            }

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
                listOpened = false;
        }
    }

}
