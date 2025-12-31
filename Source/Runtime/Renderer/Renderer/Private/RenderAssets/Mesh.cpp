#include "RenderAssets/Mesh.h"

#include "RenderAssets/Material.h"

REFL_DEFINE(Mesh)
{
	refl::ClassBuilder<Mesh>(STRINGIFYEXP(Mesh), "653c9ab4-e3eb-4e39-89ee-a28f365ce91c");
}

REFL_DEFINE(AssetRef<Mesh>)
{
	refl::ClassBuilder<AssetRef<Mesh>>(STRINGIFYEXP(AssetRef<Mesh>), "663c9ab4-e3eb-4e39-89ee-a28f365ce91c")
		.SaveFunction([](void* obj, Archive& archive, String key)
			{
				SaveAssetRef<Mesh>(obj, archive, key);
			})
		.LoadFunction([](void* obj, Archive& archive, String key)
			{
				LoadAssetRef<Mesh>(obj, archive, key);
				return true;
			});
}

Mesh::Mesh() :
	AssetBase({})
{
}

Mesh::Mesh(UniqueID128 UUID)
	:
	AssetBase(UUID)
{
}

Mesh::~Mesh()
{
}

void Mesh::SetMaterial(uint32 SubmeshIndex, AssetRef<Material> Material)
{
	if (SubmeshIndex >= mSubmeshes.size() && SubmeshIndex < 0)
	{
		LOG_WARNING("Trying to set a submesh material with index larger or less then array size");
		return;
	}

	mSubmeshes[SubmeshIndex].material = Material;
}

AssetRef<Material> Mesh::GetMaterial(uint32 SubmeshIndex) const
{
	if (SubmeshIndex >= mSubmeshes.size() && SubmeshIndex < 0)
	{
		LOG_WARNING("Trying to set a submesh material with index larger or less then array size");
		return AssetRef<Material>::Invalid();
	}

	return mSubmeshes[SubmeshIndex].material;
}

void Mesh::FillSubmeshes(const Vector<Mesh::Submesh>& Submeshes)
{
	mSubmeshes.clear();
	for (auto sub : Submeshes)
	{
		mSubmeshes.push_back(sub);
	}
}
