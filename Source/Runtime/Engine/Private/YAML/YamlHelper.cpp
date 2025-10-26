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

ENGINE_API void YamlHelper::WriteUniqueID128(YAML::Node& Node, const String& Key, const UniqueID128& UUID)
{
    WriteString(Node, Key, UUID.ToString());
}

ENGINE_API void YamlHelper::WriteString(YAML::Node& Node, const String& Key, const String& Str)
{
    std::stringstream ss(Key);
    std::string key;
    std::vector<std::string> parts;

    while (std::getline(ss, key, '/'))
        parts.push_back(key);


}

ENGINE_API YAML::Node YamlHelper::GetOrCreateChildHierarcy(YAML::Node& Node, const String& Key)
{
    std::stringstream ss(Key);
    std::string segment;
    YAML::Node current = Node;

    while (std::getline(ss, segment, '/'))
        current = current[segment];

    return current;
}
