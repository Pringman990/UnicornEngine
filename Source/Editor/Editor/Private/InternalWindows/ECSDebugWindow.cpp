#include "ECSDebugWindow.h"

#include <ESystemManager.h>

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
	auto systemsInfo = ESystemManager::Instance()->GetSystemsDebugInfo();
	auto systems = ESystemManager::Instance()->GetRegisteredSystems();

	for (const auto& [name, sys] : systems)
	{
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			if (ImGui::CollapsingHeader("debug info"))
			{
				if (systemsInfo.contains(name))
				{
					auto info = systemsInfo[name];
					ImGui::Text(String("Entity Count: " + std::to_string(info.entityCount)).c_str());
					ImGui::Text(String("Frame Run Time (sec): " + std::to_string(info.frameRunTime)).c_str());
					ImGui::Text(String("Frame Run Time (ms): " + std::to_string(info.frameRunTime * 1000.f)).c_str());
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("Component Types"))
	{
		auto types = refl::ReflectionRegistry::Instance()->GetAllTypesWithAttribute(refl::Attribute::EComponent);
		for (const auto& type : types)
		{
			ImGui::Selectable(type->name.c_str());
		}
	}

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
