#include "GameComponentViews.h"
#include <EditorViewRegistry.h>
#include <Editor.h>

#include <Components/ETransform.h>
#include <Components/EStaticMesh.h>

#include <Utility/ImguiUtility.h>

REGISTER_EDITOR_VIEW(ETransform,
	[](ETransform& data, String label)
	{
		if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit))
		{
			ImGui::TableSetupColumn("Label");
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

			// Position
			Vector3 entityPosition = data.position;
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Position:");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::DragFloat3("##Position", (float*)&entityPosition,
				Editor::Instance()->GetToolSettings().translationSpeed))
				data.position = entityPosition;

			// Rotation
			Vector3 eulerDeg = data.rotation;
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Rotation:");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::DragFloat3("##Rotation", (float*)&eulerDeg,
				Editor::Instance()->GetToolSettings().rotationSpeed))
				data.rotation = eulerDeg;

			// Scale
			Vector3 entityScale = data.scale;
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Scale:");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::DragFloat3("##Scale", (float*)&entityScale,
				Editor::Instance()->GetToolSettings().scaleSpeed, 0.001f))
				data.scale = entityScale;

			ImGui::EndTable();
		}
	});

REGISTER_EDITOR_VIEW(EEntity,
	[](EEntity& data, String label)
	{
		ImGui::DrawCopyableText(label.c_str(), data.ToString());
	});
