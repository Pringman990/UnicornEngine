#include "EditorPch.h"
#include "Inspector.h"

extern ecs::World gECSWorld;

Inspector::Inspector(Editor& aEditor) : ToolWindow(aEditor)
{
}

Inspector::~Inspector()
{
}

void Inspector::Draw(float /*aDeltaTime*/)
{
	/*if (mEditor.GetSelectedEntities().size() < 1)
		return;*/

	ImGui::Separator();

	auto typeToVectorMap = gECSWorld.GetAllEntityComponents(0/*mEditor.GetSelectedEntities()[0]*/);

	for (auto& [type, ptr] : typeToVectorMap)
	{
		reflection::TypeInfo typeInfo = reflection::Registry::GetInstance().GetTypeInfo(type);
		//ImGui::Text(typeInfo.name.c_str());

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
