#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "InputLayout.h"

struct ShaderReflectionInfo
{
    struct InputParam
    {
        String semanticName = ""; //e.g "POSITION", "TEXCOORD"...
        uint32 semanticIndex = 0; //e.g TEXTCOORD0, TEXCOORD1...
        RenderFormat format = RenderFormat::UNKNOWN;
    };

    struct BoundResources
    {
        String name; //The name of the varible in the shader.
        uint32 bindPoint; //Slot the resource is bound to e.g (t0) if texture 0.
        ShaderResourceType type;
    };

    struct CBuffer
    {
        struct Variable
        {
            String name;
            uint32 size;   //In bytes
            uint32 offset; //In bytes
            ShaderVariableType type;
        };

        String name;
        uint32 bindPoint;
        uint32 size; //In bytes
        bool engineOwned;
        Vector<Variable> variables;
    };

    Vector<InputParam> inputParams;
    Vector<BoundResources> boundResources;
    Vector<CBuffer> cBuffers;
};

struct ShaderStage
{
    ComPtr<ID3D11DeviceChild> shader;
    ComPtr<ID3DBlob> bytecode;
    ShaderStageBind bind;
    ShaderReflectionInfo reflectedInfo;
};

struct ShaderProgram
{
    ShaderStage vs;
    ShaderStage fs;
    InputLayout* inputLayout;   
};