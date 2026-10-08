#include "pch.h"
#include "UI.h"
#include "Engine/FileSystem.h"
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void UI::Init()
{
    auto& files = Services::Get<FileSystem>();
    auto vertex = files.ReadAll("engine://Assets/Shaders/Sprite.vert");
    auto fragment = files.ReadAll("engine://Assets/Shaders/Sprite_Single_Color.frag");
    if (!vertex || !fragment)
        FATAL("Can't read UI shaders");
    ShaderSourceCreateInfo info{};
    info.vertexSource = std::move(*vertex);
    info.fragmentSource = std::move(*fragment);
    info.uniformLocations = {"uColor"};
    auto program = Services::Get<Renderer>().CreateProgram(info);
    if (!program)
        FATAL("Can't create UI shader: {}", program.error());
    mProgram = *program;
}

void UI::Destroy()
{
    mWidgets.clear();
    mDraws.clear();
    ResetInput();
    Services::Get<Renderer>().DestroyProgram(mProgram);
}

void UI::SetViewport(Viewport viewport, int32 windowHeight)
{
    mViewport = viewport;
    mWindowHeight = windowHeight;
}

void UI::BeginFrame()
{
    mDraws.clear();
    mHovered = 0;
}

UIHandle UI::CreateButton(glm::vec4 bounds, glm::vec4 colour, Func<void()> onClick, UIAnchor anchor)
{
    ASSERT(onClick, "A UI button needs a callback");
    const UIHandle handle = mNextHandle++;
    mWidgets.push_back({handle, bounds, colour, anchor, std::move(onClick)});
    return handle;
}

UIHandle UI::CreateRectangle(glm::vec4 bounds, glm::vec4 colour, UIAnchor anchor)
{
    const UIHandle handle = mNextHandle++;
    mWidgets.push_back({handle, bounds, colour, anchor, {}});
    return handle;
}

void UI::Remove(UIHandle handle)
{
    std::erase_if(mWidgets, [=](const Widget& widget) { return widget.handle == handle; });
    if (mPressed == handle) mPressed = 0;
    if (mHovered == handle) mHovered = 0;
}

glm::vec4 UI::Bounds(const Widget& widget) const
{
    auto bounds = widget.bounds;
    if (widget.anchor == UIAnchor::BottomLeft)
        bounds.y += mViewport.height;
    return bounds;
}

UIHandle UI::HitTest(glm::vec2 position) const
{
    if (position.x < 0 || position.y < 0 || position.x >= mViewport.width || position.y >= mViewport.height)
        return 0;
    for (auto it = mWidgets.rbegin(); it != mWidgets.rend(); ++it)
    {
        if (!it->onClick) continue;
        const auto bounds = Bounds(*it);
        if (position.x >= bounds.x && position.x < bounds.x + bounds.z &&
            position.y >= bounds.y && position.y < bounds.y + bounds.w)
            return it->handle;
    }
    return 0;
}

bool UI::OnInput(const InputEvent& event)
{
    if (!event.IsMouse()) return false;
    // RenderView uses OpenGL's bottom-left viewport origin; input uses a top-left window origin.
    const glm::vec2 origin(mViewport.x, mWindowHeight - mViewport.y - mViewport.height);
    mHovered = HitTest(event.position - origin);
    const bool handled = mHovered != 0 || mPressed != 0;
    if (event.button == MouseButton::Left && event.type == InputType::MouseDown)
        mPressed = mHovered;
    if (event.button == MouseButton::Left && event.type == InputType::MouseUp)
    {
        const UIHandle clicked = mPressed == mHovered ? mPressed : 0;
        mPressed = 0;
        for (const auto& widget : mWidgets)
        {
            if (widget.handle != clicked) continue;
            // Copy before invoking: callbacks may remove their own controls.
            const auto callback = widget.onClick;
            callback();
            break;
        }
    }
    return handled;
}

void UI::ResetInput()
{
    mPressed = 0;
    mHovered = 0;
}

void UI::Rectangle(glm::vec4 bounds, glm::vec4 colour)
{
    if (bounds.z > 0.0f && bounds.w > 0.0f)
        mDraws.push_back({bounds, colour});
}

void UI::Render()
{
    // Persistent controls draw above transient world overlays.
    for (const auto& widget : mWidgets)
    {
        const f32 shade = !widget.onClick ? 1.0f : widget.handle == mPressed ? 0.6f :
                          widget.handle == mHovered ? 0.85f : 1.0f;
        Rectangle(Bounds(widget), glm::vec4(glm::vec3(widget.colour) * shade, widget.colour.a));
    }
    if (mDraws.empty() || mViewport.width <= 0 || mViewport.height <= 0) return;

    auto& renderer = Services::Get<Renderer>();
    const GLuint program = renderer.GetShaderProgram(mProgram)->program;
    const auto* mesh = renderer.GetMesh(renderer.GetSpriteMesh());
    const glm::mat4 projection = glm::ortho(0.0f, static_cast<f32>(mViewport.width),
                                          static_cast<f32>(mViewport.height), 0.0f);
    const GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    const GLboolean scissorTest = glIsEnabled(GL_SCISSOR_TEST);
    GLint viewport[4], scissor[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    glViewport(mViewport.x, mViewport.y, mViewport.width, mViewport.height);
    glScissor(mViewport.x, mViewport.y, mViewport.width, mViewport.height);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "uProjectionView"), 1, GL_FALSE, glm::value_ptr(projection));
    const GLint modelLocation = glGetUniformLocation(program, "uModel");
    const GLint colourLocation = glGetUniformLocation(program, "uColor");
    glBindVertexArray(mesh->vao);
    for (const auto& draw : mDraws)
    {
        const auto& r = draw.bounds;
        const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(r.x + r.z * 0.5f, r.y + r.w * 0.5f, 0.0f)) *
                                glm::scale(glm::mat4(1.0f), glm::vec3(r.z, r.w, 1.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(colourLocation, 1, glm::value_ptr(draw.colour));
        glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    if (!scissorTest) glDisable(GL_SCISSOR_TEST);
    if (depthTest) glEnable(GL_DEPTH_TEST);
}
