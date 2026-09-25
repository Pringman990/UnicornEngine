#pragma once
#include "EditorWindowBase.h"

class InspectorEditorWindow : public EditorWindowBase
{
public:
    InspectorEditorWindow(Editor& editor, const char* name);
    ~InspectorEditorWindow() override;

    bool Init() override;
    void Tick() override;

private:
};
