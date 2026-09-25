#pragma once
#include <yaml-cpp/yaml.h>

class YamlHelper
{
public:
    static YAML::Node LoadFromMemory(const ByteBuffer& Buffer);
    static ByteBuffer WriteToMemory(const YAML::Node& Node);

private:
};