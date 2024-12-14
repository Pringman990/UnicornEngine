#pragma once

#include <fbxsdk.h>

namespace FBX
{
	enum class ImportWarning : uint8
	{
		TooManySkinWeights,
		NoUVCoordinates,
		GenerateNormalsFailed,
		CannotTriangulate,
		DuplicateBoneNames,
		FailedCast,
		Unknown,
	};

	enum class ImportResult : uint8
	{
		Success,
		SuccessWithWarnings,
		InternalError,
		ImporterNotInitialized,
		SceneNotInitialized,
		FileNotFound,
		NoSkeletonData,
		NoAnimationData,
		NoMeshData,
		
	};

	struct ImportStatus
	{
		using WarningMessage = std::string;

		std::vector<std::pair<ImportWarning, WarningMessage>> warnings;
		WarningMessage message = "";
		std::string filePath = "";
		ImportResult result = ImportResult::Success;

		void AddWarning(ImportWarning aWarning, const WarningMessage& aMessage)
		{
			warnings.push_back({aWarning, aMessage});
		}

		operator bool() const 
		{
			return result == ImportResult::Success;
		}
	};

	enum class SystemUnit : uint8
	{
		Millimeter,
		Centimeter,
		Decimeter,
		Meter,
		Inch,
		Foot,
		Mile,
		Yard
	};

	enum class Axis : uint8
	{
		X,
		Y,
		Z
	};

	struct FileInfo
	{
		std::string ApplicationName;
		std::string ApplicationVersion;

		SystemUnit originalSystemUnit;
		Axis originalUpAxis;
	};

	struct Texture
	{
		std::string name;
		std::string path;
		std::string relativePath;
	};

	struct Material
	{
		std::string name;
		//Texture emissive;
		//Texture ambient;
		Texture diffuse;
		FbxDouble3 diffuseColor;
		//Texture specular;
		//Texture shininess;
		//Texture bump;
		//Texture normalMap;
		//Texture transparentColor;
		//Texture reflection;
		//Texture displacement;
		//Texture vectorDisplacement;
	};

	struct Vertex
	{
		FbxVector4 position;
		FbxVector2 uv;

		FbxVector4 normal;
		FbxVector4 tangent;
		FbxVector4 biNormal;
	};

	struct Mesh
	{
		struct SubMesh
		{
			std::vector<Vertex> vertices;
			std::vector<uint32> indices;

			FBX::Material material;
			std::string name;
		};

		FBX::FileInfo fileInfo;

		std::vector<SubMesh> subMeshes;
		std::string name;
	};

	class Importer
	{
	public:
		ImportStatus LoadMeshA(const std::string& aFilePath, FBX::Mesh& aOutMesh);

	private:
		void ProcessFBXNode(FbxNode* aNode, std::vector<FBX::Mesh>& someMeshes);
		void ProcessFBXMesh(FbxMesh* aFbxMesh, FBX::Mesh& aOutMesh);
		void ProcessFBXMaterial(FbxSurfaceMaterial* fbxMaterial, FBX::Material& material);
	private:
		ImportStatus mImportStatus;
	};
}