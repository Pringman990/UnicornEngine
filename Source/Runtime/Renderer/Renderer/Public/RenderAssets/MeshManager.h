#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Assets/IAssetLoader.h"
#include "Mesh.h"

class MeshManager : public IAssetLoader
{
public:
	MeshManager();
	~MeshManager() override;

	virtual Mesh* Load(const String& VirtualPath) override;
	virtual Mesh* ImportSource(const String& VirtualSourcePath) override;
	virtual Mesh* ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID) override;
	virtual bool UnLoad(const String& VirtualPath) override;
	
	virtual bool SupportsExtension(const String& Extension) override
	{
		return IsAnyOf(Extension, "fbx");
	}

private:
};