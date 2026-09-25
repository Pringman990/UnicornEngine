#include "pch.h"
#include "Yaml/YamlHelper.h"

YAML::Node YamlHelper::LoadFromMemory(const ByteBuffer& Buffer)
{
    const String yamlText(reinterpret_cast<const char*>(Buffer.data()), Buffer.size());
    try
    {
        YAML::Node node = YAML::Load(yamlText);
        return node;
    }
    catch (const std::exception& e)
    {
        LOG_WARNING("Failed to load yaml from memory, error code: {}", e.what());
        return {};
    }
}

ByteBuffer YamlHelper::WriteToMemory(const YAML::Node& Node)
{
    std::stringstream ss;
    ss << Node;

    String yamlText = ss.str();
    return {yamlText.begin(), yamlText.end()};
}
