#pragma once
#include "Core/Types.h"
#include "Core/Defines.h"
#include "Core/ServiceRegistry.h"
#include "Input/InputEvent.h"
#include <glm/glm.hpp>
#include "Renderer/Renderer.h"

using UIHandle = uint32;
enum class UIAnchor { TopLeft, BottomLeft };

class UI
{
    INIT_SERVICE(UI)
public:
    void Init();
    void Destroy();
    void SetViewport(Viewport viewport, int32 windowHeight);
    void BeginFrame();
    bool OnInput(const InputEvent& event);
    void ResetInput();

    UIHandle CreateButton(glm::vec4 bounds, glm::vec4 colour, Func<void()> onClick,
                          UIAnchor anchor = UIAnchor::TopLeft);
    UIHandle CreateRectangle(glm::vec4 bounds, glm::vec4 colour,
                             UIAnchor anchor = UIAnchor::TopLeft);
    void Remove(UIHandle handle);

    // Transient, viewport-local rectangles, such as this frame's health bars.
    void Rectangle(glm::vec4 bounds, glm::vec4 colour);
    void Render();

private:
    struct Widget
    {
        UIHandle handle;
        glm::vec4 bounds;
        glm::vec4 colour;
        UIAnchor anchor;
        Func<void()> onClick;
    };
    struct Draw { glm::vec4 bounds; glm::vec4 colour; };
    glm::vec4 Bounds(const Widget& widget) const;
    UIHandle HitTest(glm::vec2 position) const;

    List<Widget> mWidgets;
    List<Draw> mDraws;
    ShaderProgramHandle mProgram;
    Viewport mViewport{};
    int32 mWindowHeight = 0;
    UIHandle mNextHandle = 1;
    UIHandle mHovered = 0;
    UIHandle mPressed = 0;
};
