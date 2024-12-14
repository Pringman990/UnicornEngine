#pragma once
#include <string>

enum class eShaderTypes : int32_t
{
	eMatrial = 1,
	eVertex = 2,
	ePixel = 3,

	eInvalid = INT_MAX
};

class Shader
{
public:
	Shader(eShaderTypes aType);
	virtual ~Shader();

	virtual bool Compile();
	virtual bool Bind();

private:
	eShaderTypes mType;
};

struct ShaderKey
{
	ShaderKey(std::string aFilename, std::string aEntryPoint, std::string aShaderModel, eShaderTypes aType)
		: filename(aFilename), entryPoint(aEntryPoint), shaderModel(aShaderModel), type(aType)
	{
	}

	std::string filename;
	std::string entryPoint;
	std::string shaderModel;
	eShaderTypes type;

	bool operator==(const ShaderKey& aOther) const
	{
		return filename == aOther.filename &&
			entryPoint == aOther.entryPoint &&
			type == aOther.type &&
			shaderModel == aOther.shaderModel;
	}
};

namespace std {
	template <>
	struct hash<ShaderKey> {
		std::size_t operator()(const ShaderKey& key) const {
			size_t h1 = std::hash<std::string>{}(key.filename);
			size_t h2 = std::hash<std::string>{}(key.entryPoint);
			size_t h3 = std::hash<std::string>{}(key.shaderModel);
			size_t h4 = std::hash<int32_t>{}(static_cast<int32_t>(key.type));

			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
		}
	};
}