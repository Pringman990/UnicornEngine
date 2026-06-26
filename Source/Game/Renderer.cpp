//
// Created on 2026-06-25.
//

#include "Renderer.h"

#include "glad/glad.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Render(RenderScene scene, RenderView view)
{
    glViewport(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height);
    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& sprite : scene.sprites)
    {
        glUseProgram(shaderProgram);
        glUniform2f(positionLocation, sprite.x, sprite.y);
        glUniform2f(windowSizeLocation, view.viewport.width, view.viewport.height);
        glBindVertexArray(vao);

        glBindTexture(GL_TEXTURE_2D, playerTexture);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

}
