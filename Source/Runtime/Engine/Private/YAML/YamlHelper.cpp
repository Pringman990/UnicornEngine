#include "Yaml/YamlHelper.h"

#include "Logger/Logger.h"

ENGINE_API YAML::Node YamlHelper::LoadFromMemory(const ByteBuffer& Buffer)
{
    _PAUSE_TRACK_MEMORY(true);
    String yamlText(reinterpret_cast<const char*>(Buffer.data()), Buffer.size());

    try 
    {
        YAML::Node node = YAML::Load(yamlText);
        _PAUSE_TRACK_MEMORY(false);
        return node;
    }
    catch (const std::exception& e) 
    {  
        LOG_ERROR("Failed to load yaml from memory, error code: {}", e.what());
        _PAUSE_TRACK_MEMORY(false);
        return YAML::Node();
    }
}

ENGINE_API ByteBuffer YamlHelper::WriteToMemory(const YAML::Node& Node)
{
    std::stringstream ss;
    ss << Node;

    std::string yamlText = ss.str();
    return ByteBuffer(yamlText.begin(), yamlText.end());
}
