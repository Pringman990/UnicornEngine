#pragma once
#include "../imgui/ImguiImpl.h"

class ToolWindow;

class EditorCore
{
public:
	EditorCore();
	~EditorCore();

	bool Init();
	void Render();
	void Update(float aDeltaTime);

	void RegisterToolWindows();

	ImguiImpl& GetImguiImpl();

	void AddEntityToSelection(uint32_t anEntity);
	void RemoveEntityFromSelection(uint32_t anEntity);
	void ClearEntitiesFromSelection();
	bool IsEntitySelected(uint32_t anEntity);
	const std::vector<uint32_t>& GetSelectedEntities();

	std::shared_ptr<ToolWindow> GetWindow(std::string aWindowName);

	void SetStyleEditor();
	void SetStylePlayMode();

private:
	std::unordered_map<std::string, std::shared_ptr<ToolWindow>> mToolWindows;
	ImguiImpl mImguiImpl;

	std::vector<uint32_t> mSelectedEntities;
};