﻿#include "TGAFbx.pch.h"
#include "Importer.h"

#include <cassert>
#include <filesystem>

#include "Internals.h"
#include <unordered_map>

namespace TGA
{
	namespace FBX
	{
		struct VertexDuplicateData;

		// Represents the static data that the FBX SDK requires, for consecutive calls to functions in the Importer.
		struct FbxEnvironment
		{
		private:
			FbxEnvironment() : fbxConverter()
			{
			}

		public:

			static FbxEnvironment& Get() { static FbxEnvironment theInstance;  return theInstance; }

			FbxManager* fbxManager = nullptr;
			FbxIOSettings* fbxIOSettings = nullptr;
			FbxGeometryConverter* fbxConverter;
			FbxAxisSystem fbxAxisSystem;
			FbxImporter* fbxImporter = nullptr;
			std::string lastFbxStatus;

			void Init()
			{
				if (!fbxManager)
				{
					fbxManager = FbxManager::Create();
					fbxIOSettings = FbxIOSettings::Create(fbxManager, "TGA");

					fbxIOSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
					fbxIOSettings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, false);

					fbxImporter = FbxImporter::Create(fbxManager, "");
					fbxConverter = new FbxGeometryConverter(fbxManager);
					fbxAxisSystem = FbxAxisSystem::eDirectX;
				}
			}

			void Destroy()
			{
				if (fbxManager)
				{
					fbxManager->Destroy();
			
					delete fbxConverter;
	
					fbxConverter = nullptr;
					fbxManager = nullptr;
					fbxIOSettings = nullptr;
					fbxImporter = nullptr;

					lastFbxStatus.~basic_string();
				}
			}
		};

		FbxImportStatus Importer::LoadMeshW(const std::wstring& someFilePath, Mesh& outMesh, bool bRegenerateNormals, bool bMergeDuplicateVertices)
		{
			const std::string ansiFileName = Internals::WideStringtoAnsi(someFilePath);

			return LoadMeshA(ansiFileName, outMesh, bRegenerateNormals, bMergeDuplicateVertices);
		}

		FbxImportStatus Importer::LoadMeshA(const std::string& someFilePath, Mesh& outMesh, bool bRegenerateNormals, bool bMergeDuplicateVertices)
		{
			FbxImportStatus importStatus;

			if (!std::filesystem::exists(someFilePath))
			{
				importStatus.Result = FbxImportResult::FileNotFound;
				return importStatus;
			}

			FbxImporter* fbxImporter = FbxImporter::Create(FbxEnvironment::Get().fbxManager, "");

			if (!fbxImporter)
			{
				importStatus.Result = FbxImportResult::NotInitialized;
				return importStatus;
			}

			if (!fbxImporter->Initialize(someFilePath.c_str(), -1, FbxEnvironment::Get().fbxIOSettings) || !fbxImporter->IsFBX())
			{
				const FbxStatus& fbxStatus = fbxImporter->GetStatus();
				FbxEnvironment::Get().lastFbxStatus = fbxStatus.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
				return importStatus;
			}

			FbxScene* fbxScene = FbxScene::Create(FbxEnvironment::Get().fbxManager, "The Scene");

			if (fbxImporter->Import(fbxScene))
			{
				const FbxDocumentInfo* info = fbxImporter->GetSceneInfo();
				const FbxString applicationName = info->LastSaved_ApplicationName.Get();
				const FbxString applicationVersion = info->LastSaved_ApplicationVersion.Get();
				const FbxAxisSystem originalAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
				// 0 is X, 1 is Y, 2 is Z axis.
				const int originalUpAxis = fbxScene->GetGlobalSettings().GetOriginalUpAxis();
				const FbxSystemUnit originalSystemUnit = fbxScene->GetGlobalSettings().GetOriginalSystemUnit();

				outMesh.FileInfo.Application = applicationName;
				outMesh.FileInfo.ApplicationVersion = applicationVersion;

				if (originalSystemUnit == FbxSystemUnit::mm)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Millimeter;
				else if (originalSystemUnit == FbxSystemUnit::cm)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Centimeter;
				else if (originalSystemUnit == FbxSystemUnit::dm)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Decimeter;
				else if (originalSystemUnit == FbxSystemUnit::m)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Meter;
				else if (originalSystemUnit == FbxSystemUnit::Inch)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Inch;
				else if (originalSystemUnit == FbxSystemUnit::Foot)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Foot;
				else if (originalSystemUnit == FbxSystemUnit::Mile)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Mile;
				else if (originalSystemUnit == FbxSystemUnit::Yard)
					outMesh.FileInfo.OriginalSystemUnit = SystemUnit::Yard;

				outMesh.FileInfo.OriginalUpAxis = originalUpAxis == 0 ? Axis::X : (originalUpAxis == 1 ? Axis::Y : Axis::Z);

				if (fbxScene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::cm)
				{
					constexpr FbxSystemUnit::ConversionOptions sysUnitConversion =
					{
						false,
						true,
						true,
						true,
						true,
						true
					};

					FbxSystemUnit::cm.ConvertScene(fbxScene, sysUnitConversion);
					assert(fbxScene->GetGlobalSettings().GetSystemUnit() == FbxSystemUnit::cm);
				}

				// Load and set up Material data
				// This is flagged as Internal and might not be needed?
				fbxScene->ConnectMaterials();

				// Triangulate the scene and replace models with triangulated versions.
				if (!FbxEnvironment::Get().fbxConverter->Triangulate(fbxScene, true, true))
				{
					// Failed to use the faster legacy method so try to use the new method
					FbxEnvironment::Get().fbxConverter->Triangulate(fbxScene, true, false);
				}

				std::vector<Material> mdlMaterials;
				// Load all materials in the scene.
				const int numSceneMaterials = fbxScene->GetMaterialCount();
				mdlMaterials.resize(numSceneMaterials);

				for (int m = 0; m < numSceneMaterials; m++)
				{
					FbxSurfaceMaterial* fbxMaterial = fbxScene->GetMaterial(m);

					Internals::GetMaterialData(fbxMaterial, mdlMaterials[m]);
				}

				std::vector<FbxNode*> sceneMeshNodes;
				sceneMeshNodes.reserve(numSceneMaterials);

				Internals::GatherMeshNodes(fbxScene->GetRootNode(), sceneMeshNodes);

				std::vector<FbxNode*> sceneLODGroups;
				sceneLODGroups.reserve(numSceneMaterials);

				Internals::GatherLODGroups(fbxScene->GetRootNode(), sceneLODGroups);

				// We need to make sure all models have proper BiNormal / Tangent data.
				for (FbxNode* meshNode : sceneMeshNodes)
				{
					FbxMesh* fbxMesh = meshNode->GetMesh();

					const int numUvLayers = fbxMesh->GetUVLayerCount();
					if (numUvLayers == 0)
					{
						importStatus.AddWarning(FbxImportWarning::NoUVCoordinates);
						fbxMesh->InitTextureUV(1);
					}

					if (fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0 || bRegenerateNormals)
					{
						if (!fbxMesh->GenerateTangentsData(0, false, false))
						{
							importStatus.AddWarning(FbxImportWarning::GenerateNormalsFailed);
						}
					}
				}

				// This has to happen after ALL changes have been made to the scene. I.e. you need to
				// run Triangulate and generate missing Tangents and BiNormals before you run this or
				// generated things will point in the wrong direction.
				FbxEnvironment::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

				// Now we can fetch things from the scene and convert to TGA format.
				std::vector<Mesh::LODGroup> mdlLodGroups;

				Skeleton mdlSkeleton;
				std::vector<Skeleton::Bone> skeletonEventBones;
				std::vector<Skeleton::Socket> skeletonSockets;
				const bool fbxHasBones = Internals::GatherSkeleton(fbxScene->GetRootNode(), importStatus, mdlSkeleton, skeletonEventBones, skeletonSockets);
				if (fbxHasBones)
				{
					// Try to extract a sensible skeleton name.
					if (const Skeleton::Bone* rootBone = mdlSkeleton.GetRoot())
					{
						const std::string rootBoneName = rootBone->NamespaceName;
						if (const size_t pos = rootBoneName.find_first_of(':'); pos != std::string::npos)
						{
							mdlSkeleton.Name = rootBoneName.substr(0, pos);
						}
						else
						{
							// We have no namespace to use so we'll just use the model name.
							std::string filePathCopy = someFilePath;
							std::replace(filePathCopy.begin(), filePathCopy.end(), '/', '\\');
							if (const size_t slashPos = filePathCopy.find_last_of('\\'); slashPos != std::string::npos)
							{
								filePathCopy = filePathCopy.substr(slashPos + 1, filePathCopy.length() - (slashPos - 1));
							}
							mdlSkeleton.Name = filePathCopy.substr(0, filePathCopy.length() - 4);
						}
					}


					for (Skeleton::Socket& socket : skeletonSockets)
					{
						mdlSkeleton.Sockets.insert({ socket.Name, socket });
					}
				}

				BoxSphereBounds mdlBounds;

				// We need a definitive list of meshes that we can look at. Some meshes may be in LOD groups.
				for (FbxNode* lodGroupNode : sceneLODGroups)
				{
					Mesh::LODGroup lodGroup;

					// Go through all LOD groups and process meshes found there, and make LOD structs of them.
					// Remove the node from the sceneMeshNodes list.
					// Any meshes left in that list need to be handled separately.
					FbxLODGroup* fbxLodGroup = lodGroupNode->GetLodGroup();
					const int numLodLevels = lodGroupNode->GetChildCount();

					for (int lodLv = 0; lodLv < numLodLevels; lodLv++)
					{
						Mesh::LODGroup::LODLevel lodLevel;
						lodLevel.Level = lodLv;

						FbxDistance lodDistance;
						if (lodLv != 0)
						{
							fbxLodGroup->GetThreshold(lodLv - 1, lodDistance);
						}

						FbxNode* lodLevelRoot = lodGroupNode->GetChild(lodLv);

						lodLevel.Distance = lodDistance.value();

						std::vector<FbxNode*> lodLevelMeshNodes;
						lodLevelMeshNodes.reserve(numSceneMaterials);

						const size_t numLodLevelMeshes = Internals::GatherMeshNodes(lodLevelRoot, lodLevelMeshNodes);
						lodLevel.Elements.reserve(numLodLevelMeshes);
						for (FbxNode* lodLevelMeshNode : lodLevelMeshNodes)
						{
							for (auto it = sceneMeshNodes.begin(); it != sceneMeshNodes.end(); ++it)
							{
								if ((*it)->GetUniqueID() == lodLevelMeshNode->GetUniqueID())
								{
									sceneMeshNodes.erase(it);
									break;
								}
							}

							// Convert the mesh here.
							std::vector<Mesh::Element> lodLevelNodeMeshes;
							Internals::FbxMeshToMesh(fbxScene, lodLevelMeshNode, mdlSkeleton, importStatus, lodLevelNodeMeshes, bMergeDuplicateVertices);

							for (auto meshIt = lodLevelNodeMeshes.begin(); meshIt != lodLevelNodeMeshes.end(); ++meshIt)
							{
								const BoxSphereBounds& bounds = meshIt->BoxSphereBounds;
								mdlBounds = mdlBounds + bounds;
								lodLevel.Elements.push_back(*meshIt);
							}
						}

						lodGroup.Levels.push_back(lodLevel);
					}

					mdlLodGroups.push_back(lodGroup);
				}

				// Handle any meshes not part of a LOD group
				std::vector<Mesh::Element> mdlMeshes;

				for (FbxNode* meshNode : sceneMeshNodes)
				{
					std::vector<Mesh::Element> nodeMeshes;
					Internals::FbxMeshToMesh(fbxScene, meshNode, mdlSkeleton, importStatus, nodeMeshes, bMergeDuplicateVertices);

					for (auto meshIt = nodeMeshes.begin(); meshIt != nodeMeshes.end(); ++meshIt)
					{
						const BoxSphereBounds& bounds = meshIt->BoxSphereBounds;
						mdlBounds = mdlBounds + bounds;
					}

					mdlMeshes.insert(mdlMeshes.end(), nodeMeshes.begin(), nodeMeshes.end());
				}

				if (!mdlMeshes.empty() || !mdlLodGroups.empty())
				{
					outMesh.Name = someFilePath;
					outMesh.Elements = std::move(mdlMeshes);
					outMesh.LODGroups = std::move(mdlLodGroups);

					outMesh.Skeleton = std::move(mdlSkeleton);

					outMesh.Materials = std::move(mdlMaterials);
					outMesh.BoxSphereBounds = mdlBounds;
					outMesh.BoxBounds = Box::FromAABB(mdlBounds.Center, mdlBounds.BoxExtents);
				}
			}

			importStatus.Result = importStatus.Warnings.empty() ? FbxImportResult::Success : FbxImportResult::SuccessWithWarnings;

			const FbxStatus& Status = fbxImporter->GetStatus();
			// Status will be deleted by fbxImporter->Destroy();

			if (Status.Error())
			{
				FbxEnvironment::Get().lastFbxStatus = Status.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
			}

			fbxScene->Destroy(false);
			fbxImporter->Destroy();

			if (outMesh.Elements.empty() && outMesh.LODGroups.empty())
				importStatus.Result = FbxImportResult::NoMeshData;

			return importStatus;
		}

		FbxImportStatus Importer::LoadAnimationA(const std::string& someFilePath, Animation& outAnimation)
		{
			FbxImportStatus importStatus;

			if (!std::filesystem::exists(someFilePath))
			{
				importStatus.Result = FbxImportResult::FileNotFound;
				return importStatus;
			}

			FbxImporter* fbxImporter = FbxImporter::Create(FbxEnvironment::Get().fbxManager, "");
			if (!fbxImporter)
			{
				importStatus.Result = FbxImportResult::NotInitialized;
				return importStatus;
			}

			if (!fbxImporter->Initialize(someFilePath.c_str(), -1, FbxEnvironment::Get().fbxIOSettings) || !fbxImporter->IsFBX())
			{
				const FbxStatus& fbxStatus = fbxImporter->GetStatus();
				FbxEnvironment::Get().lastFbxStatus = fbxStatus.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
				return importStatus;
			}

			FbxScene* fbxScene = FbxScene::Create(FbxEnvironment::Get().fbxManager, "The Scene");

			if (fbxImporter->Import(fbxScene))
			{
				const FbxDocumentInfo* info = fbxImporter->GetSceneInfo();
				const FbxString applicationName = info->LastSaved_ApplicationName.Get();
				const FbxString applicationVersion = info->LastSaved_ApplicationVersion.Get();
				const FbxAxisSystem originalAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
				// 0 is X, 1 is Y, 2 is Z axis.
				const int originalUpAxis = fbxScene->GetGlobalSettings().GetOriginalUpAxis();
				const FbxSystemUnit originalSystemUnit = fbxScene->GetGlobalSettings().GetOriginalSystemUnit();

				outAnimation.FileInfo.Application = applicationName;
				outAnimation.FileInfo.ApplicationVersion = applicationVersion;

				if (originalSystemUnit == FbxSystemUnit::mm)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Millimeter;
				else if (originalSystemUnit == FbxSystemUnit::cm)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Centimeter;
				else if (originalSystemUnit == FbxSystemUnit::dm)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Decimeter;
				else if (originalSystemUnit == FbxSystemUnit::m)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Meter;
				else if (originalSystemUnit == FbxSystemUnit::Inch)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Inch;
				else if (originalSystemUnit == FbxSystemUnit::Foot)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Foot;
				else if (originalSystemUnit == FbxSystemUnit::Mile)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Mile;
				else if (originalSystemUnit == FbxSystemUnit::Yard)
					outAnimation.FileInfo.OriginalSystemUnit = SystemUnit::Yard;

				outAnimation.FileInfo.OriginalUpAxis = originalUpAxis == 0 ? Axis::X : (originalUpAxis == 1 ? Axis::Y : Axis::Z);

				FbxEnvironment::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

				Skeleton mdlSkeleton;
				std::vector<Skeleton::Bone> skeletonEventBones;
				std::vector<Skeleton::Socket> skeletonSockets;
				const bool fbxHasBones = Internals::GatherSkeleton(fbxScene->GetRootNode(), importStatus, mdlSkeleton, skeletonEventBones, skeletonSockets);
				if (!fbxHasBones)
				{
					importStatus.Result = FbxImportResult::NoSkeletonData;
					return importStatus;
				}

				// Register all events that exist for this animation.
				for (const Skeleton::Bone& EventBone : skeletonEventBones)
				{
					outAnimation.EventNames.push_back(EventBone.NamespaceName);
				}

				const Skeleton::Bone* rootBone = mdlSkeleton.GetRoot();
				const FbxNode* skeletonRoot = fbxScene->FindNodeByName(FbxString(rootBone->NamespaceName.c_str()));
				const FbxVector4 fbxTranslation = skeletonRoot->GetGeometricTranslation(::FbxNode::eSourcePivot);
				const FbxVector4 fbxRotation = skeletonRoot->GetGeometricRotation(::FbxNode::eSourcePivot);
				const FbxVector4 fbxScale = skeletonRoot->GetGeometricScaling(::FbxNode::eSourcePivot);
				const FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

				const int animStackCount = fbxImporter->GetAnimStackCount();
				if (animStackCount == 0)
				{
					importStatus.Result = FbxImportResult::NoAnimationData;
					return importStatus;
				}

				const FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(0);

				FbxString fbxAnimStackName = fbxAnimStack->GetName();
				std::string fbxAnimationName = fbxAnimStackName.Buffer();

				const FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName);

				const FbxTime::EMode fbxGlobalTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
				const double fbxFramesPerSecond = FbxTime::GetFrameRate(fbxGlobalTimeMode);
				const FbxTime fbxAnimStart = fbxTakeInfo->mLocalTimeSpan.GetStart();
				const FbxTime fbxAnimEnd = fbxTakeInfo->mLocalTimeSpan.GetStop();

				// The FBX animation may not be frames 0...N and can start at arbitrary places.
				const double fbxAnimTimeStart = fbxAnimStart.GetSecondDouble();
				const double fbxAnimTimeEnd = fbxAnimEnd.GetSecondDouble();
				const double fbxAnimTimeDuration = fbxAnimTimeEnd - fbxAnimTimeStart;

				const FbxLongLong startFrame = fbxAnimStart.GetFrameCount(fbxGlobalTimeMode);
				const FbxLongLong endFrame = fbxAnimEnd.GetFrameCount(fbxGlobalTimeMode);
				const FbxLongLong animTime = endFrame - startFrame + 1;

				outAnimation.Name = fbxAnimationName;
				outAnimation.Length = static_cast<unsigned int>(animTime);
				outAnimation.FramesPerSecond = static_cast<float>(fbxFramesPerSecond);
				outAnimation.Frames.resize(animTime);
				outAnimation.Duration = fbxAnimTimeDuration;

				size_t localFrameCounter = 0;

				for (FbxLongLong t = startFrame; t <= endFrame; t++)
				{
					FbxTime currentTime;
					currentTime.SetFrame(t, fbxGlobalTimeMode);

					outAnimation.Frames[localFrameCounter].LocalTransforms.reserve(mdlSkeleton.Bones.size());
					outAnimation.Frames[localFrameCounter].GlobalTransforms.reserve(mdlSkeleton.Bones.size());

					for (Skeleton::Bone& Bone : mdlSkeleton.Bones)
					{
						FbxAMatrix localFrameTransform;
						FbxAMatrix globalFrameTransform;

						if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(Bone.NamespaceName.c_str())))
						{
							localFrameTransform = jointNode->EvaluateLocalTransform(currentTime);
							globalFrameTransform = jointNode->EvaluateGlobalTransform(currentTime);
						}

						Matrix LocalTransform;
						Matrix GlobalTransform;

						Internals::FBXMatrixToArray(localFrameTransform, LocalTransform.Data);
						Internals::FBXMatrixToArray(globalFrameTransform, GlobalTransform.Data);

						outAnimation.Frames[localFrameCounter].LocalTransforms.insert({ Bone.Name, LocalTransform });
						outAnimation.Frames[localFrameCounter].GlobalTransforms.insert({ Bone.Name, GlobalTransform });
					}

					for (Skeleton::Socket& socket : skeletonSockets)
					{
						FbxAMatrix globalFrameTransform;

						if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(socket.Name.c_str())))
						{
							globalFrameTransform = jointNode->EvaluateGlobalTransform(currentTime);
						}

						Matrix GlobalTransform;

						Internals::FBXMatrixToArray(globalFrameTransform, GlobalTransform.Data);

						outAnimation.Frames[localFrameCounter].SocketTransforms.insert({ socket.Name, GlobalTransform });
					}

					for (Skeleton::Bone& EventBone : skeletonEventBones)
					{
						if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(EventBone.NamespaceName.c_str())))
						{
							FbxAMatrix localFrameTransform = jointNode->EvaluateLocalTransform(currentTime);

							Matrix LocalTransform;
							Internals::FBXMatrixToArray(localFrameTransform, LocalTransform.Data);
							const float eventMagnitude = std::powf(LocalTransform[12] + LocalTransform[13] + LocalTransform[14], 2);
							if (eventMagnitude > 0)
							{
								outAnimation.Frames[localFrameCounter].TriggeredEvents.insert({ EventBone.Name, true });
							}
						}
					}

					localFrameCounter++;
				}
			}

			importStatus.Result = importStatus.Warnings.empty() ? FbxImportResult::Success : FbxImportResult::SuccessWithWarnings;
			const FbxStatus& Status = fbxImporter->GetStatus();
			// Status will be deleted by fbxImporter->Destroy();

			if (Status.Error())
			{
				FbxEnvironment::Get().lastFbxStatus = Status.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
			}

			fbxScene->Destroy(false);
			fbxImporter->Destroy();

			return importStatus;
		}

		FbxImportStatus Importer::LoadNavMeshA(const std::string& someFilePath, NavMesh& outNavMesh, bool shouldTriangulate)
		{
			FbxImportStatus importStatus;

			if (!std::filesystem::exists(someFilePath))
			{
				importStatus.Result = FbxImportResult::FileNotFound;
				return importStatus;
			}

			FbxImporter* fbxImporter = FbxImporter::Create(FbxEnvironment::Get().fbxManager, "");

			if (!fbxImporter)
			{
				importStatus.Result = FbxImportResult::NotInitialized;
				return importStatus;
			}

			if (!fbxImporter->Initialize(someFilePath.c_str(), -1, FbxEnvironment::Get().fbxIOSettings) || !fbxImporter->IsFBX())
			{
				const FbxStatus& fbxStatus = fbxImporter->GetStatus();
				FbxEnvironment::Get().lastFbxStatus = fbxStatus.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
				return importStatus;
			}

			FbxScene* fbxScene = FbxScene::Create(FbxEnvironment::Get().fbxManager, "The Scene");

			if (fbxImporter->Import(fbxScene))
			{
				if (shouldTriangulate)
				{
					if (!FbxEnvironment::Get().fbxConverter->Triangulate(fbxScene, true, true))
					{
						if (!FbxEnvironment::Get().fbxConverter->Triangulate(fbxScene, true, false))
						{
							importStatus.AddWarning(FbxImportWarning::CannotTriangulate);
						}
					}
				}
			}

			std::vector<FbxNode*> sceneMeshNodes;
			Internals::GatherMeshNodes(fbxScene->GetRootNode(), sceneMeshNodes);

			float maxExtents[3] = { 0, 0, 0 };
			float minExtents[3] = { 0, 0, 0 };

			FbxEnvironment::Get().fbxAxisSystem.DeepConvertScene(fbxScene);

			std::unordered_multimap<unsigned int, std::pair<size_t, float>> ControlPointWeights;

			std::vector<NavMesh::NavMeshChunk> navMeshChunks;
			navMeshChunks.reserve(sceneMeshNodes.size());

			for (FbxNode* mdlMeshNode : sceneMeshNodes)
			{
				FbxMesh* fbxMesh = mdlMeshNode->GetMesh();

				NavMesh::NavMeshChunk chunk;
				chunk.ChunkName = mdlMeshNode->GetName();

				const int fbxMeshPolygonCount = fbxMesh->GetPolygonCount();

				// Overdo this slightly since we might not have all triangles.
				chunk.Vertices.reserve(static_cast<size_t>(fbxMeshPolygonCount) * 5ULL);
				chunk.Polygons.resize(fbxMeshPolygonCount);
				//chunk.Indices.reserve(meshData.Vertices.capacity());

				std::unordered_map<size_t, Internals::VertexDuplicateData> VertexDuplicateAccelMap;
				unsigned int IndexCounter = 0;
				VertexDuplicateAccelMap.reserve(chunk.Vertices.capacity());

				for (int p = 0; p < fbxMeshPolygonCount; p++)
				{
					int fbxPolygonVertices = fbxMesh->GetPolygonSize(p);

					for (int v = 0; v < fbxPolygonVertices; v++)
					{
						const int fbxControlPtIndex = fbxMesh->GetPolygonVertex(p, v);
						const FbxVector4 fbxVxPos = fbxMesh->GetControlPointAt(fbxControlPtIndex);

						Vertex vx;
						vx.Position[0] = static_cast<float>(fbxVxPos[0]);
						vx.Position[1] = static_cast<float>(fbxVxPos[1]);
						vx.Position[2] = static_cast<float>(fbxVxPos[2]);
						vx.Position[3] = static_cast<float>(fbxVxPos[3]);

						if (vx.Position[0] > maxExtents[0])
							maxExtents[0] = vx.Position[0];
						if (vx.Position[1] > maxExtents[1])
							maxExtents[1] = vx.Position[1];
						if (vx.Position[2] > maxExtents[2])
							maxExtents[2] = vx.Position[2];

						if (vx.Position[0] < minExtents[0])
							minExtents[0] = vx.Position[0];
						if (vx.Position[1] < minExtents[1])
							minExtents[1] = vx.Position[1];
						if (vx.Position[2] < minExtents[2])
							minExtents[2] = vx.Position[2];

						// A drawback of using control points is that we MAY get duplicate vertices.
						// This means we'll need to compare and ensure that it is a unique vert.
						Internals::VertexHash Hasher;
						size_t hash = Hasher(vx);
						if (VertexDuplicateAccelMap.find(hash) == VertexDuplicateAccelMap.end())
						{
							VertexDuplicateAccelMap[hash] = { /*{ vx },*/ IndexCounter, hash };
							chunk.Vertices.push_back(vx);
							chunk.Polygons[p].Indices.push_back(IndexCounter++);
						}
						else
						{
							chunk.Polygons[p].Indices.push_back(VertexDuplicateAccelMap[hash].Idx);
						}
					}
				}

				if (!chunk.Vertices.empty())
				{
					navMeshChunks.push_back(chunk);
				}

				VertexDuplicateAccelMap.clear();
			}

			outNavMesh.Name = someFilePath;
			outNavMesh.Chunks = std::move(navMeshChunks);

			float extentsCenter[3];
			extentsCenter[0] = 0.5f * (minExtents[0] + maxExtents[0]);
			extentsCenter[1] = 0.5f * (minExtents[1] + maxExtents[1]);
			extentsCenter[2] = 0.5f * (minExtents[2] + maxExtents[2]);

			float boxExtents[3];
			boxExtents[0] = 0.5f * (maxExtents[0] - minExtents[0]);
			boxExtents[1] = 0.5f * (maxExtents[1] - minExtents[1]);
			boxExtents[2] = 0.5f * (maxExtents[2] - minExtents[2]);

			float extentRadius = std::max(boxExtents[0], std::max(boxExtents[1], boxExtents[2]));

			outNavMesh.BoxSphereBounds = {
				{boxExtents[0], boxExtents[1], boxExtents[2]},
				{extentsCenter[0], extentsCenter[1], extentsCenter[2]},
				extentRadius
			};

			importStatus.Result = importStatus.Warnings.empty() ? FbxImportResult::Success : FbxImportResult::SuccessWithWarnings;
			const FbxStatus& Status = fbxImporter->GetStatus();
			// Status will be deleted by fbxImporter->Destroy();

			if (Status.Error())
			{
				FbxEnvironment::Get().lastFbxStatus = Status.GetErrorString();
				importStatus.Result = FbxImportResult::InternalError;
			}

			fbxScene->Destroy(false);
			fbxImporter->Destroy();

			return importStatus;
		}

		std::string Importer::GetLastSDKError()
		{
			return FbxEnvironment::Get().lastFbxStatus;
		}

		FbxImportStatus Importer::LoadAnimationW(const std::wstring& someFilePath, Animation& outAnimation)
		{
			const std::string ansiFileName = Internals::WideStringtoAnsi(someFilePath);
			return LoadAnimationA(ansiFileName, outAnimation);
		}

		FbxImportStatus Importer::LoadNavMeshW(const std::wstring& someFilePath, NavMesh& outNavMesh, bool shouldTriangulate)
		{
			const std::string ansiFileName = Internals::WideStringtoAnsi(someFilePath);

			return LoadNavMeshA(ansiFileName, outNavMesh, shouldTriangulate);
		}

		void Importer::InitImporter()
		{
			FbxEnvironment::Get().Init();
		}

		void Importer::UninitImporter()
		{
			FbxEnvironment::Get().Destroy();
		}
	}
}
