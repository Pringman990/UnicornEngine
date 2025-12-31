#pragma once
#include <GameCoreDefines.h>
#include <EWorld.h>

#include <RenderAssets/Mesh.h>
#include <Assets/AssetRef.h>
#include <Renderer.h>

class SceneView
{
public:
	SceneView() = default;
	~SceneView() = default;

	void SubmitMesh(EEntity Entity, AssetRef<Mesh> Mesh, const Transform& ObjectTransfrom)
	{
		RenderInstance instance;
		instance.mesh = Mesh;
		instance.transform = ObjectTransfrom;
		instance.renderID = (uint32)mRenderIDToEntity.size();
		mRenderIDToEntity.push_back(Entity);
		mSubmissions.push_back(instance);
	}

	void Flush()
	{
		for (auto& instance : mSubmissions)
		{
			Renderer::Instance()->SubmitMesh(instance.mesh, instance.transform, instance.renderID);
		}

		mSubmissions.clear();
	}

	void Reset() 
	{
		mRenderIDToEntity.clear();
		mRenderIDToEntity.push_back(UniqueID128::Invalid());
	}

	EEntity ResolveEntity(uint32 RenderID)
	{
		if (RenderID >= mRenderIDToEntity.size())
			return EEntity::Invalid();

		return mRenderIDToEntity[RenderID];
	}

private:
	Vector<EEntity> mRenderIDToEntity;

	struct RenderInstance
	{
		AssetRef<Mesh> mesh;
		Transform transform;
		uint32 renderID;
	};

	Vector<RenderInstance> mSubmissions;
};

class Scene
{
public:
	GAMECORE_API Scene();
	GAMECORE_API Scene(class SceneManager* Manager, const UniqueID128& UUID);
	GAMECORE_API Scene(class SceneManager* Manager, const UniqueID128& UUID, const String& Name);
	GAMECORE_API Scene(class SceneManager* Manager, const UniqueID128& UUID, const String& Name, const Path& SourcePath);
	GAMECORE_API ~Scene();

	/**
	* Happens once when the scene loads
	*/
	GAMECORE_API void OnLoad();

	GAMECORE_API void OnUpdate();

	GAMECORE_API void Unload();

	/**
	* Won't clone uuid or name.
	*/
	GAMECORE_API Scene Clone();

	EWorld& GetWorld() { return mEWorld; };
	
	const UniqueID128& GetUUID() const { return mUUID; }
	
	const String& GetName() const { return mName; }
	void SetName(const String& Name) { mName = Name; }
	
	const String& GetSourcePath() const { return mSourcePath; }
	void SetSourcePath(const Path& SourcePath) { mSourcePath = SourcePath; }

	SceneView& GetSceneView() { return mSceneView; }

private:
	friend class SceneManager;
	class SceneManager* mManager;

	UniqueID128 mUUID;
	EWorld mEWorld;
	String mName;
	Path mSourcePath;

	SceneView mSceneView;
};