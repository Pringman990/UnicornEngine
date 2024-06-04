#include "common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.position = input.position;
    output.color = input.color;
	return output;
}