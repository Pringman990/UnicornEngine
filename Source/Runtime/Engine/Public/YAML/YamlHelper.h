#pragma once
#include <Core.h>
#include "EngineDefines.h"
#include "Logger/Logger.h"

#pragma warning(push)
#pragma warning(disable: 4251 4275)
#include <yaml-cpp/yaml.h>
#pragma warning(pop)

class YamlHelper
{
public:

	ENGINE_API static YAML::Node LoadFromMemory(const ByteBuffer& Buffer);
	ENGINE_API static ByteBuffer WriteToMemory(const YAML::Node& Node);

private:
};