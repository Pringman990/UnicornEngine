#include <MoveCubeComponent.h>
#include <Reflection/ReflectionRegistry.h>
#include <EComponentRegistry.h>

struct Reflected_MoveCubeComponent_Data
{
	REFLECT_MEMBERS(Reflected_MoveCubeComponent_Data, MoveCubeComponent,
		REFLECT_MEMBER(MoveCubeComponent, speed),
		REFLECT_MEMBER(MoveCubeComponent, meshHandle)
	);

	Reflected_MoveCubeComponent_Data()
	{
		ReflectionRegistry::Get()->RegisterType(typeInfo);
		EComponentRegistry::Get()->RegisterComponent<MoveCubeComponent>();
	}
};

static Reflected_MoveCubeComponent_Data __Static_Register_Reflection_MoveCubeComponent_Data;