#pragma once
#include <RenderAssets/Mesh.h>

struct EStaticMesh
{
	AssetRef<Mesh> mesh;
};

REFL_DECLARE_COMPONENT(EStaticMesh);