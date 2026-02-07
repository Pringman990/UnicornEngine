#include "RenderScene.h"

RenderScene::RenderScene()
{
}

RenderScene::~RenderScene()
{
}

void RenderScene::Reset()
{
	mMeshes.clear();
}

void RenderScene::SubmitMesh(AssetRef<Mesh> Ref, Transform MeshTranform)
{
	MeshInstance instance;
	instance.mesh = Ref;
	instance.transform = MeshTranform;
	mMeshes.push_back(std::move(instance));
}
