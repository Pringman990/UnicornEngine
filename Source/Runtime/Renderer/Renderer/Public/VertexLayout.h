#pragma once
#include <Core.h>

enum class VertexLayoutType
{
	None,
	Vertex2D,
	Vertex3D
};

struct VertexLayout
{
	VkPipelineVertexInputStateCreateInfo vertexInput{};
	Vector<VkVertexInputBindingDescription> bindings;
	Vector<VkVertexInputAttributeDescription> attributes;

	static VertexLayout GetNullLayout();
	static VertexLayout Get2DLayout();
	static VertexLayout Get3DLayout();
};