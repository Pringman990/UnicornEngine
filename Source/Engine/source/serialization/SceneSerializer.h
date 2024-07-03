#pragma once

class Scene;

class SceneSerializer
{
public:

	bool SerializeYAML(const std::wstring& aFilePath, Scene& aScene);
	bool SerializeBinary(const std::wstring& aFilePath, Scene& aScene);

	bool DeserializeYAML(const std::wstring& aFilePath, Scene& aScene);
	bool DeserializeBinary(const std::wstring& aFilePath, Scene& aScene);

private:
	friend class Engine;
	SceneSerializer();
	~SceneSerializer();

};