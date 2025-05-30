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

#include <Math/Transform.h>
#include <Camera.h>
#include <Core.h>

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void InitCompute();
	void Render();
private:
	void UpdateCamera();
private:
	Camera mCamera;
};

extern "C" SANDBOX_API void InitGameWorld();
extern "C" SANDBOX_API void RenderGameWorld();