#include "EnginePch.h"
#include "ECSSystem.h"

#include "ECSWorld.h"

ecs::System::System()
	:
	mWorld(nullptr)
{

}

ecs::System::~System()
{
}

void ecs::System::SetPipeline(Pipeline aPipeline)
{
	if (mWorld == nullptr)
	{
		ASSERT_MSG(false, "ECS World Was Null");
		return;
	}

	//Set pipeline
	aPipeline;
}
