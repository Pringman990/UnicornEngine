#include <MoveCubeSystem.h>
#include <ESystemManager.h>

struct Static_Initilized_MoveCubeSystem_Data
{
	Static_Initilized_MoveCubeSystem_Data()
	{
		ESystemManager::Get()->RegisterSystem(&MoveCubeSystem, "MoveCubeSystem", EPipeline::ESystemUpdate);
	}
};

static Static_Initilized_MoveCubeSystem_Data __Static_Initilized_MoveCubeSystem_Data;
