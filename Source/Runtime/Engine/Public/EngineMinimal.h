#pragma once
#include <Core.h> //External

#include <Subsystem/EngineSubsystem.h>

#include <Application/Application.h>

#include <Timer/Timer.h>

#include <Reflection/ReflectionRegistry.h>

#include <Logger/Logger.h>

#include <FileSystem/FileSystem.h>
#include <FileSystem/FileWatcherSubsystem.h>
#include <FileSystem/FileWatcherBackendFactory.h>

#include <ResourceManagment/ResourcePool.h>
#include <ResourceManagment/DirectResourcePool.h>
#include <ResourceManagment/HeapResourcePool.h>

#include <UniqueID/UniqueID64.h>
#include <UniqueID/UniqueID128.h>

#define STRINGIFY(STR) #STR