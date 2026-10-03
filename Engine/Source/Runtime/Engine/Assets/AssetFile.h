#pragma once

#include "Asset.h"
#include "Core/Archive.h"

Expected<AssetInfo, String> ReadAssetInfo(Archive& archive, const String& path);
Expected<void, String> WriteAssetInfo(Archive& archive, const AssetInfo& info);
String AssetSourcePath(const String& descriptorPath, const String& source);
