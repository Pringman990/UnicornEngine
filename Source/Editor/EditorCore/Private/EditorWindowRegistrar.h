#pragma once
#include "EditorWindowManager.h"

class EditorWindowRegistrar
{
public:
    EditorWindowRegistrar(const std::string& aType, EditorWindowManager::WindowCreator creator) {
        // Register with the global WindowManager instance
        EditorWindowManager::GetInstance()->RegisterWindowType(aType, std::move(creator));
    }

private:
    
};

#define REGISTER_EDITORWINDOW(TYPE) \
    static EditorWindowRegistrar _sEditorWindowRegistrar_##TYPE(#TYPE, []() -> EditorWindow* { \
        return new TYPE(); \
    });