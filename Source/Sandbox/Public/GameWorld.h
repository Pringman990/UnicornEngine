#pragma once

#ifdef _WIN32

#ifdef SANDBOX_EXPORTS
#define SANDBOX_API __declspec(dllexport)
#else // SINGLETON_EXPORTS
#define SANDBOX_API __declspec(dllimport)
#endif // SINGLETON_EXPORTS

#else // _WIN32
#define SANDBOX_API 
#endif // _WIN32

#include <Mesh.h>
#include <Math/Transform.h>

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Render();

private:
	std::vector<std::pair<Mesh*, Transform>> mMeshes;
};

extern "C" SANDBOX_API void InitGameWorld();
extern "C" SANDBOX_API void RenderGameWorld();