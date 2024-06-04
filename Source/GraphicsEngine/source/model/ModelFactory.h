#pragma once

class Model;
class ModelInstance;

class ModelFactory
{
public:
	ModelFactory();
	~ModelFactory();

	std::shared_ptr<Model> GetModel(const std::wstring& aPath);
	ModelInstance GetModelInstance(const std::wstring& aPath);

private:
	std::shared_ptr<Model> LoadModelW(const std::wstring& aPath);
private:
	std::unordered_map<std::wstring, std::shared_ptr<Model>> mLoadedModels;
};