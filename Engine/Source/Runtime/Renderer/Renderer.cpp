//
// Created on 2026-06-25.
//
#include "pch.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <SDL3/SDL.h>

#include "FileSystem.h"
#include "MeshPrimitiveFactory.h"
#include "Core/Logs.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Destroy()
{
    SDL_GL_DestroyContext(mContext);
}

void Renderer::Init(const Application& application)
{
    mContext = SDL_GL_CreateContext(application.GetWindow());
    if (!mContext)
    {
        FATAL("SDL_GL_CreateContext failed! SDL_Error: {}", SDL_GetError());
    }

    SDL_GL_MakeCurrent(application.GetWindow(), mContext);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        FATAL("Failed to load OpenGL through GLAD!");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    mSpritePlane = CreateMesh(MeshPrimitiveFactory::CreateQuad());

    {
        auto vertexShader = Services::Get<FileSystem>().ReadAll(
            "C:\\Users\\Frohlund\\Documents\\Github\\UnicornEngine\\Engine\\Assets\\Shaders\\Line.vert");
        auto fragmentShader = Services::Get<FileSystem>().ReadAll(
            "C:\\Users\\Frohlund\\Documents\\Github\\UnicornEngine\\Engine\\Assets\\Shaders\\Line.frag");
        ASSERT(vertexShader, "Debug line vertex shader couldnt be loaded");
        ASSERT(fragmentShader, "Debug line fragment shader couldnt be loaded");

        ShaderProgramCreateInfo shaderProgramCreateInfo{};
        shaderProgramCreateInfo.vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShader.value());
        shaderProgramCreateInfo.fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader.value());
        mDebugLineShaderProgram = CreateProgram(shaderProgramCreateInfo);

        CreateDebugLineData();
    }

    {
        TextureCreateInfo missingTextureCreateInfo;
        missingTextureCreateInfo.width = 2;
        missingTextureCreateInfo.height = 2;
        missingTextureCreateInfo.channels = 4;
        constexpr uint8 defaultTexturePixels[] = {
            255, 0, 255, 255,
            0, 0, 0, 255,
            0, 0, 0, 255,
            255, 0, 255, 255
        };
        missingTextureCreateInfo.data.resize(16 * sizeof(uint8));
        memcpy_s(missingTextureCreateInfo.data.data(), 16 * sizeof(uint8), defaultTexturePixels, 16 * sizeof(uint8));

        mMissingTexture = CreateTexture(missingTextureCreateInfo);
    }

    {
        constexpr uint32_t size = 64;
        constexpr uint32_t channels = 4;

        TextureCreateInfo textureCreateInfo;
        textureCreateInfo.width = size;
        textureCreateInfo.height = size;
        textureCreateInfo.channels = channels;

        uint8_t texturePixels[size * size * channels];

        for (uint32_t y = 0; y < size; ++y)
        {
            for (uint32_t x = 0; x < size; ++x)
            {
                // Dark gray background
                uint8_t value = 180;

                // Subtle diagonal pattern
                if (((x + y) / 4) % 2 == 0)
                    value = 140;

                const uint32_t i = (y * size + x) * channels;

                texturePixels[i + 0] = value;
                texturePixels[i + 1] = value;
                texturePixels[i + 2] = value;
                texturePixels[i + 3] = 255;
            }
        }

        textureCreateInfo.data.resize(sizeof(texturePixels));
        memcpy(
            textureCreateInfo.data.data(),
            texturePixels,
            sizeof(texturePixels)
        );

        mDefaultAlbedoTexture = CreateTexture(textureCreateInfo);
    }

    {
        ShaderProgramCreateInfo programCreateInfo{};
        programCreateInfo.vertexShader = Services::Get<Renderer>().CompileShader(
            GL_VERTEX_SHADER,
            Services::Get<FileSystem>().ReadAll(
                "C:\\Users\\Frohlund\\Documents\\Github\\UnicornEngine\\Engine\\Assets\\Shaders\\Sprite.vert").value());
        programCreateInfo.fragmentShader = Services::Get<Renderer>().CompileShader(
            GL_FRAGMENT_SHADER,
            Services::Get<FileSystem>().ReadAll(
                "C:\\Users\\Frohlund\\Documents\\Github\\UnicornEngine\\Engine\\Assets\\Shaders\\Sprite.frag").value());
        auto spriteShaderProgram = Services::Get<Renderer>().CreateProgram(programCreateInfo);

        MaterialCreateInfo materialCreateInfo;
        materialCreateInfo.shaderProgram = spriteShaderProgram;
        materialCreateInfo.textures[0] = mDefaultAlbedoTexture;
        materialCreateInfo.textureCount = 1;
        mDefaultSpriteShader = CreateMaterial(materialCreateInfo);
    }
}

TextureHandle Renderer::CreateTexture(const TextureCreateInfo& createInfo)
{
    GLenum format = GL_RGB;

    if (createInfo.channels == 4)
    {
        format = GL_RGBA;
    }
    else if (createInfo.channels == 3)
    {
        format = GL_RGB;
    }
    else if (createInfo.channels == 1)
    {
        format = GL_RED;
    }

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        createInfo.width,
        createInfo.height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        createInfo.data.data()
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    Texture texture{};
    texture.id = textureId;
    texture.width = createInfo.width;
    texture.height = createInfo.height;

    auto handle = mTextureStorage.Allocate(std::move(texture));

    return handle;
}

void Renderer::DestroyTexture(TextureHandle& handle)
{
    auto* texture = mTextureStorage.Get(handle);
    if (!texture)
        return;

    glDeleteTextures(1, &texture->id);
    texture->height = 0;
    texture->width = 0;

    mTextureStorage.NullResources(handle);
}

GLuint Renderer::CompileShader(const GLuint shaderType, const ByteBuffer& data)
{
    const GLuint shader = glCreateShader(shaderType);

    const char* str = reinterpret_cast<const char*>(data.data());
    GLint length = static_cast<GLint>(data.size());

    glShaderSource(shader, 1, &str, &length);

    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        LOG_WARNING("Shader compilation failed: ", log);
    }

    return shader;
}

ShaderProgramHandle Renderer::CreateProgram(const ShaderProgramCreateInfo& createInfo)
{
    const GLuint glProgram = glCreateProgram();
    glAttachShader(glProgram, createInfo.vertexShader);
    glAttachShader(glProgram, createInfo.fragmentShader);
    glLinkProgram(glProgram);

    glDeleteShader(createInfo.vertexShader);
    glDeleteShader(createInfo.fragmentShader);

    ShaderProgram program{};
    program.program = glProgram;

    for (auto name : createInfo.uniformLocations)
    {
        program.uniformLocations[name] = glGetUniformLocation(program.program, name);
    }

    auto handle = mShaderProgramStorage.Allocate(std::move(program));
    return handle;
}

void Renderer::DestroyProgram(ShaderProgramHandle handle)
{
    auto* program = mShaderProgramStorage.Get(handle);
    if (!program)
        return;

    glDeleteProgram(program->program);

    mShaderProgramStorage.NullResources(handle);
}

MeshHandle Renderer::CreateMesh(const MeshCreateInfo& createInfo)
{
    Mesh mesh{};

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, createInfo.vertices.size() * sizeof(Vertex), createInfo.vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, createInfo.indices.size() * sizeof(uint32_t), createInfo.indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(
        3,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(
        4,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(4);

    mesh.indexCount = createInfo.indices.size();

    MeshHandle handle = mMeshStorage.Allocate(std::move(mesh));
    return handle;
}

void Renderer::DestroyMesh(MeshHandle& handle)
{
    Mesh* mesh = mMeshStorage.Get(handle);
    if (!mesh)
        return;

    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    mesh->indexCount = 0;

    mMeshStorage.NullResources(handle);
}

MaterialHandle Renderer::CreateMaterial(const MaterialCreateInfo& createInfo)
{
    RenderMaterial material{};
    material.shaderProgram = createInfo.shaderProgram;

    material.textures = createInfo.textures;
    material.textureCount = createInfo.textureCount;
    material.parameters = createInfo.parameters;
    material.parameterCount = createInfo.parameterCount;

    return mMaterialStorage.Allocate(std::move(material));
}

void Renderer::DrawDebugLine(RenderScene& scene, glm::vec3 from, glm::vec3 to, glm::vec4 color)
{
    DebugLineVertex p0{};
    p0.position = from;
    p0.color = color;

    DebugLineVertex p1{};
    p1.position = to;
    p1.color = color;

    scene.debugLineVertices[scene.debugLineVertexCount++] = p0;
    scene.debugLineVertices[scene.debugLineVertexCount++] = p1;
}

void Renderer::DrawDebug2DBox(RenderScene& scene, glm::vec2 center, glm::vec2 halfExtents, float y, glm::vec4 color)
{
    glm::vec3 p0(center.x - halfExtents.x, y, center.y - halfExtents.y);
    glm::vec3 p1(center.x + halfExtents.x, y, center.y - halfExtents.y);
    glm::vec3 p2(center.x + halfExtents.x, y, center.y + halfExtents.y);
    glm::vec3 p3(center.x - halfExtents.x, y, center.y + halfExtents.y);

    DrawDebugLine(scene, p0, p1, color);
    DrawDebugLine(scene, p1, p2, color);
    DrawDebugLine(scene, p2, p3, color);
    DrawDebugLine(scene, p3, p0, color);
}

void Renderer::Render(const RenderScene& scene, const RenderView& view)
{
    glViewport(view.viewport.x, view.viewport.y, view.viewport.width, view.viewport.height);
    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& data : scene.data)
    {
        auto* mesh = mMeshStorage.Get(data.mesh);
        auto* material = mMaterialStorage.Get(data.material);
        auto* shaderProgram = mShaderProgramStorage.Get(material->shaderProgram);

        glUseProgram(shaderProgram->program);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram->program, "uModel"), 1, GL_FALSE, &data.transform[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram->program, "uProjectionView"), 1, GL_FALSE,
                           &view.projectionView[0][0]);
        glUniform4fv(glGetUniformLocation(shaderProgram->program, "uUvRect"), 1, glm::value_ptr(data.uvRect));


        for (auto i = 0; i < material->textureCount; ++i)
        {
            auto textureHandle = data.instanceData.textureOverrides[i]
            ? data.instanceData.textures[i]
            : material->textures[i];

            auto* texture = mTextureStorage.Get(textureHandle);
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture->id);
        }

        for (auto i = 0; i < material->parameterCount; ++i)
        {
            auto parameter = data.instanceData.parameterOverrides[i]
            ? data.instanceData.parameters[i]
            : material->parameters[i];

            auto it = shaderProgram->uniformLocations.find(parameter.name);
            ASSERT(it != shaderProgram->uniformLocations.end(), "Parameter not found");

            SetUniform(it->second, parameter.value);
        }

        glBindVertexArray(mesh->vao);

        glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
    }

    ASSERT(scene.debugLineVertexCount < 1024, "Too many debug lines");

    glDisable(GL_DEPTH_TEST);

    auto* shaderProgram = mShaderProgramStorage.Get(mDebugLineShaderProgram);

    glUseProgram(shaderProgram->program);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram->program, "uProjectionView"), 1, GL_FALSE,
                       &view.projectionView[0][0]);

    glBindVertexArray(mDebugLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mDebugLineVBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        scene.debugLineVertexCount * sizeof(DebugLineVertex),
        scene.debugLineVertices
    );

    glDrawArrays(
        GL_LINES,
        0,
        scene.debugLineVertexCount);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void Renderer::Swap(const Application& application)
{
    SDL_GL_SwapWindow(application.GetWindow());
}

void Renderer::CreateDebugLineData()
{
    glGenVertexArrays(1, &mDebugLineVAO);
    glGenBuffers(1, &mDebugLineVBO);

    glBindVertexArray(mDebugLineVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mDebugLineVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        1024 * sizeof(DebugLineVertex),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(DebugLineVertex),
        reinterpret_cast<void*>(offsetof(DebugLineVertex, position))
    );

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(DebugLineVertex),
        reinterpret_cast<void*>(offsetof(DebugLineVertex, color))
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::SetUniform(GLuint location, const UniformValue& value)
{
    std::visit([&](auto&& type)
    {
        using T = std::decay_t<decltype(type)>;

        if constexpr (std::is_same_v<T, float>)
        {
            glUniform1f(location, type);
        }
        else if constexpr (std::is_same_v<T, glm::vec2>)
        {
            glUniform2fv(location, 1, glm::value_ptr(type));
        }
        else if constexpr (std::is_same_v<T, glm::vec3>)
        {
            glUniform3fv(location, 1, glm::value_ptr(type));
        }
        else if constexpr (std::is_same_v<T, glm::vec4>)
        {
            glUniform4fv(location, 1, glm::value_ptr(type));
        }

    }, value);
}
