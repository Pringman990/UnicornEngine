#include "GraphicsPch.h"
#include "ModelInstance.h"

#include "../shaders/ModelShader.h"
#include "Model.h"

ModelInstance::ModelInstance()
{
}

ModelInstance::~ModelInstance()
{
}

bool ModelInstance::Init(std::shared_ptr<Model> aModel)
{
    mModel = aModel;

    mShader = std::make_shared<ModelShader>();
    if (!mShader->CreateShader(L"PBR_Model_VS.hlsl", L"PBR_Model_PS.hlsl"))
        return false;

    return false;
}

void ModelInstance::Render()
{
    mModel->BindMaterials();

    const std::vector<MeshData>& meshData = mModel->GetMeshData();

    for (int i = 0; i < meshData.size(); i++)
    {
        mShader->Render(meshData[i], mModel->GetTransform());
    }

    GraphicsEngine::GetInstance().AddDrawCall();
}

void ModelInstance::Render(ModelShader& aModelShader)
{
    mModel->BindMaterials();

    const std::vector<MeshData>& meshData = mModel->GetMeshData();

    for (int i = 0; i < meshData.size(); i++)
    {
        aModelShader.Render(meshData[i], mModel->GetTransform());
    }
}

std::shared_ptr<Model> ModelInstance::GetModel()
{
    return mModel;
}
