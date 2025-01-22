#include "AssetRegistry.h"

#include <type_traits>

AssetRegistry::AssetRegistry()
{
}

AssetRegistry::~AssetRegistry()
{
	mManagers.clear();

}
