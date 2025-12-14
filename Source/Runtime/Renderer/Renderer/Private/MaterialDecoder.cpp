#include "MaterialDecoder.h"

#include "YAML/YamlHelper.h"

MaterialDecodeData MaterialDecoder::LoadMaterial(const ByteBuffer& Buffer)
{
    YAML::Node root = YamlHelper::LoadFromMemory(Buffer);

    MaterialDecodeData data;
    data.shaderProgram = root["shaderprogram"].as<String>();

    auto texNode = root["textures"];
    for (const auto& tex : texNode)
    {
        std::pair<UniqueID128, String> pair;

        pair.second = tex["name"].as<String>();
        pair.first = UniqueID128(tex["uuid"].as<String>());

        data.textures.push_back(pair);
    }

    return data;
}
