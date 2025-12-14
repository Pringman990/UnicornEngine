#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "InputLayout.h"

struct ShaderReflectionInfo;
class Renderer;

class InputLayoutManager
{
public:
	InputLayoutManager(Renderer* InRenderer);
	~InputLayoutManager();

	InputLayout* TryGetLayout(void* VsBlob, const ShaderReflectionInfo& VsReflection);

private:
	size_t HashSignatureBlob(ID3DBlob* Signature);
private:
	Renderer* mRenderer;
	UnorderedMap<size_t, OwnedPtr<InputLayout>> mLayouts;
};