#pragma once
#include "EditorWindowBase.h"
#include "Core/Types.h"

class EditorWindowManager
{
public:
    EditorWindowManager();
    ~EditorWindowManager();

    void Init();

    template<typename T, typename... Args>
    void RegisterWindow(Args&&... args)
    {
        mWindows.push_back(MakeOwned<T>(std::forward<Args>(args)...));

        if (mInitialized)
        {
            mWindows.back()->Init();
        }
    }

    void Tick();

private:
    List<OwnedPtr<EditorWindowBase>> mWindows;
    bool mInitialized = false;
};
