#include "pch.h"
#include "FBXImporter.h"



#include <filesystem>

namespace FBX
{
	//TODO: maybe change it to a class instead
	namespace {
		class Internal
		{
		public:
			static Internal& Get()
			{
				static Internal instance;
				return instance;
			}

			void Destroy()
			{
				mFbxManager->Destroy();
				mFbxManager = nullptr;

				mFbxIOSettings->Destroy();
				mFbxIOSettings = nullptr;

				delete mFbxConverter;
				mFbxConverter = nullptr;
			}
		private:
			Internal()
				:
				mFbxManager(FbxManager::Create()),
				mFbxIOSettings(FbxIOSettings::Create(mFbxManager, "Unicorn IO")),
				mFbxConverter(new FbxGeometryConverter(mFbxManager)),
				mFbxAxisSystem(FbxAxisSystem::eDirectX)
			{
				mFbxManager->SetIOSettings(mFbxIOSettings);
			}

			~Internal() {}

		public:
			FbxManager* mFbxManager;
			FbxIOSettings* mFbxIOSettings;
			FbxGeometryConverter* mFbxConverter;
			FbxAxisSystem mFbxAxisSystem;
		};
	}

	ImportStatus Importer::LoadMeshA(const std::string& aFilePath, FBX::Mesh& /*aOutMesh*/)
	{
		mImportStatus.filePath = aFilePath;

		if (!std::filesystem::exists(aFilePath))
		{
			mImportStatus.result = ImportResult::FileNotFound;
			return mImportStatus;
		}

		//TODO: maybe change to the using thread?
		FbxImporter* fbxImporter = FbxImporter::Create(Internal::Get().mFbxManager, "Thread Main");
		if (!fbxImporter)
		{
			mImportStatus.result = ImportResult::ImporterNotInitialized;
			return mImportStatus;
		}

		if (!fbxImporter->Initialize(aFilePath.c_str(), -1, Internal::Get().mFbxIOSettings) || !fbxImporter->IsFBX())
		{
			const FbxStatus& fbxStatus = fbxImporter->GetStatus();
			mImportStatus.result = ImportResult::InternalError;
			mImportStatus.message = fbxStatus.GetErrorString();
			return mImportStatus;
		}

		FbxScene* fbxScene = FbxScene::Create(Internal::Get().mFbxManager, ("Scene: " + aFilePath).c_str());
		if (!fbxScene)
		{
			mImportStatus.result = ImportResult::SceneNotInitialized;
			return mImportStatus;
		}

		if (!fbxImporter->Import(fbxScene))
		{
			const FbxStatus& fbxStatus = fbxImporter->GetStatus();
			mImportStatus.result = ImportResult::InternalError;
			mImportStatus.message = fbxStatus.GetErrorString();
			fbxImporter->Destroy();
			return mImportStatus;
		}
		fbxImporter->Destroy();

		std::vector<FBX::Mesh> meshes;
		meshes.reserve(fbxScene->GetMaterialCount());

		FbxNode* rootNode = fbxScene->GetRootNode();
		if (rootNode)
		{
			for (int32 i = 0; i < rootNode->GetChildCount(); i++)
			{
				ProcessFBXNode(rootNode->GetChild(i), meshes);
			}
		}

		return mImportStatus;
	}

	void Importer::ProcessFBXNode(FbxNode* aNode, std::vector<FBX::Mesh>& someMeshes)
	{
		FbxMesh* fbxMesh = aNode->GetMesh();
		if (fbxMesh)
		{
			FBX::Mesh mesh;
			ProcessFBXMesh(fbxMesh, mesh);
			someMeshes.emplace_back(mesh);
		}

		for (int32 i = 0; i < aNode->GetChildCount(); i++)
		{
			ProcessFBXNode(aNode->GetChild(i), someMeshes);
		}
	}

	void Importer::ProcessFBXMesh(FbxMesh* aFbxMesh, FBX::Mesh& aOutMesh)
	{
		std::vector<FBX::Material> materials;
		const int32 materialCount = aFbxMesh->GetNode()->GetMaterialCount();
		materials.resize(materialCount);

		for (int32 i = 0; i < materialCount; i++)
		{
			ProcessFBXMaterial(aFbxMesh->GetNode()->GetMaterial(i), materials[i]);
		}

		const int32 polygonCount = aFbxMesh->GetPolygonCount();
		for (int32 materialIndex = 0; materialIndex < materialCount; materialIndex++)
		{
			FBX::Mesh::SubMesh subMesh;
			subMesh.vertices.reserve(polygonCount * 3);
			subMesh.material = materials[materialIndex];

			for (int32 i = 0; i < polygonCount; i++)
			{
				if (aFbxMesh->GetElementMaterial())
				{
					const int32 polygonMaterialIndex = aFbxMesh->GetElementMaterial()->GetIndexArray().GetAt(i);
					if (polygonMaterialIndex != materialIndex)
						continue;
				}

				for (int32 j = 0; j < 3; j++) //Triangulated assumed
				{
					const int32 controlPointIndex = aFbxMesh->GetPolygonVertex(i, j);

					FBX::Vertex vertex;
					vertex.position = aFbxMesh->GetControlPoints()[controlPointIndex];

					FbxGeometryElementNormal* normalElement = aFbxMesh->GetElementNormal();
					if (normalElement) {
						const int32 normalIndex = (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							? controlPointIndex
							: normalElement->GetIndexArray().GetAt(i * 3 + j);
						vertex.normal = normalElement->GetDirectArray().GetAt(normalIndex);
					}

					FbxGeometryElementUV* uvElement = aFbxMesh->GetElementUV();
					if (uvElement) {
						const int32 uvIndex = (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							? controlPointIndex
							: uvElement->GetIndexArray().GetAt(i * 3 + j);
						vertex.uv = uvElement->GetDirectArray().GetAt(uvIndex);
					}

					subMesh.vertices.emplace_back(vertex);
					subMesh.indices.emplace_back(static_cast<uint32_t>(subMesh.vertices.size() - 1));
				}
			}
			aOutMesh.subMeshes.emplace_back(subMesh);
		}
	}

	void Importer::ProcessFBXMaterial(FbxSurfaceMaterial* fbxMaterial, FBX::Material& material)
	{
		material.name = fbxMaterial->GetName();

		// Check if the diffuse property is valid
		FbxProperty diffuseProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (diffuseProperty.IsValid()) {
			// Check if there's a texture assigned to the diffuse property
			if (FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>()) {
				// If a texture is assigned, use it
				material.diffuse.path = texture->GetFileName();      // Absolute path to texture
				material.diffuse.relativePath = texture->GetRelativeFileName(); // Relative path for asset management
			}
			else {
				// If no texture, but a color exists, use the color (diffuse color)
				FbxDouble3 color = diffuseProperty.Get<FbxDouble3>();
				material.diffuseColor = color;

				// Don't assign a default texture if a diffuse color is already present
				material.diffuse.path = ""; // No texture assigned
				material.diffuse.relativePath = "";
			}
		}
		else {
			// If no diffuse property exists, assign a default texture (e.g., white texture)
			//material.texturePath = "path/to/default/white_texture.png";
			//material.relativeTexturePath = "default/white_texture.png";
		}
	}
}
