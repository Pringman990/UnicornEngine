#pragma once
#include "Editor.h"

class EditorWindowBase
{
public:
    EditorWindowBase(Editor& editor, const char* name) : mEditor(editor), mName(name) {};
    virtual ~EditorWindowBase() = default;

    virtual bool Init() {FATAL("Must override Init");};
    virtual void Tick() {FATAL("Must override Tick");};

    NODISC const char* GetName() const {return mName;}

protected:
    Editor& mEditor;
    const char* mName;
};
