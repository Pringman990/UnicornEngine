#include "InternalViews/AssetRefTypeViews.h"

#include "Editor.h"
#include "Utility/ImguiUtility.h"
#include <misc/cpp/imgui_stdlib.h>

#include <RenderAssets/Mesh.h>
#include <Input/Assets/InputMapping.h>

REGISTER_EDITOR_VIEW(AssetRef<Mesh>,
	[](AssetRef<Mesh>& data, String label)
	{
		Mesh* asset = data ? data.Get() : nullptr;

		String meshName = "INVALID";
		if (asset)
			meshName = asset->GetName();

		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginCombo("##estaticmesh", meshName.c_str()))
		{
			auto meshes = AssetRegistry::Instance()->GetAssetsOfType<Mesh>("Mesh");

			for (auto& m : meshes)
			{
				bool selected = (m->GetName() == meshName);
				if (ImGui::Selectable(m->GetName().c_str(), &selected))
				{
					data = AssetRegistry::Instance()->GetAssetFromUUID<Mesh>(m->GetUUID());
				}
			}
			ImGui::EndCombo();
		}
	});

REGISTER_EDITOR_VIEW(AssetRef<InputMapping>,
	[](AssetRef<InputMapping>& data, String label)
	{
		InputMapping* asset = data ? data.Get() : nullptr;

		String assetName = "INVALID";
		if (asset)
			assetName = asset->GetName();

		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginCombo("##inputmapping", assetName.c_str()))
		{
			auto assets = AssetRegistry::Instance()->GetAssetsOfType<InputMapping>("InputMapping");

			for (auto& m : assets)
			{
				bool selected = (m->GetName() == assetName);
				if (ImGui::Selectable(m->GetName().c_str(), &selected))
				{
					data = AssetRegistry::Instance()->GetAssetFromUUID<InputMapping>(m->GetUUID());
				}
			}
			ImGui::EndCombo();
		}
	});

REGISTER_EDITOR_VIEW(AssetRef<InputAction>,
	[](AssetRef<InputAction>& data, String label)
	{
		InputAction* asset = data ? data.Get() : nullptr;

		String assetName = "INVALID";
		if (asset)
			assetName = asset->GetName();

		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginCombo("##InputAction", assetName.c_str()))
		{
			auto assets = AssetRegistry::Instance()->GetAssetsOfType<InputAction>("InputAction");

			for (auto& m : assets)
			{
				bool selected = (m->GetName() == assetName);
				if (ImGui::Selectable(m->GetName().c_str(), &selected))
				{
					data = AssetRegistry::Instance()->GetAssetFromUUID<InputAction>(m->GetUUID());
				}
			}
			ImGui::EndCombo();
		}
	});