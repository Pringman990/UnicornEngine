#include "MoveCubeComponent.h"

struct Reflected_MoveCubeComponent_Data
{
	DEFINE_TYPE_UUID("4e160c7a-5be9-486a-b65c-1fd18397adfe");

	REFLECT_SCHEMA(Reflected_MoveCubeComponent_Data,
		MoveCubeComponent, 
		"Component",
		_uuid,
		REFLECT_FIELD(MoveCubeComponent, speed, "float", "f6569fe5-cfb3-4aeb-9564-ba15e5336330"),
		REFLECT_FIELD(MoveCubeComponent, slow, "float", "17f3d19d-9eb2-4d26-a80e-11adfe678ebf")
	);

	Reflected_MoveCubeComponent_Data()
	{
		__Reflection::RegisterType(&_schema);
	}

	~Reflected_MoveCubeComponent_Data()
	{
		__Reflection::NullifyType(_uuid);
	}

};

static Reflected_MoveCubeComponent_Data __Static_Register_Reflection_MoveCubeComponent_Data;

DECLARE_GETUUID_TYPEINFO(Reflected_MoveCubeComponent_Data, MoveCubeComponent);