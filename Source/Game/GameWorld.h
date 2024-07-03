#pragma once

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();

private:
	void RegisterSystems();
};