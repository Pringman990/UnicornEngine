#include "GameWorld.h"

#include <StandardTypes/StandardTypes.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include <Logger/Logger.h>

namespace {
	GameWorld gameworld;
}

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}
void GameWorld::Init()
{
	LOG_INFO("GameWorld init...");

}

void GameWorld::Render()
{
}

void GameWorld::UpdateCamera()
{
	
}

void InitGameWorld()
{
	gameworld.Init();
}

void RenderGameWorld()
{
	gameworld.Render();
}
