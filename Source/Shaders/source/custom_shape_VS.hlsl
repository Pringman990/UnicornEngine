#include "common.hlsli"

ModelPixelInputType main(ModelVertexInputType input)
{
    ModelPixelInputType output;
    output.position = input.position;
    output.color = input.color;
	return output;
}