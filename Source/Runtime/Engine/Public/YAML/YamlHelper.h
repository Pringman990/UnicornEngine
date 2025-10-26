#pragma once
#include <Core.h>
#include "EngineDefines.h"

#pragma warning(push)
#pragma warning(disable: 4251 4275)
#include <yaml-cpp/yaml.h>
#pragma warning(pop)

class YamlHelper
{
public:

	ENGINE_API static YAML::Node LoadFromMemory(const ByteBuffer& Buffer);
	ENGINE_API static ByteBuffer WriteToMemory(const YAML::Node& Node);

	ENGINE_API static void WriteUniqueID128(YAML::Node& Node, const String& Key, const UniqueID128& UUID);
	ENGINE_API static void WriteString(YAML::Node& Node, const String& Key, const String& Str);
private:
	ENGINE_API static YAML::Node GetOrCreateChildHierarcy(YAML::Node& Node, const String& Key);
};