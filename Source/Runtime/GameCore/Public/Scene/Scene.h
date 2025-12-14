#pragma once
#include <GameCoreDefines.h>
#include <EWorld.h>

class Scene
{
public:
	GAMECORE_API Scene();
	GAMECORE_API Scene(const UniqueID128& UUID);
	GAMECORE_API Scene(const UniqueID128& UUID, const String& Name);
	GAMECORE_API Scene(const UniqueID128& UUID, const String& Name, const Path& SourcePath);
	GAMECORE_API ~Scene();

	/**
	* Happens once when the scene loads
	*/
	GAMECORE_API void OnLoad();

	GAMECORE_API void OnUpdate();

	GAMECORE_API void Unload();

	EWorld& GetWorld() { return mEWorld; };
	
	const UniqueID128& GetUUID() const { return mUUID; }
	
	const String& GetName() const { return mName; }
	void SetName(const String& Name) { mName = Name; }
	
	const String& GetSourcePath() const { return mSourcePath; }
	void SetSourcePath(const Path& SourcePath) { mSourcePath = SourcePath; }

private:
	UniqueID128 mUUID;
	EWorld mEWorld;
	String mName;
	Path mSourcePath;
};