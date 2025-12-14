#pragma once
#include <EngineMinimal.h>
#include <imgui.h>

namespace ImGui
{
	inline void DrawCopyableText(const char* Label, const String& Value)
	{
		ImGui::Text(Label);
		ImGui::SameLine();

		// Remove background + padding so it aligns vertically
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

		ImGui::InputText("##copyabletext", (char*)Value.c_str(), Value.size() + 1, ImGuiInputTextFlags_ReadOnly);

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
	}
}