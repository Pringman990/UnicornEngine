#pragma once
#include <Core.h>
#include "EditorWindow.h"

#include <Renderer.h>

class GraphicsDebugWindow : public EditorWindow
{
public:
	GraphicsDebugWindow(Editor* EditorPtr);
	~GraphicsDebugWindow() override;

	virtual bool Init() override;
	virtual void Render() override;

private:
	GraphicsCardInfo mGraphicsCardInfo;
	ImTextureID imguiTex;
	class Sampler* sampler;
	class Texture2D* texture;
};

//REGISTER_EDITORWINDOW(GraphicsDebugWindow)