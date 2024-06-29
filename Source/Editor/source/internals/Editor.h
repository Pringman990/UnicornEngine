#pragma once
#include "../imgui/ImguiImpl.h"

class ToolWindow;

class  Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void Update(float aDeltaTime);

	void RegisterToolWindows();

	ImguiImpl& GetImguiImpl();

	void AddEntityToSelection(uint32_t anEntity);
	void RemoveEntityFromSelection(uint32_t anEntity);
	void ClearEntitiesFromSelection();
	const std::vector<uint32_t>& GetSelectedEntities();

	std::shared_ptr<ToolWindow> GetWindow(std::string aWindowName);

private:
	
	std::unordered_map<std::string, std::shared_ptr<ToolWindow>> mToolWindows;
	ImguiImpl mImguiImpl;

	std::vector<uint32_t> mSelectedEntities;
};