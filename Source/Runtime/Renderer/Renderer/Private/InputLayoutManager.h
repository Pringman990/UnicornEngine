#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "InputLayout.h"

struct VSReflectData;
class Renderer;

class InputLayoutManager
{
public:
	InputLayoutManager(Renderer* InRenderer);
	~InputLayoutManager();

	InputLayout* TryGetLayout(void* VsBlob, const VSReflectData& VsReflection);

private:
	size_t HashSignatureBlob(ID3DBlob* Signature);
private:
	Renderer* mRenderer;
	UnorderedMap<size_t, InputLayout> mLayouts;
};