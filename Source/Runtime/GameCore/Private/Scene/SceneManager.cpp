#include "Scene/SceneManager.h"

SceneManager* SceneManager::sInstance = nullptr;
REGISTER_ENGINE_SUBSYSTEM(SceneManager)

#include <YAML/YamlHelper.h>
#include <Archive/YamlArchive.h>

SceneManager::SceneManager()
	:
	mActiveScene(nullptr)
{
#ifdef _DEBUG
	ASSERT(sInstance == nullptr, "The instance was not null and we are trying to set it again");
#endif
	sInstance = this;
}

SceneManager::~SceneManager()
{
	if (sInstance == this)
		sInstance = nullptr;
}

GAMECORE_API SceneManager* SceneManager::Instance()
{
#ifdef _DEBUG
	ASSERT(sInstance, "Instance was accessed before/after it was created/destroyed");
#endif
	return sInstance;
}

GAMECORE_API void SceneManager::LoadAllSceneFiles()
{
	Path root = FileSystem::Instance()->GetAbsolutPath("engine://");
	for (auto& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (!entry.is_directory() && entry.path().extension() == ".scene")
		{
			auto relative = std::filesystem::relative(entry.path(), root);
			Path virtualPath = "engine://" + relative.generic_string();

			ByteBuffer buffer = FileSystem::Instance()->ReadAll(virtualPath);

			LoadSceneMetadataOnly(virtualPath);
		}
	}
}

Scene* SceneManager::CreateScene(String Name)
{
	return CreateScene(UniqueID128::FromRandom(), Name);
}

GAMECORE_API Scene* SceneManager::CreateScene(const UniqueID128& UUID, String Name)
{
	Scene* checkNameScene = Exists(Name);
	if (checkNameScene)
	{
		LOG_WARNING("There already exists a scene with name, {}", Name);
		return nullptr;
	}

	OwnedPtr<Scene> scene = MakeOwned<Scene>(UUID, Name);
	mScenes[UUID] = std::move(scene);
	mNameToUUID[Name] = UUID;

	return mScenes[UUID].get();
}

void SceneManager::UpdateActiveScene()
{
	mActiveScene->OnUpdate();
}

Scene* SceneManager::GetActiveScene()
{
	return mActiveScene;
}

Scene* SceneManager::GetScene(StringView Name)
{
	return GetSceneFromName(Name);
}

void SceneManager::SetActiveScene(StringView Name)
{
	Scene* scene = GetSceneFromName(Name);
	if (!scene)
	{
		LOG_ERROR("Failed to set scene '{}' as active");
		return;
	}
	mActiveScene = scene;
	mActiveScene->OnLoad();
}

void SceneManager::ClearAllScenes()
{
	mScenes.clear();
	mNameToUUID.clear();
	mActiveScene = nullptr;
}

SceneErrorCode SceneManager::SaveActiveSceneToFile(PathView FilePath)
{
	SceneErrorCode result;
	if (result = SaveSceneToFile(mActiveScene, FilePath))
	{
		String sourcePath(FilePath);
		mActiveScene->SetSourcePath(sourcePath);
	}
	return result;
}

SceneErrorCode SceneManager::SaveActiveScene()
{
	if (mActiveScene->GetName() == "Default")
		return SceneErrorCode(SceneErrorCode::ErrorCode::OK);

	if (mActiveScene->GetSourcePath().empty())
	{
		return SceneErrorCode(SceneErrorCode::ErrorCode::InvalidPath);
	}

	return SaveSceneToFile(mActiveScene, mActiveScene->GetSourcePath());
}

Scene* SceneManager::GetSceneFromName(StringView Name)
{
	Scene* scene = Exists(Name);
	if (!scene)
	{
		LOG_WARNING("Can't get scene with name '{}'", Name);
		return nullptr;
	}

	return scene;
}

GAMECORE_API Scene* SceneManager::Exists(StringView Name)
{
	auto it = mNameToUUID.find(String(Name));
	if (it == mNameToUUID.end())
	{
		return nullptr;
	}

	return GetSceneFromUUID(it->second);
}

Scene* SceneManager::GetSceneFromUUID(const UniqueID128& UUID)
{
	auto it = mScenes.find(UUID);
	if (it == mScenes.end())
	{
		LOG_WARNING("Can't set scene with uuid '{}'", UUID.ToString());
		return nullptr;
	}

	return it->second.get();
}

GAMECORE_API SceneErrorCode SceneManager::SaveSceneToFile(Scene* ScenePtr, PathView FilePath)
{
	FileSystem* fileSys = FileSystem::Instance();
	if (!fileSys->Exists(FilePath))
	{
		LOG_ERROR("Filed to save active scene to file path '{}', because it doesn't exist", FilePath);
		return SceneErrorCode(SceneErrorCode::ErrorCode::InvalidPath);
	}

	if (!ScenePtr)
	{
		LOG_ERROR("Can't save the active scene as its null??");
		return SceneErrorCode(SceneErrorCode::ErrorCode::InvalidScenePtr);
	}

	YamlArchive archive;
	archive.WriteString(ScenePtr->GetUUID().ToString(), "uuid");
	archive.WriteString(ScenePtr->GetName(), "name");

	EWorld& world = ScenePtr->GetWorld();
	const Vector<EEntity>& worldEntities = world.GetAllEntities();
	
	archive.BeginWriteArray("entities");
	for (uint32 i = 0; i < worldEntities.size(); i++)
	{
		archive.WriteString(worldEntities[i].ToString());
	}
	archive.EndWriteArray();

	refl::ReflectionRegistry* rr = refl::ReflectionRegistry::Instance();
	auto& componentStores = world.ComponentAllStores();

	archive.BeginWriteObject("components");
	for (auto& [componentUUID, store] : componentStores)
	{
		const refl::Type* type = rr->GetOrNull(componentUUID);
		//I don't think this can ever happen as all components added on entities most be is_component reflected
		if (!type)
			continue;
		
		uint32 componentCount = store.allocator.GetCount();
		
		archive.BeginWriteArray(componentUUID.ToString());
		for (uint32 i = 0; i < componentCount; i++)
		{
			void* component = store.allocator.Get(i);
			if (!component)
			{
				continue;
			}

			archive.BeginWriteObject();
			archive.WriteString(store.indexToEntity.at(i).ToString(), "entity");
			type->functions.saveFunction(component, archive, "");
			archive.EndWriteObject();
		}
		archive.EndWriteArray();
	}
	archive.EndWriteObject();

	archive.WriteToFile(FilePath);
	return SceneErrorCode(SceneErrorCode::ErrorCode::OK);
}

GAMECORE_API bool SceneManager::LoadScene(StringView Name)
{
	if (Name == "Default")
		return true;

	Scene* scene = GetSceneFromName(Name);
	if (!scene)
	{
		LOG_ERROR("Failed to load scene because it doesn't exist, '{}'", Name);
		return false;
	}

	return LoadExistingScene(scene->GetUUID());
}

GAMECORE_API void SceneManager::UnloadScene(StringView Name)
{
	Scene* scene = GetSceneFromName(Name);
	if (!scene)
	{
		LOG_ERROR("Failed to unload scene because it doesn't exist, '{}'", Name);
		return;
	}

	scene->Unload();
}

GAMECORE_API UniqueID128 SceneManager::LoadSceneMetadataOnly(PathView FilePath)
{
	FileSystem* fileSys = FileSystem::Instance();
	if (!fileSys->Exists(FilePath))
	{
		LOG_ERROR("Failed to load scene to file path '{}', because it doesn't exist", FilePath);
		return UniqueID128::Invalid();
	}

	ByteBuffer buffer = fileSys->ReadAll(FilePath);
	YamlArchive archive(buffer);

	String uuidStr;
	archive.ReadString(uuidStr, "uuid");
	UniqueID128 uuid(uuidStr);

	String name;
	archive.ReadString(name, "name");
	
	Scene* scene = CreateScene(uuid, name);
	scene->SetSourcePath(String(FilePath));

	return uuid;
}

GAMECORE_API bool SceneManager::LoadSceneFromFile(PathView FilePath)
{
	FileSystem* fileSys = FileSystem::Instance();
	if (!fileSys->Exists(FilePath))
	{
		LOG_ERROR("Filed to load scene from file path '{}', because it doesn't exist", FilePath);
		return false;
	}

	UniqueID128 uuid = LoadSceneMetadataOnly(FilePath);
	if (!uuid.IsValid())
	{
		LOG_ERROR("Failed to get scene uuid, path '{}'", FilePath);
		return false;
	}

	return LoadExistingScene(uuid);
}

GAMECORE_API bool SceneManager::LoadExistingScene(const UniqueID128& UUID)
{
	Scene* scene = GetSceneFromUUID(UUID);

	if (!scene)
	{
		LOG_ERROR("Failed to load scene because it doesn't exist, '{}'", UUID.ToString());
		return false;
	}

	FileSystem* fileSys = FileSystem::Instance();
	if (!fileSys->Exists(scene->GetSourcePath()))
	{
		LOG_ERROR("Filed to load scene to file path '{}', because it doesn't exist", scene->GetSourcePath());
		return false;
	}

	ByteBuffer buffer = fileSys->ReadAll(scene->GetSourcePath());
	YamlArchive archive(buffer);

	EWorld& world = scene->GetWorld();

	archive.BeginReadArray("entities");
	uint32 entityArrSize = archive.GetArraySize();
	for (uint32 i = 0; i < entityArrSize; i++)
	{
		String entityUUIDStr;
		archive.ReadString(entityUUIDStr);
		UniqueID128 entityUUID(entityUUIDStr);

		world.CreateEntity(entityUUID);

		archive.Next();
	}
	archive.EndReadArray();

	refl::ReflectionRegistry* rr = refl::ReflectionRegistry::Instance();

	archive.BeginReadObject("components");
	String componentKey;
	while (archive.ReadObjectKey(componentKey))
	{
		UniqueID128 componentUUID(componentKey);
		const refl::Type* componentType = rr->GetOrNull(componentUUID);
		if (!componentType)
		{
			LOG_WARNING("Tried to get a component from scene file that does not exist anymore");
			continue;
		}

		archive.BeginReadArray(componentKey);
		uint32 componentArrSize = archive.GetArraySize();
		for (uint32 i = 0; i < componentArrSize; i++)
		{
			archive.BeginReadObject();

			String entityUUIDStr;
			archive.ReadString(entityUUIDStr, "entity");
			EEntity entity = UniqueID128(entityUUIDStr);

			void* componentData = world.AddComponent(entity, componentType->uuid);

			componentType->functions.loadFunction(componentData, archive, "");

			archive.EndReadObject();

			archive.Next();
		}
		archive.EndReadArray();
	}

	return true;
}