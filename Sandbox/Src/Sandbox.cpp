//
// Created on 2026-07-11.
//
#include "pch.h"
#include "Sandbox.h"

#include "FileSystem.h"
#include "SpriteSheetConverter.h"
#include "TextureLoader.h"
#include "Core/Logs.h"
#include "Renderer/Renderer.h"
#include "ECS/WorldManager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "Physics/DetectionTest.h"
#include "Renderer/MeshPrimitiveFactory.h"

Prefab GCubePrefab;
Prefab GGrassPrefab;

MaterialHandle GSingleColorMaterial;

struct Transform
{
    REFL_ID("d2a2f98a-d618-4fb1-9d50-70f6c3c10f33")

    glm::vec3 position{0};
    glm::quat rotation{1, 0, 0, 0};
    glm::vec3 scale{1.0f};

    glm::mat4 world = glm::mat4(1.0f);
};

struct CharacterController
{
    REFL_ID("d2d2f98a-d618-4fb1-9d51-70f6c3c10f33")

    float moveSpeed = 3.0f;
    float stepHeight = 0.15f;
    glm::vec2 velocity{0.0f, 0.0f};
};

struct Model
{
    List<MeshHandle> meshes;
    List<MaterialHandle> materials;
};

struct MeshRenderer
{
    REFL_ID("d2d2f98a-d618-4fb1-9d50-70f6c3c10f33")

    Model model{};
};

struct SpriteRenderer
{
    REFL_ID("a2d2f98a-d628-4fb1-9d50-70f6c3c10f33")

    MaterialHandle material;
    TextureHandle texture;

    glm::vec4 rect{};

    float pixelsPerUnit = 100.0f;
    glm::vec2 pivot{0.5f, 0.0f};
};

struct AnimationSequence2D
{
    List<glm::vec4> rects;
    float frameDuration = 0.12f;
};

AnimationSequence2D PlayerIdleSequence;
AnimationSequence2D PlayerRunUpSequence;
AnimationSequence2D PlayerRunDownSequence;
AnimationSequence2D PlayerRunLeftSequence;
AnimationSequence2D PlayerRunRightSequence;

TextureHandle PlayerIdleTexture;
TextureHandle PlayerRunTexture;

struct Animator2D
{
    REFL_ID("a2d2f98a-d618-4fb1-9d50-70a6c3c10f33")

    AnimationSequence2D* sequence{};
    float currentTime = 0.0f;
    uint32 currentFrame = 0;
};

struct Camera
{
    REFL_ID("a2d2f98a-d618-4fb1-9d50-70f6c3c10f33")

    float fov = glm::radians(60.0f);
    float aspect = 16.0f / 9.0f;

    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
};

struct BoxCollider2D
{
    REFL_ID("a2d2f98a-d618-4fb1-9d50-70f6c3c10f31")

    glm::vec2 offset{};
    glm::vec2 halfExtents{};

    bool walkable = false;
};

struct HitBox
{
};

void RenderSystem(World& world, const FrameData& frameData);
void MovementSystem(World& world, const FrameData& frameData);
void CameraSystem(World& world, const FrameData& frameData);
void CollisionSystem(World& world, const FrameData& frameData);
void Animation2DSystem(World& world, const FrameData& frameData);
void AnimationDirectionSwitchSystem(World& world, const FrameData& frameData);
void TransformSystem(World& world, const FrameData& frameData);

AABB GetAABB(const glm::mat4& matrix, const BoxCollider2D& collider)
{
    const auto position = glm::vec3(matrix[3]);

    glm::vec2 center = glm::vec2(position.x, position.z) + collider.offset;
    return {center - collider.halfExtents, center + collider.halfExtents};
}

void Sandbox::Init()
{
    {
        auto& refl = Services::Get<Refl::ReflectionRegistry>();

        refl.RegisterType<Relationship>(
            Refl::ClassBuilder<Relationship>("Relationship")
            .Build()
        );

        refl.RegisterType<MeshRenderer>(
            Refl::ClassBuilder<MeshRenderer>("Mesh Renderer")
            .Property("mesh", &MeshRenderer::model)
            .Build()
        );

        refl.RegisterType<Camera>(
            Refl::ClassBuilder<Camera>("Camera")
            .Build()
        );

        refl.RegisterType<SpriteRenderer>(
            Refl::ClassBuilder<SpriteRenderer>("Sprite Renderer")
            .Build()
        );

        refl.RegisterType<CharacterController>(
            Refl::ClassBuilder<CharacterController>("Character Controller")
            .Build()
        );

        refl.RegisterType<BoxCollider2D>(
            Refl::ClassBuilder<BoxCollider2D>("BoxCollider2D")
            .Build()
        );

        refl.RegisterType<Animator2D>(
            Refl::ClassBuilder<Animator2D>("Animator2D")
            .Build()
        );

        refl.RegisterType<Transform>(
            Refl::ClassBuilder<Transform>("Transform")
            .Property("Position", &Transform::position)
            .Property("Rotation", &Transform::rotation)
            .Property("Scale", &Transform::scale)
            .Build()
        );
    }

    Services::Get<FileSystem>().AddMount("engine://", "Engine/");
    Services::Get<FileSystem>().AddMount("game://", "Sandbox/");

    {
        ShaderProgramCreateInfo shaderProgramCreateInfo{};
        shaderProgramCreateInfo.uniformLocations.push_back("uColor");
        shaderProgramCreateInfo.vertexShader = Services::Get<Renderer>().CompileShader(
            GL_VERTEX_SHADER,
            Services::Get<FileSystem>().ReadAll(
                "engine://Assets/Shaders/Sprite.vert").value());
        shaderProgramCreateInfo.fragmentShader = Services::Get<Renderer>().CompileShader(
            GL_FRAGMENT_SHADER,
            Services::Get<FileSystem>().ReadAll(
                                           "engine://Assets/Shaders/Sprite_Single_Color.frag")
                                       .value());
        auto spriteShaderProgram = Services::Get<Renderer>().CreateProgram(shaderProgramCreateInfo);

        MaterialCreateInfo materialCreateInfo{};
        materialCreateInfo.shaderProgram = spriteShaderProgram;
        materialCreateInfo.parameterCount = 1;
        materialCreateInfo.parameters[0].name = "uColor";
        materialCreateInfo.parameters[0].value = glm::vec4(0.071f, 0.431f, 0.071f, 1.0f);
        GSingleColorMaterial = Services::Get<Renderer>().CreateMaterial(materialCreateInfo);
    }

    {
        auto textureData = Services::Get<FileSystem>().ReadAll(
                                                          "game://Assets/Player/Sword_Idle_full.png")
                                                      .
                                                      value();
        auto loadedTexture = TextureLoader::LoadTexture(textureData);

        TextureCreateInfo textureCreateInfo{};
        textureCreateInfo.data = loadedTexture.data;
        textureCreateInfo.width = loadedTexture.width;
        textureCreateInfo.height = loadedTexture.height;
        textureCreateInfo.channels = loadedTexture.channels;

        PlayerIdleTexture = Services::Get<Renderer>().CreateTexture(textureCreateInfo);
    }

    {
        auto textureData = Services::Get<FileSystem>().ReadAll(
                                                          "game://Assets/Player/Sword_Run_full.png")
                                                      .
                                                      value();
        auto loadedTexture = TextureLoader::LoadTexture(textureData);

        TextureCreateInfo textureCreateInfo{};
        textureCreateInfo.data = loadedTexture.data;
        textureCreateInfo.width = loadedTexture.width;
        textureCreateInfo.height = loadedTexture.height;
        textureCreateInfo.channels = loadedTexture.channels;

        PlayerRunTexture = Services::Get<Renderer>().CreateTexture(textureCreateInfo);
    }

    {
        auto textureData = Services::Get<FileSystem>().ReadAll(
                                                          "game://Assets/Player/Sword_Idle_full.png")
                                                      .
                                                      value();
        auto loadedTexture = TextureLoader::LoadTexture(textureData);
        auto sheet = SpriteSheetConverter::Convert(loadedTexture.data, loadedTexture.width, loadedTexture.height,
                                                   loadedTexture.channels);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[0]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[1]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[2]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[3]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[4]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[5]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[6]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[7]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[8]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[9]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[10]);
        PlayerIdleSequence.rects.emplace_back(sheet.frames[11]);
    }
    {
        auto textureData = Services::Get<FileSystem>().ReadAll(
                                                          "game://Assets/Player/Sword_Run_full.png")
                                                      .
                                                      value();
        auto loadedTexture = TextureLoader::LoadTexture(textureData);
        auto sheet = SpriteSheetConverter::Convert(loadedTexture.data, loadedTexture.width, loadedTexture.height,
                                                   loadedTexture.channels);

        PlayerRunDownSequence.rects.emplace_back(sheet.frames[0]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[1]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[2]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[3]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[4]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[5]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[6]);
        PlayerRunDownSequence.rects.emplace_back(sheet.frames[7]);

        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[8]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[9]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[10]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[11]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[12]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[13]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[14]);
        PlayerRunLeftSequence.rects.emplace_back(sheet.frames[15]);

        PlayerRunRightSequence.rects.emplace_back(sheet.frames[16]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[17]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[18]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[19]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[20]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[21]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[22]);
        PlayerRunRightSequence.rects.emplace_back(sheet.frames[23]);

        PlayerRunUpSequence.rects.emplace_back(sheet.frames[24]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[25]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[26]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[27]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[28]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[29]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[30]);
        PlayerRunUpSequence.rects.emplace_back(sheet.frames[31]);
    }

    auto& wrlMan = Services::Get<WorldManager>();
    World& world = wrlMan.GetActiveWorld();

    MaterialHandle spriteDefaultMaterial = Services::Get<Renderer>().GetDefaultSpriteMaterial();

    {
        auto cube = world.CreateEntity();
        auto* trans = world.AddComponent<Transform>(cube);
        trans->position.x = 1.0f;
        auto* meshRenderer = world.AddComponent<MeshRenderer>(cube);
        meshRenderer->model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreateCube()));

        meshRenderer->model.materials.push_back(spriteDefaultMaterial);

        auto* collider = world.AddComponent<BoxCollider2D>(cube);
        collider->halfExtents = glm::vec2(0.5f, 0.5f);
        collider->offset = glm::vec2();

        PrefabBuilder prefabBuilder;
        uint32 root = prefabBuilder.CreateRoot();

        {
            prefabBuilder.Add(root, Transform());

            MeshRenderer prefabMesh;
            prefabMesh.model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreateCube()));
            prefabMesh.model.materials.push_back(spriteDefaultMaterial);
            prefabBuilder.Add(root, prefabMesh);

            BoxCollider2D prefabCollider;
            prefabCollider.halfExtents = glm::vec2(0.5f, 0.5f);
            prefabCollider.offset = glm::vec2();
            prefabBuilder.Add(root, prefabCollider);
        }

        {
            uint32 child = prefabBuilder.CreateEntity(root);
            Transform transform;
            transform.position.x = -0.3f;
            transform.position.y = 2.0f;
            prefabBuilder.Add(child, transform);

            MeshRenderer prefabMesh;
            prefabMesh.model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreateCube()));
            prefabMesh.model.materials.push_back(spriteDefaultMaterial);
            prefabBuilder.Add(child, prefabMesh);

            BoxCollider2D prefabCollider;
            prefabCollider.halfExtents = glm::vec2(0.5f, 0.5f);
            prefabCollider.offset = glm::vec2();
            prefabBuilder.Add(child, prefabCollider);
        }

        GCubePrefab = prefabBuilder.Build();
    }

    {
        Entity secondCube = world.Instantiate(GCubePrefab);
        auto* secondCubeTransform = world.GetComponent<Transform>(secondCube);
        secondCubeTransform->position.x = -1.0f;
    }

    // {
    //     Entity secondCube = world.Instantiate(GCubePrefab);
    //     auto* secondCubeTransform = world.GetComponent<Transform>(secondCube);
    //     secondCubeTransform->position.z = -1.0f;
    // }

    {
        auto plane = world.CreateEntity();
        Transform* trans = world.AddComponent<Transform>(plane);
        trans->scale = glm::vec3(10.0f, 1.f, 10.f);

        MeshRenderer* meshRenderer = world.AddComponent<MeshRenderer>(plane);
        meshRenderer->model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreatePlane()));

        meshRenderer->model.materials.push_back(GSingleColorMaterial);

        auto* collider = world.AddComponent<BoxCollider2D>(plane);
        collider->halfExtents = glm::vec2(5.0f, 5.0f);
        collider->offset = glm::vec2();
        collider->walkable = true;
    }

    {
        auto plane = world.CreateEntity();
        Transform* trans = world.AddComponent<Transform>(plane);
        trans->scale = glm::vec3(1.0f, 1.f, 1.f);
        trans->position.x = -5.5f;
        trans->position.y = 0.15f;

        MeshRenderer* meshRenderer = world.AddComponent<MeshRenderer>(plane);
        meshRenderer->model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreatePlane()));

        meshRenderer->model.materials.push_back(GSingleColorMaterial);

        auto* collider = world.AddComponent<BoxCollider2D>(plane);
        collider->halfExtents = glm::vec2(0.50f, 0.50f);
        collider->offset = glm::vec2();
        collider->walkable = true;
    }

    {
        auto plane = world.CreateEntity();
        Transform* trans = world.AddComponent<Transform>(plane);
        trans->scale = glm::vec3(1.0f, 1.f, 1.f);
        trans->position.x = -6.5f;
        trans->position.y = 0.30f;

        MeshRenderer* meshRenderer = world.AddComponent<MeshRenderer>(plane);
        meshRenderer->model.meshes.push_back(Services::Get<Renderer>().CreateMesh(MeshPrimitiveFactory::CreatePlane()));

        meshRenderer->model.materials.push_back(GSingleColorMaterial);

        auto* collider = world.AddComponent<BoxCollider2D>(plane);
        collider->halfExtents = glm::vec2(0.50f, 0.50f);
        collider->offset = glm::vec2();
        collider->walkable = true;
    }

    // Player
    {
        auto sprite = world.CreateEntity();
        Transform* transform = world.AddComponent<Transform>(sprite);
        transform->scale *= 2.f;
        world.AddComponent<CharacterController>(sprite);
        auto* spriteRenderer = world.AddComponent<SpriteRenderer>(sprite);

        spriteRenderer->material = spriteDefaultMaterial;
        spriteRenderer->texture = PlayerIdleTexture;
        spriteRenderer->rect = glm::vec4(0, 0, 1, 1);

        auto* collider = world.AddComponent<BoxCollider2D>(sprite);
        collider->halfExtents = glm::vec2(0.16f, 0.16f);
        collider->offset = glm::vec2(0.0f, 0.06f);

        auto* animator = world.AddComponent<Animator2D>(sprite);
        animator->sequence = &PlayerIdleSequence;
    }

    {
        auto textureData = Services::Get<FileSystem>().ReadAll(
            "game://Assets/plants.png").value();
        auto loadedTexture = TextureLoader::LoadTexture(textureData);
        auto sheet = SpriteSheetConverter::Convert(loadedTexture.data, loadedTexture.width, loadedTexture.height,
                                                   loadedTexture.channels);

        TextureCreateInfo textureCreateInfo{};
        textureCreateInfo.data = loadedTexture.data;
        textureCreateInfo.width = loadedTexture.width;
        textureCreateInfo.height = loadedTexture.height;
        textureCreateInfo.channels = loadedTexture.channels;
        auto textureHandle = Services::Get<Renderer>().CreateTexture(textureCreateInfo);

        PrefabBuilder prefabBuilder;
        uint32 root = prefabBuilder.CreateRoot();

        {
            Transform transform;
            transform.scale = glm::vec3(2.5f, 2.5f, 2.5f);
            prefabBuilder.Add(root, transform);

            SpriteRenderer spriteRenderer;
            spriteRenderer.material = spriteDefaultMaterial;
            spriteRenderer.texture = textureHandle;
            spriteRenderer.rect = sheet.frames[5];
            prefabBuilder.Add(root, spriteRenderer);
        }

        {
            uint32 child = prefabBuilder.CreateEntity(root);

            Transform transform;
            transform.rotation = glm::vec3(0.0f, 90.0f, 0.0f);
            prefabBuilder.Add(child, transform);

            SpriteRenderer spriteRenderer;
            spriteRenderer.material = spriteDefaultMaterial;
            spriteRenderer.texture = textureHandle;
            spriteRenderer.rect = sheet.frames[5];
            prefabBuilder.Add(child, spriteRenderer);
        }

        GGrassPrefab = prefabBuilder.Build();
    }

    {
        float minX = -5.0f;
        float minY = -5.0f;
        float maxX = 5.0f;
        float maxY = 5.0f;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution xDist(minX, maxX);
        std::uniform_real_distribution yDist(minY, maxY);

        for (uint32 i = 0; i < 300; ++i)
        {
            auto entity = world.Instantiate(GGrassPrefab);
            auto* transform = world.GetComponent<Transform>(entity);
            transform->position.x = xDist(gen);
            transform->position.z = yDist(gen);
        }
    }

    {
        auto camera = world.CreateEntity();
        world.AddComponent<Camera>(camera);
        world.AddComponent<Transform>(camera);
    }

    auto& sysMan = Services::Get<SystemManager>();
    {
        System system{};
        system.name = "Render System";
        system.stage = SystemStage::SS_PostTick;
        system.Tick = &RenderSystem;
        sysMan.RegisterSystem(std::move(system));
    }
    {
        System system{};
        system.name = "Movement System";
        system.stage = SystemStage::SS_Tick;
        system.Tick = &MovementSystem;
        sysMan.RegisterSystem(std::move(system));
    }
    {
        System system{};
        system.name = "Camera System";
        system.stage = SystemStage::SS_Tick;
        system.Tick = &CameraSystem;
        sysMan.RegisterSystem(std::move(system));
    }
    {
        System system{};
        system.name = "Collision System";
        system.stage = SystemStage::SS_PostTick;
        system.Tick = &CollisionSystem;
        sysMan.RegisterSystem(std::move(system));
    }

    {
        System system{};
        system.name = "Animation2D System";
        system.stage = SystemStage::SS_PostTick;
        system.Tick = &Animation2DSystem;
        sysMan.RegisterSystem(std::move(system));
    }

    {
        System system{};
        system.name = "Animation Direction Switch System";
        system.stage = SystemStage::SS_PostTick;
        system.Tick = &AnimationDirectionSwitchSystem;
        sysMan.RegisterSystem(std::move(system));
    }

    {
        System system{};
        system.name = "Transform System";
        system.stage = SystemStage::SS_PostTick;
        system.Tick = &TransformSystem;
        sysMan.RegisterSystem(std::move(system));
    }
}

void Sandbox::Tick(SystemManager& systemManager, const FrameData& frameData, World& world)
{
    systemManager.TickSystems(world, frameData);
}

static glm::mat4x4 GetMatrix(const Transform& T)
{
    return glm::translate(glm::mat4(1.0f), T.position) *
        glm::mat4_cast(T.rotation) *
        glm::scale(glm::mat4(1.0f), T.scale);
}

void RenderSystem(World& world, const FrameData& frameData)
{
    auto& renderer = Services::Get<Renderer>();
    RenderScene scene;

    for (const auto& [entity, mesh, transform] : world.Query<MeshRenderer, Transform>())
    {
        for (uint32 i = 0; i < mesh.model.meshes.size(); ++i)
        {
            RenderData data;
            data.mesh = mesh.model.meshes[i];
            data.material = mesh.model.materials[i];

            data.transform = transform.world;
            scene.data.push_back(data);
        }
    }

    for (const auto& [entity, sprite, transform] : world.Query<SpriteRenderer, Transform>())
    {
        RenderData data;
        data.mesh = renderer.GetSpriteMesh();
        data.material = sprite.material;

        MaterialInstanceData instanceData;
        instanceData.textures[0] = sprite.texture ? sprite.texture : renderer.GetMissingTexture();
        instanceData.textureOverrides[0] = true;

        data.instanceData = instanceData;

        auto* texture = renderer.GetTexture(instanceData.textures[0]);

        glm::vec2 size(
            sprite.rect.z / sprite.pixelsPerUnit,
            sprite.rect.w / sprite.pixelsPerUnit
        );

        glm::vec2 pivotOffset = (glm::vec2(0.5f) - sprite.pivot) * size;

        data.transform = transform.world *
            glm::translate(glm::mat4(1.0f), glm::vec3(pivotOffset, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

        const float u0 = sprite.rect.x / texture->width;
        const float v0 = sprite.rect.y / texture->height;
        const float u1 = (sprite.rect.x + sprite.rect.z) / texture->width;
        const float v1 = (sprite.rect.y + sprite.rect.w) / texture->height;

        // Image coordinates are top-left origin.
        // GPU UV rectangle uses bottom-to-top Y ordering.
        // uvRect = (uMin, vMax, uMax, vMin)
        data.uvRect = glm::vec4(u0, v1, u1, v0);

        scene.data.push_back(data);
    }

    for (const auto& [entity, transform, collider] : world.Query<Transform, BoxCollider2D>())
    {
        const auto position = glm::vec3(transform.world[3]);
        renderer.DrawDebug2DBox(scene, glm::vec2(position.x, position.z), collider.halfExtents, position.y,
                                glm::vec4(1, 0, 0, 1));
    }

    glm::mat4x4 projView{};
    for (const auto& [_, cam, trans] : world.Query<Camera, Transform>())
    {
        glm::mat4 worldMatrix = trans.world;
        glm::mat4 view = glm::inverse(worldMatrix);
        glm::mat4 proj = glm::perspective(cam.fov, cam.aspect, cam.nearPlane, cam.farPlane);
        projView = proj * view;
        break;
    }

    auto& app = Services::Get<Application>();

    RenderView view{};
    view.viewport = {0, 0, app.GetInfo().viewportWidth, app.GetInfo().viewportHeight};
    view.projectionView = projView;
    renderer.Render(scene, view);
}

void MovementSystem(World& world, const FrameData& frameData)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    for (const auto& [entity, controller, transform] : world.Query<CharacterController, Transform>())
    {
        glm::vec3 deltaPosition{0.0f};

        if (keys[SDL_SCANCODE_W])
            deltaPosition.z -= 1.0f;

        if (keys[SDL_SCANCODE_S])
            deltaPosition.z += 1.0f;

        if (keys[SDL_SCANCODE_A])
            deltaPosition.x -= 1.0f;

        if (keys[SDL_SCANCODE_D])
            deltaPosition.x += 1.0f;

        controller.velocity = glm::vec2(0.0f);

        if (glm::length2(deltaPosition) > 0.0f)
        {
            deltaPosition = glm::normalize(deltaPosition);
            controller.velocity = glm::vec2(deltaPosition.x, deltaPosition.z) * controller.moveSpeed;
        }
    }
}

void CollisionSystem(World& world, const FrameData& frameData)
{
    for (const auto& [entity, controller, transform, collider] : world.Query<
             CharacterController, Transform, BoxCollider2D>())
    {
        auto worldPosition = glm::vec3(transform.world[3]);
        glm::vec2 movement = controller.velocity * frameData.deltaTime;
        worldPosition.x += movement.x;
        worldPosition.z += movement.y;

        float targetElevation = transform.position.y;
        bool foundGround = false;

        // Build collision shape at the proposed position.
        glm::mat4 proposedWorld = transform.world;
        proposedWorld[3] = glm::vec4(worldPosition, 1.0f);

        AABB controllerBox = GetAABB(proposedWorld, collider);

        for (const auto& [otherEntity, otherTransform, otherCollider] : world.Query<Transform, BoxCollider2D>())
        {
            if (entity == otherEntity)
                continue;

            AABB otherBox = GetAABB(otherTransform.world, otherCollider);

            CollisionResult result;
            if (!DetectionTest::GetCollision(
                controllerBox,
                otherBox,
                result))
            {
                continue;
            }

            float elevationDelta = otherTransform.position.y - worldPosition.y;
            if (!otherCollider.walkable && elevationDelta == 0)
            {
                worldPosition.x += result.normal.x * result.penetration;
                worldPosition.z += result.normal.y * result.penetration;
            }

            if (otherCollider.walkable && elevationDelta > controller.stepHeight)
            {
                worldPosition.x += result.normal.x * result.penetration;
                worldPosition.z += result.normal.y * result.penetration;
            }
            else
            {
                targetElevation = otherTransform.position.y;
                foundGround = true;
            }

            if (foundGround)
                worldPosition.y = targetElevation;
            else
                worldPosition.y = 0.0f;

            proposedWorld[3] = glm::vec4(worldPosition, 1.0f);
            controllerBox = GetAABB(proposedWorld, collider);
        }

        worldPosition.y = targetElevation;

        auto relationship = world.GetComponent<Relationship>(entity);
        if (relationship->parent != InvalidEntity)
        {
            auto* parentTransform = world.GetComponent<Transform>(relationship->parent);

            glm::vec4 localPosition = glm::inverse(parentTransform->world) * glm::vec4(worldPosition, 1.0f);
            transform.position = localPosition;
        }
        else
        {
            transform.position = worldPosition;
        }
    }
}

void CameraSystem(World& world, const FrameData& frameData)
{
    Transform* playerTransform = nullptr;

    for (const auto& [entity, controller, transform] :
         world.Query<CharacterController, Transform>())
    {
        playerTransform = &transform;
        break;
    }

    if (!playerTransform)
        return;

    for (const auto& [entity, camera, cameraTransform] :
         world.Query<Camera, Transform>())
    {
        cameraTransform.position =
            playerTransform->position +
            glm::vec3(0.0f, 2.5f, 3.0f);

        glm::vec3 direction =
            glm::normalize(
                playerTransform->position -
                cameraTransform.position
            );

        cameraTransform.rotation =
            glm::quatLookAt(
                direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
    }
}

void Animation2DSystem(World& world, const FrameData& frameData)
{
    for (const auto& [entity, sprite, animator] : world.Query<SpriteRenderer, Animator2D>())
    {
        animator.currentTime += frameData.deltaTime;

        while (animator.currentTime >= animator.sequence->frameDuration)
        {
            animator.currentTime -= animator.sequence->frameDuration;

            animator.currentFrame++;
            if (animator.currentFrame >= animator.sequence->rects.size())
            {
                animator.currentFrame = 0;
            }

            sprite.rect = animator.sequence->rects[animator.currentFrame];
        }
    }
}

void AnimationDirectionSwitchSystem(World& world, const FrameData& frameData)
{
    for (const auto& [entity, sprite, animator, controller] : world.Query<
             SpriteRenderer, Animator2D, CharacterController>())
    {
        if (glm::length(controller.velocity) < 0.001f)
        {
            sprite.texture = PlayerIdleTexture;
            animator.sequence = &PlayerIdleSequence;
        }
        else if (std::abs(controller.velocity.x) > std::abs(controller.velocity.y))
        {
            if (controller.velocity.x < 0.0f)
            {
                // Left
                sprite.texture = PlayerRunTexture;
                animator.sequence = &PlayerRunLeftSequence;
            }
            else
            {
                // Right
                sprite.texture = PlayerRunTexture;
                animator.sequence = &PlayerRunRightSequence;
            }
        }
        else
        {
            if (controller.velocity.y < 0.0f)
            {
                // Up
                sprite.texture = PlayerRunTexture;
                animator.sequence = &PlayerRunUpSequence;
            }
            else
            {
                // Down
                sprite.texture = PlayerRunTexture;
                animator.sequence = &PlayerRunDownSequence;
            }
        }
    }
}

void UpdateTransformHierarchy(World& world, Entity entity, const glm::mat4& parentTransform,
                              const UnorderedMap<Entity, List<Entity>>& children)
{
    auto transform = world.GetComponent<Transform>(entity);

    glm::mat4 worldMatrix = parentTransform * GetMatrix(*transform);
    transform->world = worldMatrix;

    auto it = children.find(entity);
    if (it == children.end())
        return;

    for (Entity child : it->second)
    {
        UpdateTransformHierarchy(world, child, worldMatrix, children);
    }
}

void TransformSystem(World& world, const FrameData& frameData)
{
    UnorderedMap<Entity, List<Entity>> children;
    for (const auto& [entity, relation] : world.Query<Relationship>())
    {
        children[relation.parent].push_back(entity);
    }

    for (const auto& [entity, relation, transform] : world.Query<Relationship, Transform>())
    {
        // We only want to process Root entities
        if (relation.parent != InvalidEntity)
            continue;

        UpdateTransformHierarchy(world, entity, glm::mat4(1.0f), children);
    }
}
