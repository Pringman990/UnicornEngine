#pragma once
#include <span>

#include "SDL3/SDL_opengl.h"

struct Texture
{
    GLuint id;
    uint32_t width;
    uint32_t height;
};

struct SpriteInstance
{
    Texture* texture;

    int32_t x;
    int32_t y;
};

struct RenderScene
{
    std::span<const SpriteInstance> sprites;
};

struct Viewport
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

struct RenderView
{
    Viewport viewport;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Render(RenderScene scene, RenderView view);

private:
};
