#include "StandardTypeViews.h"

#include "Editor.h"
#include "Utility/ImguiUtility.h"
#include <misc/cpp/imgui_stdlib.h>

REGISTER_EDITOR_VIEW(int,
	[](int& data, String label)
	{
		ImGui::DragInt(label.c_str(), &data, 0.1f);
	});

REGISTER_EDITOR_VIEW(float,
	[](float& data, String label)
	{
		ImGui::DragFloat(label.c_str(), &data, 0.1f);
	});

REGISTER_EDITOR_VIEW(Vector3,
	[](Vector3& data, String label)
	{
		Vector3 vector = data;
		ImGui::Text((label + ": ").c_str());
		ImGui::SameLine();
		if (ImGui::DragFloat3(("##" + label).c_str(), (float*)&vector, 0.1f))
			data = vector;
	});

REGISTER_EDITOR_VIEW(String,
	[](String& data, String label)
	{
		ImGui::InputText(label.c_str(), &data);
	});