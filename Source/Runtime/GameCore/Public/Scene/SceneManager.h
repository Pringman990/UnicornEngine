#pragma once
#include <EngineMinimal.h>

#include "Scene/Scene.h"

struct SceneErrorCode
{
	enum class ErrorCode
	{
		OK,
		InvalidPath,
		InvalidScenePtr
	};

	SceneErrorCode() {};
	SceneErrorCode(SceneErrorCode::ErrorCode Code)
	{
		code = Code;
	}

	ErrorCode code = ErrorCode::OK;

    operator bool() const
	{
		return code == ErrorCode::OK;
	}
};

/*
* Engine Subsystem
*/
class SceneManager
{
	friend struct subsystem::SubsystemDescriptor;

public:
	GAMECORE_API static SceneManager* Instance();

	GAMECORE_API void LoadAllSceneFiles();

	GAMECORE_API Scene* CreateScene(String Name);
	GAMECORE_API Scene* CreateScene(const UniqueID128& UUID, String Name);

	GAMECORE_API void UpdateActiveScene();

	GAMECORE_API bool LoadScene(StringView Name);
	GAMECORE_API void UnloadScene(StringView Name);

	GAMECORE_API Scene* GetActiveScene();
	GAMECORE_API Scene* GetScene(StringView Name);

	GAMECORE_API void SetActiveScene(StringView Name);

	GAMECORE_API void ClearAllScenes();

	GAMECORE_API SceneErrorCode SaveActiveSceneToFile(PathView FilePath);
	GAMECORE_API SceneErrorCode SaveActiveScene();

	GAMECORE_API bool LoadSceneFromFile(PathView FilePath);

	GAMECORE_API const UnorderedMap<UniqueID128, OwnedPtr<Scene>>& GetAllScenes() const { return mScenes; };

private:
	GAMECORE_API SceneManager();
	GAMECORE_API ~SceneManager();

	GAMECORE_API UniqueID128 LoadSceneMetadataOnly(PathView FilePath);

	GAMECORE_API Scene* GetSceneFromName(StringView Name);
	GAMECORE_API Scene* Exists(StringView Name);
	GAMECORE_API Scene* GetSceneFromUUID(const UniqueID128& UUID);

	GAMECORE_API SceneErrorCode SaveSceneToFile(Scene* ScenePtr, PathView FilePath);
	GAMECORE_API bool LoadExistingScene(const UniqueID128& UUID);
private:
	static SceneManager* sInstance;

	UnorderedMap<UniqueID128, OwnedPtr<Scene>> mScenes;
	UnorderedMap<String, UniqueID128> mNameToUUID;
	Scene* mActiveScene;
};