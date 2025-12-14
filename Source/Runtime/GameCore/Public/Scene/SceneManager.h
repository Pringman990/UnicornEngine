#pragma once
#include <EngineMinimal.h>

#include "Scene/Scene.h"

/*
* Engine Subsystem
*/
class SceneManager
{
	friend struct subsystem::SubsystemDescriptor;

public:
	GAMECORE_API Scene* CreateScene(const String& Name);
	GAMECORE_API Scene* CreateScene(const UniqueID128& UUID, const String& Name);

	GAMECORE_API void UpdateActiveScene();

	GAMECORE_API bool LoadScene(const String& Name);
	GAMECORE_API void UnloadScene(const String& Name);

	GAMECORE_API Scene* GetActiveScene();
	GAMECORE_API Scene* GetScene(const String& Name);

	GAMECORE_API void SetActiveScene(const String& Name);

	GAMECORE_API void ClearAllScenes();

	GAMECORE_API bool SaveActiveSceneToFile(const Path& VirtualPath);
	GAMECORE_API bool SaveActiveScene();

	GAMECORE_API bool LoadSceneFromFile(const Path& VirtualPath);

	GAMECORE_API const UnorderedMap<UniqueID128, OwnedPtr<Scene>>& GetAllScenes() const { return mScenes; };

private:
	GAMECORE_API SceneManager();
	GAMECORE_API ~SceneManager();

	GAMECORE_API void LoadAllSceneFiles();
	GAMECORE_API UniqueID128 LoadSceneMetadataOnly(const Path& VirtualPath);

	GAMECORE_API Scene* GetSceneFromName(const String& Name);
	GAMECORE_API Scene* Exists(const String& Name);
	GAMECORE_API Scene* GetSceneFromUUID(const UniqueID128& UUID);

	GAMECORE_API bool SaveSceneToFile(Scene* ScenePtr, const Path& VirtualPath);
	GAMECORE_API bool LoadExistingScene(const UniqueID128& UUID);
private:
	UnorderedMap<UniqueID128, OwnedPtr<Scene>> mScenes;
	UnorderedMap<String, UniqueID128> mNameToUUID;
	Scene* mActiveScene;
};