#include "EditorPch.h"
#include "Utility.h"

#include <model/ModelInstance.h>
#include <model/Model.h>

void ImGui::DisplayReflectedValues(const std::string& aTypeName, const reflection::MemberInfo& aMember, void*& someData)
{
	char* firstElement = static_cast<char*>(someData);
	char* wantedMemberPtr = firstElement + aMember.offset;

	if (aMember.type == typeid(int))
	{
		int dest;
		memcpy_s(&dest, sizeof(int), wantedMemberPtr, sizeof(int));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::DragInt("##", &dest))
		{
			memcpy_s(wantedMemberPtr, sizeof(int), &dest, sizeof(int));
		}
		ImGui::PopID();
	}
	else if (aMember.type == typeid(float))
	{
		float dest;
		memcpy_s(&dest, sizeof(float), wantedMemberPtr, sizeof(float));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::DragFloat("##", &dest))
		{
			memcpy_s(wantedMemberPtr, sizeof(float), &dest, sizeof(float));
		}
		ImGui::PopID();
	}
	else if (aMember.type == typeid(double))
	{
		double dest;
		memcpy_s(&dest, sizeof(double), wantedMemberPtr, sizeof(double));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::InputDouble("##", &dest))
		{
			memcpy_s(wantedMemberPtr, sizeof(double), &dest, sizeof(double));
		}
		ImGui::PopID();
	}
	else if (aMember.type == typeid(std::string))
	{
		std::string* stringPtr = reinterpret_cast<std::string*>(wantedMemberPtr);

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		ImGui::InputText("##", stringPtr);
		ImGui::PopID();
	}
	else if (aMember.type == typeid(std::wstring))
	{
		std::wstring* stringPtr = reinterpret_cast<std::wstring*>(wantedMemberPtr);

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		std::string str = WStringToString(*stringPtr);
		if (ImGui::InputText("##", &str))
		{
			std::wstring wstr = StringToWString(str);
			*stringPtr = wstr;
		}
		ImGui::PopID();
	}
	else if (aMember.type == typeid(const char*))
	{
		const char** stringPtr = reinterpret_cast<const char**>(wantedMemberPtr);

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();
		ImGui::Text(*stringPtr);
	}
	else if (aMember.type == typeid(const wchar_t*))
	{
		const wchar_t** stringPtr = reinterpret_cast<const wchar_t**>(wantedMemberPtr);

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();
		std::string str = WStringToString(*stringPtr);
		ImGui::Text(str.c_str());
	}
	else if (aMember.type == typeid(Vector2))
	{
		Vector3 dest;
		memcpy_s(&dest, sizeof(Vector3), wantedMemberPtr, sizeof(Vector3));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::DragFloat3("##", (float*)&dest, 0.01f))
		{
			memcpy_s(wantedMemberPtr, sizeof(Vector3), &dest, sizeof(Vector3));
		}
		ImGui::PopID();
	}
	else if (aMember.type == typeid(Vector3))
	{
		Vector3 dest;
		memcpy_s(&dest, sizeof(Vector3), wantedMemberPtr, sizeof(Vector3));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::DragFloat3("##", (float*)&dest, 0.01f))
		{
			memcpy_s(wantedMemberPtr, sizeof(Vector3), &dest, sizeof(Vector3));
		}
		ImGui::PopID();
		}
	else if (aMember.type == typeid(Vector4))
	{
		Vector3 dest;
		memcpy_s(&dest, sizeof(Vector3), wantedMemberPtr, sizeof(Vector3));

		ImGui::Text("%s:", aMember.name.c_str());
		ImGui::SameLine();

		ImGui::PushID((aTypeName + aMember.name).c_str());
		if (ImGui::DragFloat3("##", (float*)&dest, 0.01f))
		{
			memcpy_s(wantedMemberPtr, sizeof(Vector3), &dest, sizeof(Vector3));
		}
		ImGui::PopID();
		}
	else if (aMember.type == typeid(ModelInstance))
	{
		float indent = 0;
		ImGui::Indent(10);
		indent += 10;
		if (ImGui::CollapsingHeader("Model Instance"))
		{
			ImGui::Indent(10);
			indent += 10;
			if (ImGui::CollapsingHeader("Meshes"))
			{
				ModelInstance* mdlInstance = reinterpret_cast<ModelInstance*>(wantedMemberPtr);

				Model mdl = *mdlInstance->GetModel();
				const std::vector<MeshData>& meshData = mdl.GetMeshData();
				
				ImGui::Indent(10);
				indent += 10;
				for (int i = 0; i < meshData.size(); i++)
				{
					if (ImGui::CollapsingHeader(("Mesh " + std::to_string(i)).c_str()))
					{
						ImGui::Text("Index Count: %d", meshData[i].indexCount);
						ImGui::Text("Vertex Count: %d", meshData[i].vertexCount);
					}
					//TODO: Add changing of materials
				}
			}
		}
		ImGui::Unindent(indent);
	}
	else if (
		aMember.type == typeid(std::vector<int>) ||
		aMember.type == typeid(std::vector<float>) ||
		aMember.type == typeid(std::vector<double>) ||
		aMember.type == typeid(std::vector<std::string>) ||
		aMember.type == typeid(std::vector<const char*>)
		)
	{
		internal::DisplayVectorTypes(aTypeName, aMember, wantedMemberPtr);
	}
}

void ImGui::internal::DisplayVectorTypes(const std::string& aTypeName, const reflection::MemberInfo& aMember, char*& someData)
{
	ImGui::PushID((aTypeName + aMember.name).c_str());
	if (ImGui::CollapsingHeader(aMember.name.c_str()))
	{
		if (aMember.type == typeid(std::vector<int>))
		{
			std::vector<int>* vectorPtr = reinterpret_cast<std::vector<int>*>(someData);

			for (int i = 0; i < vectorPtr->size(); i++)
			{
				ImGui::Text("%d: ", i);
				ImGui::SameLine();

				ImGui::PushID((aTypeName + aMember.name + std::to_string(i)).c_str());
				ImGui::DragInt("##", &(*vectorPtr)[i]);
				ImGui::PopID();
			}
		}
		else if (aMember.type == typeid(std::vector<float>))
		{
			std::vector<float>* vectorPtr = reinterpret_cast<std::vector<float>*>(someData);

			for (int i = 0; i < vectorPtr->size(); i++)
			{
				ImGui::Text("%d: ", i);
				ImGui::SameLine();

				ImGui::PushID((aTypeName + aMember.name + std::to_string(i)).c_str());
				ImGui::DragFloat("##", &(*vectorPtr)[i]);
				ImGui::PopID();
			}
		}
		else if (aMember.type == typeid(std::vector<double>))
		{
			std::vector<double>* vectorPtr = reinterpret_cast<std::vector<double>*>(someData);

			for (int i = 0; i < vectorPtr->size(); i++)
			{
				ImGui::Text("%d: ", i);
				ImGui::SameLine();

				ImGui::PushID((aTypeName + aMember.name + std::to_string(i)).c_str());
				ImGui::InputDouble("##", &(*vectorPtr)[i]);
				ImGui::PopID();
			}
		}
		else if (aMember.type == typeid(std::vector<std::string>))
		{
			std::vector<std::string>* vectorPtr = reinterpret_cast<std::vector<std::string>*>(someData);

			for (int i = 0; i < vectorPtr->size(); i++)
			{
				ImGui::Text("%d: ", i);
				ImGui::SameLine();

				ImGui::PushID((aTypeName + aMember.name + std::to_string(i)).c_str());
				ImGui::InputText("##", &(*vectorPtr)[i]);
				ImGui::PopID();
			}
		}
		else if (aMember.type == typeid(std::vector<const char*>))
		{
			std::vector<const char*>* vectorPtr = reinterpret_cast<std::vector<const char*>*>(someData);

			for (int i = 0; i < vectorPtr->size(); i++)
			{
				ImGui::Text("%d: ", i);
				ImGui::SameLine();

				ImGui::PushID((aTypeName + aMember.name + std::to_string(i)).c_str());
				ImGui::Text((*vectorPtr)[i]);
				ImGui::PopID();
			}
		}
	}
	ImGui::PopID();
}