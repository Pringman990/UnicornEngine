#pragma once
#include "Shader/MaterialShader.h"
#include "Shader/InputLayout.h"
#include "Mesh.h"
#include "Buffers/RenderBuffer.h"

#include <Transform.h>

class IRenderStateManager
{
public:
	IRenderStateManager() = default;
	virtual ~IRenderStateManager() = default;

	virtual bool Init() = 0;

	virtual void SetMaterialShader(MaterialShader* aShader) = 0;
	virtual void SetInputLayoutShader(InputLayout* aLayout) = 0;
	virtual void SetVertexBuffer(RenderBuffer* aVertexBuffer) = 0;
	virtual void SetIndexBuffer(RenderBuffer* aIndexBuffer) = 0;
	virtual void SetObjectTransform(const Transform& aTransform) = 0;
	virtual void DrawMesh(uint32 aStartIndex, uint32 aIndexCount) = 0;

	virtual void FinishCommandList() = 0;

private:

};