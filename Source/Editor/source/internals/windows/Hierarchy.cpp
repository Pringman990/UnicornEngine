#include "EditorPch.h"
#include "Hierarchy.h"

#include <scene/SceneManager.h>

Hierarchy::Hierarchy(EditorCore& aEditor) : ToolWindow(aEditor)
{
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Init()
{
}

void Hierarchy::Draw(float /*aDeltaTime*/)
{
	if (ImGui::Button(ICON_FA_PLUS))
		ImGui::OpenPopup("Hierarchy Object List");

	if (ImGui::BeginPopup("Hierarchy Object List"))
	{
		ObjectList();
		ImGui::EndPopup();
	}

	if (ImGui::BeginChild("##Hierarchy entity list"))
	{
		std::vector<ecs::Entity> entities = Engine::GetSceneManager().GetCurrentEcsWorld().GetAllEntites();
		for (ecs::Entity i = 0; i < entities.size(); i++)
		{
			TreeNode(entities[i]);
		}
		ImGui::EndChild();
	}
}

void Hierarchy::TreeNode(ecs::Entity anEntity)
{
	bool isSelected = false;
	if (mEditor.IsEntitySelected(anEntity))
		isSelected = true;

	ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_AllowItemOverlap |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		//TODO: When parent-child hierarchy is implemented, check if entity has children
		ImGuiTreeNodeFlags_Leaf;

	if (isSelected)
		treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

	bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)anEntity, treeNodeFlags, "%s", (const char*)ICON_FA_CUBE);
	ImGui::SameLine();

	if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
	{
		if (isSelected && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			mEditor.RemoveEntityFromSelection(anEntity);
		}
		else if (!isSelected && !ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			mEditor.ClearEntitiesFromSelection();
			mEditor.AddEntityToSelection(anEntity);
		}
		else if (isSelected)
		{
			mEditor.ClearEntitiesFromSelection();
			mEditor.AddEntityToSelection(anEntity);
		}
		else if (!isSelected && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		{
			mEditor.AddEntityToSelection(anEntity);
		}
	}
	ecs::Name* name = Engine::GetSceneManager().GetCurrentEcsWorld().GetComponent<ecs::Name>(anEntity);
	ImGui::Text(name->name.c_str());

	if (nodeOpen)
		ImGui::TreePop();
}

void Hierarchy::ObjectList()
{
	if (ImGui::MenuItem("Empty Entity"))
	{
		ecs::Entity entity = Engine::GetSceneManager().GetCurrentEcsWorld().CreateEntity("Empty Entity");
		mEditor.ClearEntitiesFromSelection();
		mEditor.AddEntityToSelection(entity);
	}

	if (ImGui::MenuItem("Cube"))
	{
		ecs::World& world = Engine::GetSceneManager().GetCurrentEcsWorld();
		ecs::Entity entity = world.CreateEntity("Cube");
		ecs::Mesh mesh(L"../EngineAssets/Models/sm_cube.fbx");
		world.AddComponent(entity, mesh);
		
		mEditor.ClearEntitiesFromSelection();
		mEditor.AddEntityToSelection(entity);
	}
}
