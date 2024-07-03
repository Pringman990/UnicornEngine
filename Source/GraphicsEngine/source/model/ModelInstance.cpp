#include "GraphicsPch.h"
#include "ModelInstance.h"

#include "shaders/ModelShader.h"
#include "Model.h"

ModelInstance::ModelInstance()
    :
    mIsValid(false)
{
}

ModelInstance::~ModelInstance()
{
}

bool ModelInstance::IsValid()
{
    return mIsValid;
}

bool ModelInstance::Init(std::shared_ptr<Model> aModel)
{
    mModel = aModel;
    if (mModel == nullptr)
    {
        mIsValid = false;
        return false;
    }

    mShader = std::make_shared<ModelShader>();
    if (!mShader->CreateShader(L"PBR_Model_VS.hlsl", L"PBR_Model_PS.hlsl"))
    {
        mIsValid = false;
        return false;
    }

    mIsValid = true;
    return true;
}

void ModelInstance::Render()
{
    mModel->BindMaterials();

    const std::vector<MeshData>& meshData = mModel->GetMeshData();

    for (int i = 0; i < meshData.size(); i++)
    {
        mShader->Render(meshData[i], mModel->GetTransform());
    }

#ifdef _DEBUG
    Engine::GetGraphicsEngine().AddDrawCall();
#endif // _DEBUG
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
