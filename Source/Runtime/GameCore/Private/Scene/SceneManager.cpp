#include "Scene/SceneManager.h"
REGISTER_ENGINE_SUBSYSTEM(SceneManager)

#include <YAML/YamlHelper.h>
#include <Archive/YamlArchive.h>

SceneManager::SceneManager()
	:
	mActiveScene(nullptr)
{
}

SceneManager::~SceneManager()
{
}

GAMECORE_API void SceneManager::LoadAllSceneFiles()
{
	Path root = GET_FILESYSTEM()->GetAbsolutPath("engine://");
	for (auto& entry : std::filesystem::recursive_directory_iterator(root))
	{
		if (!entry.is_directory() && entry.path().extension() == ".scene")
		{
			auto relative = std::filesystem::relative(entry.path(), root);
			Path virtualPath = "engine://" + relative.generic_string();

			ByteBuffer buffer = GET_FILESYSTEM()->ReadAll(virtualPath);

			LoadSceneMetadataOnly(virtualPath);
		}
	}
}

Scene* SceneManager::CreateScene(const String& Name)
{
	return CreateScene(UniqueID128::FromRandom(), Name);
}

GAMECORE_API Scene* SceneManager::CreateScene(const UniqueID128& UUID, const String& Name)
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

Scene* SceneManager::GetScene(const String& Name)
{
	return GetSceneFromName(Name);
}

void SceneManager::SetActiveScene(const String& Name)
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

bool SceneManager::SaveActiveSceneToFile(const Path& VirtualPath)
{
	return SaveSceneToFile(mActiveScene, VirtualPath);	
}

bool SceneManager::SaveActiveScene()
{
	if (mActiveScene->GetName() == "Default")
		return true;

	if (mActiveScene->GetSourcePath().empty())
	{
		LOG_ERROR("Couldn't save active scene as it does not have a source path, use SaveActiveSceneToFile");
		return false;
	}

	return SaveSceneToFile(mActiveScene, mActiveScene->GetSourcePath());
}

Scene* SceneManager::GetSceneFromName(const String& Name)
{
	Scene* scene = Exists(Name);
	if (!scene)
	{
		LOG_WARNING("Can't get scene with name '{}'", Name);
		return nullptr;
	}

	return scene;
}

GAMECORE_API Scene* SceneManager::Exists(const String& Name)
{
	auto it = mNameToUUID.find(Name);
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

GAMECORE_API bool SceneManager::SaveSceneToFile(Scene* ScenePtr, const Path& VirtualPath)
{
	FileSystem* fileSys = SubsystemManager::Get<FileSystem>();
	if (!fileSys->Exists(VirtualPath))
	{
		LOG_ERROR("Filed to save active scene to file path '{}', because it doesn't exist", VirtualPath);
		return false;
	}

	if (!ScenePtr)
	{
		LOG_ERROR("Can't save the active scene as its null??");
		return false;
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

	refl::ReflectionRegistry& rr = refl::ReflectionRegistry::GetInstance();
	auto& componentStores = world.ComponentAllStores();

	archive.BeginWriteObject("components");
	for (auto& [componentUUID, store] : componentStores)
	{
		const refl::Type* type = rr.GetOrNull(componentUUID);
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

	archive.WriteToFile(VirtualPath);
	return true;
}

GAMECORE_API bool SceneManager::LoadScene(const String& Name)
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

GAMECORE_API void SceneManager::UnloadScene(const String& Name)
{
	Scene* scene = GetSceneFromName(Name);
	if (!scene)
	{
		LOG_ERROR("Failed to unload scene because it doesn't exist, '{}'", Name);
		return;
	}

	scene->Unload();
}

GAMECORE_API UniqueID128 SceneManager::LoadSceneMetadataOnly(const Path& VirtualPath)
{
	FileSystem* fileSys = SubsystemManager::Get<FileSystem>();
	if (!fileSys->Exists(VirtualPath))
	{
		LOG_ERROR("Failed to load scene to file path '{}', because it doesn't exist", VirtualPath);
		return UniqueID128::Invalid();
	}

	ByteBuffer buffer = fileSys->ReadAll(VirtualPath);
	YamlArchive archive(buffer);

	String uuidStr;
	archive.ReadString(uuidStr, "uuid");
	UniqueID128 uuid(uuidStr);

	String name;
	archive.ReadString(name, "name");

	Scene* scene = CreateScene(uuid, name);
	scene->SetSourcePath(VirtualPath);

	return uuid;
}

GAMECORE_API bool SceneManager::LoadSceneFromFile(const Path& VirtualPath)
{
	FileSystem* fileSys = SubsystemManager::Get<FileSystem>();
	if (!fileSys->Exists(VirtualPath))
	{
		LOG_ERROR("Filed to load scene from file path '{}', because it doesn't exist", VirtualPath);
		return false;
	}

	UniqueID128 uuid = LoadSceneMetadataOnly(VirtualPath);
	if (!uuid.IsValid())
	{
		LOG_ERROR("Failed to get scene uuid, path '{}'", VirtualPath);
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

	FileSystem* fileSys = SubsystemManager::Get<FileSystem>();
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

	refl::ReflectionRegistry& rr = refl::ReflectionRegistry::GetInstance();

	archive.BeginReadObject("components");
	String componentKey;
	while (archive.ReadObjectKey(componentKey))
	{
		UniqueID128 componentUUID(componentKey);
		const refl::Type* componentType = rr.GetOrNull(componentUUID);
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