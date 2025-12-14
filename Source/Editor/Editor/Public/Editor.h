#pragma once
#include <Core.h>
#include <ECommon.h>

class IImguiBackend;
class EditorWindowManager;

enum class SelectedItemType
{ 
	Non,
	Entity
};

using SelectedItemVariant = Variant<std::monostate, EEntity>;

struct SelectedItem
{
	SelectedItemVariant item;
	SelectedItemType type;
};

struct EditorToolSettings
{
	/**
	* Decides the speed in which we move the translation of a object.
	* Measured in actual float value and is not a multiplier.
	*/
	float translationSpeed = 0.01f;
	float rotationSpeed = 0.01f;
	float scaleSpeed = 0.01f;
};

class Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void BeginFrame();
	void Render();
	void EndFrame();

	uint32 GetPreviousFrameDrawCalls() const { return mPreviousFrameDrawCalls; };

	void SetSelectedItem(SelectedItemType Type, const SelectedItemVariant& Item);
	const SelectedItem& GetSelectedItem() const { return mSelectedItem; };
	void InvalidateSelectedItem();

	EditorToolSettings& GetToolSettings() { return mEditorToolSettings; };

private:
	void RenderMainMenuBar();
	void RegisterEditorWindows();
private:
	IImguiBackend* mImguiBackend;
	EditorWindowManager* mWindowManager;
	uint32 mPreviousFrameDrawCalls = 0;

	SelectedItem mSelectedItem;
	EditorToolSettings mEditorToolSettings;
};