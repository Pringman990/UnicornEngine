#include "SceneHierarchyWindow.h"
#include <Scene/SceneManager.h>

#include <Components/ENameComponent.h>
#include <Components/ETransform.h>
#include <Components/EStaticMesh.h>

SceneHierarchyWindow::SceneHierarchyWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
}

SceneHierarchyWindow::~SceneHierarchyWindow()
{
}

bool SceneHierarchyWindow::Init()
{
	return true;
}

void SceneHierarchyWindow::Render()
{
	SceneManager* sceneManager = SceneManager::Instance();

	ImGui::SeparatorText(sceneManager->GetActiveScene()->GetName().c_str());
	EWorld& world = sceneManager->GetActiveScene()->GetWorld();
	const Vector<EEntity>& entities = world.GetAllEntities();

	static bool addMenuOpened = false;
	if (ImGui::Button(ICON_FA_PLUS))
	{
		addMenuOpened = true;
	}

	if (addMenuOpened)
	{
		if (ImGui::Selectable("Cube", false))
		{
			EEntity entity = world.CreateEntity();
			world.AddComponent<ETransform>(entity);

			int32 cubeNameCount = -1;
			for (const EEntity& ent : entities)
			{
				ENameComponent* nameComp = world.GetComponent<ENameComponent>(ent);
				if (nameComp && StartsWith(nameComp->name, "Cube"))
					cubeNameCount++;
			}

			ENameComponent* nameComponent = world.AddComponent<ENameComponent>(entity);
			if(cubeNameCount == -1)
				nameComponent->name = "Cube";
			else
				nameComponent->name = String("Cube") + ToString(cubeNameCount);

			AssetRegistry* assetReg = AssetRegistry::Instance();
			//TODO: We need a better way to get a asset. (maybe just the path is enough).
			world.AddComponent<EStaticMesh>(entity)->mesh = assetReg->GetAssetFromUUID<Mesh>(UniqueID128("74897956-e7c6-467d-8142-f091ce2190ab"));
			
			addMenuOpened = false;
		}
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsItemHovered())
		addMenuOpened = false;

	ImGui::SeparatorText("Hierarchy");

	for (const EEntity& ent : entities)
	{
		DrawEntityNode(ent, world);
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
		mEditor->InvalidateSelectedItem();
}

void SceneHierarchyWindow::DrawEntityNode(const EEntity& Entity, EWorld& World)
{
	ImGuiTreeNodeFlags flags = 
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	if (Entity == mSelectedEntity)
		flags |= ImGuiTreeNodeFlags_Selected;

	//TODO: check if entity has children
	flags |= ImGuiTreeNodeFlags_Leaf;

	ENameComponent* nameComp = World.GetComponent<ENameComponent>(Entity);
	String entityName = nameComp ? nameComp->name.empty() ? "EMPTY" : nameComp->name : "INVALID";

	bool opened = ImGui::TreeNodeEx(Entity.ToString().c_str(), flags, entityName.c_str());

	if ((ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) || 
		(ImGui::IsItemClicked(ImGuiMouseButton_Right && !ImGui::IsItemToggledOpen()))
		)
		mEditor->SetSelectedItem(SelectedItemType::Entity, Entity);

	// Context menu
	if (ImGui::BeginPopupContextItem())
	{
		ImGui::Text("NOT IMPLEMENTED!!");
		/*if (ImGui::MenuItem("Duplicate"))
			scene.DuplicateEntity(e);

		if (ImGui::MenuItem("Delete"))
			scene.DestroyEntity(e);*/

		ImGui::EndPopup();
	}

	// Begin rename mode
	if (nameComp && ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(0))
	{
		mRenamingEntity = Entity;
		strcpy_s(mRenameBuffer, nameComp->name.c_str());
		ImGui::SetKeyboardFocusHere();
	}

	// Rename UI
	if (nameComp && mRenamingEntity == Entity)
	{
		if (ImGui::InputText("##rename", mRenameBuffer, 256,
			ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			nameComp->name = mRenameBuffer;
			mRenamingEntity = EEntity::Invalid();
		}

		// Cancel rename with click elsewhere
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			mRenamingEntity = EEntity::Invalid();
	}

	if (opened)
	{
		//TODO: Loop children

		ImGui::TreePop();
	}
}
