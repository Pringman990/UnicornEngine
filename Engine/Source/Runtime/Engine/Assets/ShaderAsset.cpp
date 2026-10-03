#include "pch.h"
#include "ShaderAsset.h"

ShaderAsset::~ShaderAsset()
{
    if (mRuntime)
        Services::Get<Renderer>().DestroyProgram(mRuntime);
}
