#include "pch.h"
#include "Shader/Shader.h"

Shader::Shader(eShaderTypes aType)
    :
    mType(aType)
{
}

Shader::~Shader()
{
}

bool Shader::Compile()
{
    _LOG_RENDERER_WARNING("Shader has not implemented Compile");
    return false;
}

bool Shader::Bind()
{
    _LOG_RENDERER_WARNING("Shader has not implemented Bind");
    return false;
}
