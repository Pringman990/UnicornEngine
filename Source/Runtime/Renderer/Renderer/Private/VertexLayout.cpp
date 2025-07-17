#include "VertexLayout.h"

VertexLayout VertexLayout::GetNullLayout()
{
	VertexLayout layout;

	layout.vertexInput =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = VK_NULL_HANDLE,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = VK_NULL_HANDLE
	};

	return layout;
}

VertexLayout VertexLayout::Get2DLayout()
{
	VertexLayout layout;

	layout.bindings =
	{
		{
			.binding = 0,
			.stride = sizeof(Vertex2D),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};

	layout.attributes =
	{
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(Vertex2D, position)
		},
		{
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex2D, color)
		}
	};

	layout.vertexInput =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = static_cast<uint32>(layout.bindings.size()),
		.pVertexBindingDescriptions = layout.bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32>(layout.attributes.size()),
		.pVertexAttributeDescriptions = layout.attributes.data()
	};

	return layout;
}

VertexLayout VertexLayout::Get3DLayout()
{
	VertexLayout layout;

	layout.bindings =
	{
		{
			.binding = 0,
			.stride = sizeof(Vertex3D),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};

	layout.attributes =
	{
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex3D, position)
		},
		{
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex3D, color)
		}
	};

	layout.vertexInput =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = static_cast<uint32>(layout.bindings.size()),
		.pVertexBindingDescriptions = layout.bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32>(layout.attributes.size()),
		.pVertexAttributeDescriptions = layout.attributes.data()
	};

	return layout;
}
