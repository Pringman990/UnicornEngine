#include "pch.h"
#include "RenderQueue.h"

RenderQueue::RenderQueue()
	:
	mRenderStateManager(nullptr)
{
}

RenderQueue::~RenderQueue()
{
	mRenderPasses.clear();
	delete mRenderStateManager;
	mRenderStateManager = nullptr;
}
bool RenderQueue::Init()
{
	return mRenderStateManager->Init();
}
void RenderQueue::SubmitRenderPass(const RenderPass& aRenderPass)
{
	mRenderPasses.push_back(aRenderPass);
}

//void RenderQueue::SwapBuffers()
//{
//
//}

void RenderQueue::Execute()
{
	for (const auto& renderPass : mRenderPasses)
	{
		ExecuteRenderPass(renderPass);
	}

	mRenderPasses.clear();
}

void RenderQueue::ExecuteRenderPass(const RenderPass& aRenderPass)
{
	std::vector<RenderTarget*> renderTargets;
	for (const auto& attachment : aRenderPass.GetAttachments())
	{
		if (attachment.rendertarget)
		{
			renderTargets.push_back(attachment.rendertarget.value());

			if (attachment.shouldClear)
				attachment.rendertarget.value()->Clear();
		}
	}

	// We send multiple as this is ONE renderpass and for like a G-Buffer 
	// -we want to render to multiple rendertargets at once
	if (!renderTargets.empty())
	{
		//Send all rendertargets to the statemanager
	}

	for (const auto& commandList : aRenderPass.GetCommandLists())
	{
		ExecuteCommandList(commandList);
	}
}

void RenderQueue::ExecuteCommandList(const RenderCommandList& aCommandList)
{
	for (const auto& command : aCommandList.GetCommands())
	{
		switch (command.type)
		{
		case RenderCommand::RenderCommandTypes::SetShader:
		{
			//StateManager.SetShader
			break;
		}
		case RenderCommand::RenderCommandTypes::DrawMesh:
		{
			RenderCommand::MeshData meshData = std::get<RenderCommand::MeshData>(command.data);

			if (meshData.mesh->GetSubMeshes().size() <= 0)
				break;

			mRenderStateManager->SetObjectTransform(meshData.transform);

			mRenderStateManager->SetVertexBuffer(meshData.mesh->GetVertexBuffer());
			mRenderStateManager->SetIndexBuffer(meshData.mesh->GetIndexBuffer());

			for (auto& subMesh : meshData.mesh->GetSubMeshes())
			{
				mRenderStateManager->SetInputLayoutShader(subMesh.material->GetInputLayout());
				mRenderStateManager->SetMaterialShader(subMesh.material->GetShader());

				mRenderStateManager->DrawMesh(subMesh.startIndex, subMesh.indexCount);
			}
			break;
		}
		default:
			break;
		}
	}
}

void RenderQueue::_SetStateManager(IRenderStateManager* aStateManager)
{
	mRenderStateManager = aStateManager;
}