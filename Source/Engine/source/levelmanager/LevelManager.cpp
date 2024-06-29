#include "EnginePch.h"
#include "LevelManager.h"

ecs::World& LevelManager::GetCurrentLevel()
{
	return mLevels[mCurrentLevel];
}
