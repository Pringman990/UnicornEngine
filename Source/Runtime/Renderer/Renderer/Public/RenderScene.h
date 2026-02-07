#pragma once
#include "RendererDefines.h"
#include "RenderAssets/Mesh.h"
#include <Math/Transform.h>

struct MeshInstance
{
	AssetRef<Mesh> mesh;
	Transform transform;
};

class RenderScene
{
public:
	RENDERER_API RenderScene();
	RENDERER_API ~RenderScene();

	RENDERER_API void Reset();

	RENDERER_API void SubmitMesh(AssetRef<Mesh> Ref, Transform MeshTranform);

	RENDERER_API const Vector<MeshInstance>& GetMeshes() { return mMeshes; };

private:
	Vector<MeshInstance> mMeshes;
};