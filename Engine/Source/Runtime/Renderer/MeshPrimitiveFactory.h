#pragma once
#include "Renderer.h"

class MeshPrimitiveFactory
{
public:
    static MeshCreateInfo CreateCube();

    static MeshCreateInfo CreatePlane();

    static MeshCreateInfo CreateQuad();
};
