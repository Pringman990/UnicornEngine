#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class Shader;
struct ID3D11Buffer;

struct DebugLineVertex
{
	Vector3 start;
	Color color;
};

class DebugLineRenderer
{
public:
	DebugLineRenderer();
	~DebugLineRenderer();

	void DrawLine(const Vector3& aStart, const Vector3& aEnd, const Color& aColor);
	void DrawCube(const Vector3& aCenter, const Vector3& aSize, const Color& aColor);

private:
	friend class Renderer;
	void Init();
	void Create(uint32 aLineCapacity);
	void UpdateBuffer();
	void Render();
private:
	std::vector<DebugLineVertex> mLineVertices;
	std::shared_ptr<Shader> mShader;
	ComPtr<ID3D11Buffer> mDebugVertexBuffer;

	uint32 mMaxLines;
};