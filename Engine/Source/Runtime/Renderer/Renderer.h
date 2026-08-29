#pragma once
#include <glad/glad.h>

#include "../Application.h"
#include "Core/ResourceStorage.h"

#define MAX_TEXTURE_SLOTS 16
#define MAX_MATERIAL_PARAMETERS 16

using UniformValue = Variant<
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4
    >;

struct ShaderProgramCreateInfo
{
    GLuint vertexShader;
    GLuint fragmentShader;

    List<const char*> uniformLocations;
};

struct Vertex
{
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 normal = glm::vec3(0.0);
    glm::vec3 tangent = glm::vec3(0.0);
    glm::vec3 bitangent = glm::vec3(0.0);
    glm::vec2 uv = glm::vec2(0);
};

struct DebugLineVertex
{
    glm::vec3 position = glm::vec3(0.0);
    glm::vec4 color = glm::vec4(1.0f,0.0f,0.0f,1.0f);
};

struct MeshCreateInfo
{
    List<Vertex> vertices;
    List<uint32_t> indices;
};

struct TextureCreateInfo
{
    ByteBuffer data;
    uint32 channels;
    uint32_t width;
    uint32_t height;
};

struct Texture
{
    GLuint id;

    uint32_t width;
    uint32_t height;
};

using TextureHandle = GenerationHandle<Texture>;

struct Mesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    uint32_t indexCount = 0;
};

using MeshHandle = GenerationHandle<Mesh>;

struct ShaderProgram
{
    GLuint program = 0;

    UnorderedMap<String, GLint> uniformLocations;
};

using ShaderProgramHandle = GenerationHandle<ShaderProgram>;

struct MaterialParameter
{
    const char* name;
    UniformValue value;
};

struct MaterialCreateInfo
{
    ShaderProgramHandle shaderProgram;

    Array<TextureHandle, MAX_TEXTURE_SLOTS> textures;
    uint32 textureCount = 0;

    Array<MaterialParameter, MAX_MATERIAL_PARAMETERS> parameters;
    uint32 parameterCount = 0;
};

struct RenderMaterial
{
    ShaderProgramHandle shaderProgram;

    Array<TextureHandle, MAX_TEXTURE_SLOTS> textures;
    uint32 textureCount = 0;

    Array<MaterialParameter, MAX_MATERIAL_PARAMETERS> parameters;
    uint32 parameterCount = 0;
};

using MaterialHandle = GenerationHandle<RenderMaterial>;

struct MaterialInstanceData
{
    Array<TextureHandle, MAX_TEXTURE_SLOTS> textures;
    Array<bool,  MAX_TEXTURE_SLOTS> textureOverrides;

    Array<MaterialParameter, MAX_MATERIAL_PARAMETERS> parameters;
    Array<bool, MAX_MATERIAL_PARAMETERS> parameterOverrides;
};

struct RenderData
{
    MeshHandle mesh;
    MaterialHandle material;

    MaterialInstanceData instanceData;

    glm::mat4x4 transform{};

    glm::vec4 uvRect{0.0f, 0.0f, 1.0f, 1.0f};
};

struct RenderScene
{
    std::vector<RenderData> data;
    DebugLineVertex debugLineVertices[1024];
    size_t debugLineVertexCount = 0;
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
    glm::mat4x4 projectionView;
};

class Renderer
{
    INIT_SERVICE(Renderer)
public:
    Renderer();
    ~Renderer();

    void Destroy();
    void Init(const Application& application);

    NODISC TextureHandle CreateTexture(const TextureCreateInfo& createInfo);
    void DestroyTexture(TextureHandle& handle);

    NODISC GLuint CompileShader(GLuint shaderType, const ByteBuffer& data);

    NODISC ShaderProgramHandle CreateProgram(const ShaderProgramCreateInfo& createInfo);
    void DestroyProgram(ShaderProgramHandle handle);

    NODISC MeshHandle CreateMesh(const MeshCreateInfo& createInfo);
    void DestroyMesh(MeshHandle& handle);

    NODISC MaterialHandle CreateMaterial(const MaterialCreateInfo& createInfo);

    void DrawDebugLine(RenderScene& scene, glm::vec3 from, glm::vec3 to, glm::vec4 color);
    void DrawDebug2DBox(RenderScene& scene, glm::vec2 center, glm::vec2 halfExtents, float y, glm::vec4 color);

    void Render(const RenderScene& scene, const RenderView& view);

    void Swap(const Application& application);

    NODISC MeshHandle GetSpriteMesh() const { return mSpritePlane; }

    NODISC Mesh* GetMesh(MeshHandle handle) { return mMeshStorage.Get(handle); }
    NODISC const RenderMaterial* GetMaterial(MaterialHandle handle) const { return mMaterialStorage.Get(handle); }
    NODISC Texture* GetTexture(TextureHandle handle) { return mTextureStorage.Get(handle); }
    NODISC ShaderProgram* GetShaderProgram(ShaderProgramHandle handle) { return mShaderProgramStorage.Get(handle); }
    NODISC MaterialHandle GetDefaultSpriteMaterial() const {return mDefaultSpriteShader;};

    NODISC TextureHandle GetMissingTexture() const { return mMissingTexture;};

private:
    void CreateDebugLineData();

    void SetUniform(GLuint location, const UniformValue& value);

private:
    SDL_GLContext mContext;

    MeshHandle mSpritePlane;
    MaterialHandle mDefaultSpriteShader;

    ShaderProgramHandle mDebugLineShaderProgram;
    GLuint mDebugLineVAO;
    GLuint mDebugLineVBO;

    TextureHandle mDefaultAlbedoTexture;
    TextureHandle mMissingTexture;

    ResourceStorage<Mesh> mMeshStorage;
    ResourceStorage<RenderMaterial> mMaterialStorage;
    ResourceStorage<Texture> mTextureStorage;
    ResourceStorage<ShaderProgram> mShaderProgramStorage;
};
