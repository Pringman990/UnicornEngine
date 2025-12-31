#include "InspectorWindow.h"

#include <Scene/SceneManager.h>
#include <EWorld.h>

#include <Components/ETransform.h>
#include <Components/EStaticMesh.h>

#include "EditorViewRegistry.h"

#include <EditorWindowManager.h>
REGISTER_EDITOR_WINDOW(InspectorWindow, "92b8e2bc-a1bb-49aa-8509-a85b46afad3a");

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
	return true;
}

void InspectorWindow::Render()
{
	if (mEditor->GetSelectedItem().type != SelectedItemType::Entity)
	{
		return;
	}

	SceneManager* sceneManager = SceneManager::Instance();

	EWorld& world = sceneManager->GetActiveScene()->GetWorld();

	EEntity entity = std::get<EEntity>(mEditor->GetSelectedItem().item);
	auto components = world.GetAllComponentsOnEntity(entity);

	ImGui::DrawCopyableText("UUID: ", entity.ToString());

	for (auto& [typeUUID, componentPtr] : components)
	{
		const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull(typeUUID);
		if (type == nullptr)
		{
			LOG_ERROR("Entity had a component of type that was not valid");
			return;
		}

		const char* typeName = type->name.c_str();

		ImGui::PushID(typeName);
		ImGui::SeparatorText(typeName);
		if (type->name != "ETransform" && type->name != "ENameComponent")
		{
			float width = ImGui::CalcTextSize(ICON_FA_TRASH).x
				+ ImGui::GetStyle().FramePadding.x * 2;

			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - width);

			if (ImGui::Button(ICON_FA_TRASH))
			{
				world.RemoveComponent(entity, typeUUID);
				ImGui::PopID();
				continue;
			}
		}
		ImGui::PopID();
		CallViewFunctionsRecurcivly(typeUUID, componentPtr);
	}

	ImGui::SeparatorText("");

	ImGui::SetNextItemWidth(-FLT_MIN);
	if (ImGui::BeginCombo("##componentsDropdown", "Components"))
	{
		auto reflComponents = refl::ReflectionRegistry::Instance()->GetAllTypesWithAttribute(refl::Attribute::EComponent);
		for (auto& comp : reflComponents)
		{
			if (ImGui::Selectable(comp->name.c_str(), false))
			{
				world.AddComponent(entity, comp->uuid);
			}
		}
		ImGui::EndCombo();
	}
}

void InspectorWindow::CallViewFunctionsRecurcivly(UniqueID128 TypeID, void* ComponentPtr)
{
	auto func = EditorViewRegistry::Instance()->GetFunction(TypeID);
	if (func)
	{
		func(ComponentPtr, "");
		return;
	}

	const refl::Type* type = refl::ReflectionRegistry::Instance()->GetOrNull(TypeID);
	if (type == nullptr)
	{
		LOG_ERROR("Entity had a component of type that was not valid");
		return;
	}

	for (auto& member : type->properties)
	{
		void* memberPtr = static_cast<byte*>(ComponentPtr) + member.offset;

		auto memberFunc = EditorViewRegistry::Instance()->GetFunction(member.type->uuid);
		if (memberFunc)
		{
			const char* memberName = member.name.c_str();
			auto typeAttIt = member.attributes.find(refl::Attribute::DisplayName);
			if (typeAttIt != member.attributes.end())
				memberName = std::get<String>(typeAttIt->second).c_str();

			memberFunc(memberPtr, memberName);
		}
		else
		{
			CallViewFunctionsRecurcivly(member.type->uuid, memberPtr);
		}
	}
}
