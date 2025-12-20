#include "Archive/YamlArchive.h"
#include "FileSystem/FileSystem.h"

YamlArchive::YamlArchive()
{
	YAML::Node root;
	mStack.push_back(
		{
			.type = ArchiveContextType::Object,
			.node = std::move(root),
			.currentArrayIndex = 0
		});
}

YamlArchive::YamlArchive(const ByteBuffer& Buffer)
{
	YAML::Node root = YamlHelper::LoadFromMemory(Buffer);
	mStack.push_back(
		{
			.type = ArchiveContextType::Object,
			.node = std::move(root),
			.currentArrayIndex = 0
		});
}

YamlArchive::YamlArchive(YAML::Node Node)
{
	mStack.push_back(
		{
			.type = ArchiveContextType::Object,
			.node = std::move(Node),
			.currentArrayIndex = 0
		});
}

YamlArchive::~YamlArchive()
{
}

ENGINE_API OwnedPtr<Archive> YamlArchive::CreateSubArchive(const String& Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("Can't read with no active container");
		return {};
	}

	auto& ctx = mStack.back();

	return MakeOwned<YamlArchive>(ctx.node[Key]);
}

void YamlArchive::WriteToFile(PathView FilePath)
{
	FileSystem* fs = FileSystem::Instance();
	fs->WriteAll(FilePath, YamlHelper::WriteToMemory(mStack[0].node));
}

#pragma region Writing

bool YamlArchive::WriteInt32(int32 Value, const String& Key)
{
	return WriteStandardTypes<int32>(Value, Key);
}

bool YamlArchive::WriteInt64(int64 Value, const String& Key)
{
	return WriteStandardTypes<int64>(Value, Key);
}

bool YamlArchive::WriteUInt32(uint32 Value, const String& Key)
{
	return WriteStandardTypes<uint32>(Value, Key);
}

bool YamlArchive::WriteUInt64(uint64 Value, const String& Key)
{
	return WriteStandardTypes<uint64>(Value, Key);
}

ENGINE_API bool YamlArchive::WriteLong(long Value, const String& Key)
{
	return WriteStandardTypes<long>(Value, Key);
}

bool YamlArchive::WriteFloat(float Value, const String& Key)
{
	return WriteStandardTypes<float>(Value, Key);
}

ENGINE_API bool YamlArchive::WriteDouble(double Value, const String& Key)
{
	return WriteStandardTypes<double>(Value, Key);
}

bool YamlArchive::WriteBool(bool Value, const String& Key)
{
	return WriteStandardTypes<bool>(Value, Key);
}

bool YamlArchive::WriteString(const String& Value, const String& Key)
{
	return WriteStandardTypes<String>(Value, Key);
}

bool YamlArchive::BeginWriteArray(const String& Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return false;
	}

	YAML::Node childSeq = YAML::Node(YAML::NodeType::Sequence);

	auto& ctx = mStack.back();
	if (ctx.type == ArchiveContextType::Object)
	{
		if (Key.empty())
		{
			LOG_ERROR("Missing key for object value");
			return false;
		}

		ctx.node[Key] = childSeq;
	}
	else
	{
		ctx.node.push_back(childSeq);
	}

	mStack.push_back(
		{
			.type = ArchiveContextType::Sequence,
			.node = std::move(childSeq),
			.currentArrayIndex = 0
		});

	return true;
}

void YamlArchive::EndWriteArray()
{
	if (mStack.empty())
	{
		LOG_ERROR("Trying to EndObject with empty stack");
		return;
	}

	mStack.pop_back();
}

bool YamlArchive::BeginWriteObject(const String& Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return false;
	}
	
	YAML::Node childObject = YAML::Node(YAML::NodeType::Map);

	auto& ctx = mStack.back();
	if (ctx.type == ArchiveContextType::Object)
	{
		if (Key.empty())
		{
			LOG_ERROR("Missing key for object value");
			return false;
		}

		ctx.node[Key] = childObject;
	}
	else
	{
		ctx.node.push_back(childObject);
	}

	mStack.push_back(
		{
			.type = ArchiveContextType::Object,
			.node = std::move(childObject),
			.currentArrayIndex = 0
		});

	return true;
}

void YamlArchive::EndWriteObject()
{
	if (mStack.empty())
	{
		LOG_ERROR("Trying to EndObject with empty stack");
		return;
	}

	mStack.pop_back();
}

#pragma endregion

#pragma region Reading

bool YamlArchive::HasKey(std::string_view Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("Can't read with no active container");
		return false;
	}

	auto& ctx = mStack.back();

	return ctx.node.IsMap() && ctx.node[Key].IsDefined();
}

bool YamlArchive::ReadInt32(int32& OutValue, const String& Key)
{
	return ReadStandardTypes<int32>(OutValue, Key);
}

bool YamlArchive::ReadInt64(int64& OutValue, const String& Key)
{
	return ReadStandardTypes<int64>(OutValue, Key);
}

bool YamlArchive::ReadUInt32(uint32& OutValue, const String& Key)
{
	return ReadStandardTypes<uint32>(OutValue, Key);
}

bool YamlArchive::ReadUInt64(uint64& OutValue, const String& Key)
{
	return ReadStandardTypes<uint64>(OutValue, Key);
}

ENGINE_API bool YamlArchive::ReadLong(long& OutValue, const String& Key)
{
	return ReadStandardTypes<long>(OutValue, Key);
}

bool YamlArchive::ReadFloat(float& OutValue, const String& Key)
{
	return ReadStandardTypes<float>(OutValue, Key);
}

ENGINE_API bool YamlArchive::ReadDouble(double& OutValue, const String& Key)
{
	return ReadStandardTypes<double>(OutValue, Key);
}

bool YamlArchive::ReadBool(bool& OutValue, const String& Key)
{
	return ReadStandardTypes<bool>(OutValue, Key);
}

bool YamlArchive::ReadString(String& OutValue, const String& Key)
{
	return ReadStandardTypes<String>(OutValue, Key);
}

bool YamlArchive::BeginReadArray(const String& Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return false;
	}

	auto& ctx = mStack.back();
	YAML::Node child;

	if (ctx.type == ArchiveContextType::Object)
	{
		if (Key.empty())
		{
			LOG_ERROR("Missing key for object value");
			return false;
		}

		child = ctx.node[Key];
		if (!child || !child.IsSequence())
		{
			LOG_ERROR("Trying to read non sequence as an array");
			return false;
		}
	}
	else
	{
		child = ctx.node[ctx.currentArrayIndex];
		if (!child || !child.IsSequence())
		{
			LOG_ERROR("Trying to read non sequence as an array");
			return false;
		}
	}

	mStack.push_back(
		{
			.type = ArchiveContextType::Sequence,
			.node = std::move(child),
			.currentArrayIndex = 0
		});

	return true;
}

void YamlArchive::EndReadArray()
{
	if (mStack.empty())
	{
		LOG_ERROR("Trying to EndReadArray with empty stack");
		return;
	}

	mStack.pop_back();
}

ENGINE_API uint32 YamlArchive::GetArraySize()
{
	if (mStack.empty())
	{
		LOG_ERROR("Trying to EndReadArray with empty stack");
		return 0;
	}

	YAML::Node arr = mStack.back().node;
	if (!arr || !arr.IsSequence())
	{
		LOG_ERROR("Trying to get size of array from non array");
		return 0;
	}

	return static_cast<uint32>(arr.size());
}

void YamlArchive::Next()
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return;
	}

	mStack.back().currentArrayIndex++;
}

bool YamlArchive::BeginReadObject(const String& Key)
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return false;
	}


	auto& ctx = mStack.back();
	YAML::Node child;

	if (ctx.type == ArchiveContextType::Object)
	{
		if (Key.empty())
		{
			LOG_ERROR("Missing key for object value");
			return false;
		}

		child = ctx.node[Key];
	}
	else
	{
		child = ctx.node[ctx.currentArrayIndex];
	}

	mStack.push_back(
		{
			.type = ArchiveContextType::Object,
			.node = std::move(child),
			.currentArrayIndex = 0,
			.mapIt = child.begin(),
			.mapEnd = child.end()
		});

	return true;
}

void YamlArchive::EndReadObject()
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return;
	}

	mStack.pop_back();
}

ENGINE_API bool YamlArchive::ReadObjectKey(String& OutKey)
{
	if (mStack.empty())
	{
		LOG_ERROR("No active container");
		return false;
	}

	auto& ctx = mStack.back();
	if (ctx.type != ArchiveContextType::Object)
	{
		LOG_ERROR("Can't read object key from non object");
		return false;
	}

	if (ctx.mapIt == ctx.mapEnd)
		return false;

	OutKey = ctx.mapIt->first.as<String>();

	ctx.mapIt++;

	return true;
}




#pragma endregion