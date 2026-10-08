#pragma once
#include "Engine/Layer.h"
#include "Engine/Application.h"
#include "UI.h"

class UILayer : public Layer
{
public:
    void BeginFrame() override
    {
        const auto& info = Services::Get<Application>().GetInfo();
        auto& ui = Services::Get<UI>();
        ui.SetViewport({0, 0, info.viewportWidth, info.viewportHeight}, info.viewportHeight);
        ui.BeginFrame();
    }
    bool OnInput(const InputEvent& event) override { return Services::Get<UI>().OnInput(event); }
    void ResetInput() override { Services::Get<UI>().ResetInput(); }
    void Render() override { Services::Get<UI>().Render(); }
};
