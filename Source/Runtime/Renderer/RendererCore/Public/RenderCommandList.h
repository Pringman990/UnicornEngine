#pragma once
#include "Shader/Shader.h"
#include "Mesh.h"
#include <Transform.h>
#include <variant>

struct RenderCommand
{
	enum class RenderCommandTypes
	{
		SetShader,
		DrawMesh
	} type;

	struct ShaderData
	{
		Shader* shader;
	};

	struct MeshData
	{
		Mesh* mesh;
		Transform transform;
	};

	std::variant<ShaderData, MeshData> data;

	static RenderCommand SetShader(Shader* aShader)
	{
		RenderCommand cmd;
		cmd.type = RenderCommandTypes::SetShader;
		cmd.data = ShaderData{ aShader };
		return cmd;
	}

	static RenderCommand SetMesh(Mesh* aMesh, const Transform& aTransform)
	{
		RenderCommand cmd;
		cmd.type = RenderCommandTypes::DrawMesh;
		cmd.data = MeshData{ aMesh, aTransform };
		return cmd;
	}
};

class RenderCommandList
{
public:
	const std::vector<RenderCommand>& GetCommands() const { return mCommands; };

	void RecordSetShader(Shader* aShader)
	{
		mCommands.push_back(RenderCommand::SetShader(aShader));
	};

	void RecordDrawMesh(Mesh* aMesh, const Transform& aTransform)
	{
		mCommands.push_back(RenderCommand::SetMesh(aMesh, aTransform));
	};


private:
	std::vector<RenderCommand> mCommands;
};