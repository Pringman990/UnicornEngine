#include "EditorPch.h"
#include "Inspector.h"

#include <scene/SceneManager.h>

Inspector::Inspector(EditorCore& aEditor) : ToolWindow(aEditor)
{
}

Inspector::~Inspector()
{
}

void Inspector::Draw(float /*aDeltaTime*/)
{
	if (mEditor.GetSelectedEntities().size() < 1)
		return;

	auto typeToVectorMap = Engine::GetSceneManager().GetCurrentEcsWorld().GetAllEntityComponents(mEditor.GetSelectedEntities()[0]);

	void* data = typeToVectorMap[typeid(ecs::Name)];

	EngineCustomDraw(typeid(ecs::Name), data);

	for (auto& [type, ptr] : typeToVectorMap)
	{
		reflection::TypeInfo typeInfo = reflection::Registry::GetInstance().GetTypeInfo(type);
		
		if (typeInfo.name == "_INVALID" || 
			*typeInfo.type == typeid(ecs::ChildParent) ||
			*typeInfo.type == typeid(ecs::Name) ||
			*typeInfo.type == typeid(ecs::Tag)
			)
			continue;

		if (ImGui::CollapsingHeader(typeInfo.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& memberInfo : typeInfo.members)
			{
				ImGui::DisplayReflectedValues(typeInfo.name, memberInfo, ptr);
			}
		}

		ImGui::Separator();
	}
}

void Inspector::EngineCustomDraw(ecs::ComponentType aType, void*& someData)
{
	if (aType == typeid(ecs::Name))
	{
		ecs::Name* nameComponent = static_cast<ecs::Name*>(someData);
		nameComponent;
		char* firstElement = static_cast<char*>(someData);
		std::string* name = reinterpret_cast<std::string*>(firstElement);
		if (ImGui::InputText("##_EntityName", name))
		{

		}
	}
}
