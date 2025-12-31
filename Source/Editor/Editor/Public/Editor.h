#pragma once
#include <Core.h>
#include <ECommon.h>
#include "EditorDefines.h"

class IImguiBackend;

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

enum class EditorPlayState
{
	Play,
	Stopped
};

struct InputTextDialogInfo
{
	bool open = false;
	bool confirmed = false;
	char input[128] = "";

	String windowText;
	String inputText;
};

class Editor
{
public:
	EDITOR_API static Editor* Instance();

	EDITOR_API void Shutdown();

	EDITOR_API bool Init();
	EDITOR_API void BeginFrame();
	EDITOR_API void Render();
	EDITOR_API void EndFrame();

	uint32 GetPreviousFrameDrawCalls() const { return mPreviousFrameDrawCalls; };

	EDITOR_API void SetSelectedItem(SelectedItemType Type, const SelectedItemVariant& Item);
	const SelectedItem& GetSelectedItem() const { return mSelectedItem; };
	EDITOR_API void InvalidateSelectedItem();

	EditorToolSettings& GetToolSettings() { return mEditorToolSettings; };

	EditorPlayState GetPlayState() const { return mPlayState; }
	void SetPlayState(EditorPlayState State) 
	{
		mPlayState = State;
		PlayStateChangeNotifier.Notify(State);
	};

	MultiNotifierArgs<EditorPlayState> PlayStateChangeNotifier;

private:
	Editor();
	~Editor();

	bool RenderTextInputBox();
	void OpenNewSceneCreatePopup();

	void RenderMainMenuBar();
	void RegisterEditorWindows();

	void PlayStateChangeCallback(EditorPlayState State);
private:
	IImguiBackend* mImguiBackend;
	uint32 mPreviousFrameDrawCalls = 0;

	SelectedItem mSelectedItem;
	EditorToolSettings mEditorToolSettings;

	InputTextDialogInfo mInputTextDialogInfo;

	class Scene* mPlayScene;
	String mCopiedSceneName;

	EditorPlayState mPlayState;
};