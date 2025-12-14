#pragma once
#include <EngineMinimal.h>
#include "EditorWindow.h"

class ReflectionRegistryWindow : public EditorWindow
{
public:
	ReflectionRegistryWindow(Editor* EditorPtr);
	~ReflectionRegistryWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	void DrawTypeDetails(const refl::Type* Type);
	void DrawPropertyDetails(const refl::Type* Type, const refl::Property& Prop);
	void DrawAttributes(const UnorderedMap<refl::Attribute, refl::AttributeVariant>& Attributes);
};