#include "ECSDebugWindow.h"

ECSDebugWindow::ECSDebugWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
	mWindowDisplayName = "ECS Debug";
}

ECSDebugWindow::~ECSDebugWindow()
{
}

bool ECSDebugWindow::Init()
{
	return true;
}

void ECSDebugWindow::Render()
{
	//ImGui::BeginColumns("#", 3);

	////Column 1
	//{
	//	if (ImGui::CollapsingHeader("World"))
	//	{
	//		if (ImGui::Button("Create Entity"))
	//		{
	//			mWorld.CreateEntity();
	//		}
	//	}

	//	if (ImGui::CollapsingHeader("Entity list"))
	//	{
	//		int index = 0;
	//		for (EEntity entity : mWorld.GetAllEntities())
	//		{
	//			bool selected = mSelectedEntity == entity ? true : false;
	//			
	//			String ent = std::to_string(index) + ": " + std::to_string(entity);
	//			if (ImGui::Selectable(ent.c_str(), &selected))
	//			{
	//				mSelectedEntity = entity;
	//			}

	//			if (!selected && mSelectedEntity == entity)
	//				mSelectedEntity = 0;

	//			index++;
	//		}
	//	}

	//	if (ImGui::CollapsingHeader("Components"))
	//	{
	//		for (auto uuid : EComponentRegistry::Get()->GetAllComponentsTypes())
	//		{
	//			auto info = ReflectionRegistry::Get()->GetInfo(uuid);

	//			bool selected = mSelectedComponent == uuid ? true : false;

	//			if (ImGui::Selectable(info->name.data(), &selected))
	//			{
	//				mSelectedComponent = uuid;
	//			}

	//			if (!selected && mSelectedComponent == uuid)
	//				mSelectedComponent = {};
	//		}
	//	}
	////}

	////Column 2
	//{
	//	ImGui::NextColumn();
	//	ImGui::Text("Component Reflection Info");

	//	if (mSelectedComponent.IsValid())
	//	{
	//		auto info = ReflectionRegistry::Get()->GetInfo(mSelectedComponent);
	//		ImGui::Text("Name: %s", info->name.data());
	//		ImGui::Text("Size: %u", info->size);
	//		if (ImGui::CollapsingHeader("Members"))
	//		{
	//			for (uint32 i = 0; i < info->members.size(); i++)
	//			{
	//				ImGui::Text("Name: %s", info->members[i].name.data());
	//				ImGui::Text("Size: %u", info->members[i].size);
	//				ImGui::Text("Offset: %u", info->members[i].offset);
	//			}
	//		}
	//	}
	//}

	////Column 3
	//{
	//	ImGui::NextColumn();
	//	ImGui::Text("Entity Info");

	//	if (mSelectedEntity != 0)
	//	{
	//		ImGui::Text("ID: %u", mSelectedEntity);
	//	}
	//}

	//ImGui::EndColumns();
}
