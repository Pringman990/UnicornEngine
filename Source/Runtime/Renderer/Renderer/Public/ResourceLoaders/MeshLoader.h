#pragma once
#include <Core.h>

#include <AssetManager/AssetLoader.h>
#include "Mesh.h"

class MeshLoader : public AssetLoader<Mesh>
{
public:
	MeshLoader();
	~MeshLoader();

private:

};