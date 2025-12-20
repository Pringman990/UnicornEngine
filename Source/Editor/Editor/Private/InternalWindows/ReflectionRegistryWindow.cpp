#include "ReflectionRegistryWindow.h"

ReflectionRegistryWindow::ReflectionRegistryWindow(Editor* EditorPtr)
	:
	EditorWindow(EditorPtr)
{
	mWindowDisplayName = "Reflection Registry";
}

ReflectionRegistryWindow::~ReflectionRegistryWindow()
{
}

bool ReflectionRegistryWindow::Init()
{
	return true;
}

void ReflectionRegistryWindow::Render()
{
	auto types = refl::ReflectionRegistry::Instance()->GetAllTypes();

	ImGui::Columns(2);

	ImGui::Text("Types");
	//TODO: add search bar for uuid and name and type index

	ImGui::Separator();

	static const refl::Type* selectedType = nullptr;
	for (const auto& type : types)
	{
		if (ImGui::Selectable(type->name.c_str(), type == selectedType))
			selectedType = type;
	}

	ImGui::NextColumn();

	if (selectedType)
	{
		DrawTypeDetails(selectedType);
	}

	ImGui::Columns(1);
}

void ReflectionRegistryWindow::DrawTypeDetails(const refl::Type* Type)
{
	ImGui::Text("Name: %s", Type->name.c_str());
	ImGui::DrawCopyableText("UUID:", Type->uuid.ToString().c_str());
	ImGui::Text("Type Index: %s", Type->typeIndex.name());
	ImGui::Text("Size: %d", Type->size);
	ImGui::Text("Is Fully Reflected: %s", ToString(Type->isFullyReflected).c_str());
	ImGui::Text("Is Valid: %s", ToString(Type->isValid).c_str());

	ImGui::SeparatorText("Attributes");
	ImGui::Indent();
	
	DrawAttributes(Type->attributes);

	ImGui::Unindent();
	ImGui::SeparatorText("Properties");
	ImGui::Indent();

	for (auto& p : Type->properties)
	{
		DrawPropertyDetails(Type, p);
	}

	ImGui::Unindent();
}

void ReflectionRegistryWindow::DrawPropertyDetails(const refl::Type* Type, const refl::Property& Prop)
{
	ImGui::PushID(&Prop);

	if (ImGui::TreeNode(Prop.name.c_str()))
	{
		ImGui::Text("Type: %s", Prop.type->name.c_str());
		ImGui::DrawCopyableText("UUID:", Prop.type->uuid.ToString());
		ImGui::Text("Offset: %d", static_cast<int32>(Prop.offset));
		
		ImGui::SeparatorText("Property Attributes");
		ImGui::Indent();
		DrawAttributes(Prop.attributes);
		ImGui::Unindent();

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void ReflectionRegistryWindow::DrawAttributes(const UnorderedMap<refl::Attribute, refl::AttributeVariant>& Attributes)
{
	for (auto& [att, value] : Attributes)
	{
		switch (att)
		{
		case refl::Attribute::DisplayName:
		{
			ImGui::Text("Type: %s, Value: %s", "Display Name", std::get<String>(value).c_str());
			break;
		}
		case refl::Attribute::Private:
		{
			ImGui::Text("Type: %s, Value: %s", "Private", "monostate");
			break;
		}
		case refl::Attribute::EComponent:
		{
			ImGui::Text("Type: %s, Value: %s", "EComponent", "monostate");
			break;
		}
		}
	}
}

