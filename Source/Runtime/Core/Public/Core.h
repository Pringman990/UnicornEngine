#pragma once

//STL
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>
#include <string>
#include <typeindex>
#include <memory>
#include <bitset>
#include <algorithm>

//Application
#include <Application/Application.h>
#include <Application/Generic/GenericApplication.h>
#include <Application/Generic/IWindow.h>

//Math
#include <Math/Transform.h>
#include <SimpleMath.h>

//Event
#include <EventDispatcher/Notifier.h>

//Logger
#include <Logger/Logger.h>

//StandardTypes
#include <StandardTypes/StandardTypes.h>

//UniqueID
#include <UniqueID/FGUID.h>
#include <UniqueID/IDGenerator.h>
#include <UniqueID/UniqueID.h>

//Utility
#include <Utility/Array3D.h>
#include <Utility/Utility.h>

#include <Module/ModuleManager.h>


#include <Timer/Timer.h>

#include <FileSystem/FileSystem.h>
#include <FileSystem/FileWatcherSubsystem.h>
#include <FileSystem/FileWatcherBackendFactory.h>

#include <ResourceManagment/ResourceHandle.h>
#include <ResourceManagment/ResourcePool.h>

#include <Reflection/ReflectionRegistry.h>