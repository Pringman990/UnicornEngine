#pragma once

using LevelID = uint32_t;

class LevelManager
{
public:
	static LevelManager& GetInstance()
	{
		static LevelManager instance;
		return instance;
	}

	ecs::World& GetCurrentLevel();

private:
	LevelManager() = default;
	~LevelManager() = default;
private:
	LevelID mCurrentLevel = 0;
	LevelID mNextLevelID = 0;
	std::unordered_map<LevelID, ecs::World> mLevels;
};