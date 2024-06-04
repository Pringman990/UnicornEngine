#pragma once

class Model;
class ModelShader;

class ModelInstance
{
public:
	ModelInstance();
	~ModelInstance();

	bool Init(std::shared_ptr<Model> aModel);
	void Render();
	void Render(ModelShader& aModelShader);

	std::shared_ptr<Model> GetModel();

private:
	std::shared_ptr<Model> mModel;
	std::shared_ptr<ModelShader> mShader;
};