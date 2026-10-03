#include "pch.h"
#include "Sandbox.h"

#include "AIComponents.h"
#include "Selectable.h"
#include "ECS/WorldManager.h"
#include "../../Engine/Source/Runtime/Game/EngineComponents.h"
#include "Engine/FileSystem.h"
#include "Renderer/MeshPrimitiveFactory.h"
#include "Core/Logging/Logs.h"
#include "Engine/Assets/AssetFile.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/TextureAsset.h"
#include "Engine/Assets/TextureAssetSerializer.h"
#include "Yaml/YamlArchive.h"

void RenderSystem(GameContext& gameContext);
void TransformSystem(GameContext& gameContext);
void MoveSystem(GameContext& gameContext);

void Sandbox::Init()
{
    {
        refl::ClassRegistrator<Selectable>("Selectable Tag", refl::TypeID("150c14a7-f199-4976-a491-955d11d50405"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<Movement>("Movement", refl::TypeID("150c14a2-f199-4976-a491-955d11d50405"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<MoveTarget>("Move Target", refl::TypeID("150c24a7-f199-4976-a491-955d11d50405"))
           .Category("Component")
           .Finish();
    }

    auto& world = Services::Get<WorldManager>().GetActiveWorld();
    auto& renderer = Services::Get<Renderer>();
    auto& fileSystem = Services::Get<FileSystem>();
    auto& systemManager = Services::Get<SystemManager>();

    MaterialHandle grassMaterial;
    {
        ShaderProgramCreateInfo shaderProgramCreateInfo{};
        shaderProgramCreateInfo.uniformLocations.push_back("uColor");
        shaderProgramCreateInfo.vertexShader = renderer.CompileShader(GL_VERTEX_SHADER, fileSystem.ReadAll("engine://Assets/Shaders/Sprite.vert").value());
        shaderProgramCreateInfo.fragmentShader = renderer.CompileShader(GL_FRAGMENT_SHADER, fileSystem.ReadAll("engine://Assets/Shaders/Sprite_Single_Color.frag").value());
        auto shaderProgram = renderer.CreateProgram(shaderProgramCreateInfo);

        MaterialCreateInfo materialCreateInfo{};
        materialCreateInfo.shaderProgram = shaderProgram;
        materialCreateInfo.parameterCount = 1;
        materialCreateInfo.parameters[0].name = "uColor";
        materialCreateInfo.parameters[0].value = glm::vec4(0.071f, 0.431f, 0.071f, 1.0f);
        grassMaterial = renderer.CreateMaterial(materialCreateInfo);
    }

    MaterialHandle unitMaterial;
    {
        ShaderProgramCreateInfo shaderProgramCreateInfo{};
        shaderProgramCreateInfo.uniformLocations.push_back("uColor");
        shaderProgramCreateInfo.vertexShader = renderer.CompileShader(GL_VERTEX_SHADER, fileSystem.ReadAll("engine://Assets/Shaders/Sprite.vert").value());
        shaderProgramCreateInfo.fragmentShader = renderer.CompileShader(GL_FRAGMENT_SHADER, fileSystem.ReadAll("engine://Assets/Shaders/Sprite_Single_Color.frag").value());
        auto shaderProgram = renderer.CreateProgram(shaderProgramCreateInfo);

        MaterialCreateInfo materialCreateInfo{};
        materialCreateInfo.shaderProgram = shaderProgram;
        materialCreateInfo.parameterCount = 1;
        materialCreateInfo.parameters[0].name = "uColor";
        materialCreateInfo.parameters[0].value = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        unitMaterial = renderer.CreateMaterial(materialCreateInfo);
    }

    {
        auto floor = world.CreateEntity("Floor");
        auto transform = world.AddComponent<Transform>(floor);
        transform->scale = glm::vec3(10.0f, 1.0f, 10.0f);

        auto meshRenderer = world.AddComponent<MeshRenderer>(floor);

        auto planeCreateInfo = MeshPrimitiveFactory::CreatePlane();
        auto planeMesh = renderer.CreateMesh(planeCreateInfo);
        meshRenderer->model.meshes.push_back(planeMesh);
        meshRenderer->model.materials.push_back(grassMaterial);
    }

    Prefab unitPrefab{};
    {
        auto meshCreateInfo = MeshPrimitiveFactory::CreateCube();
        auto mesh = renderer.CreateMesh(meshCreateInfo);

        PrefabBuilder builder{};
        auto root = builder.CreateRoot("Unit");
        builder.Add(root, Transform());
        builder.Add(root, Selectable());
        builder.Add(root, Movement());
        builder.Add(root, MoveTarget());

        MeshRenderer prefabRenderer{};
        prefabRenderer.model.meshes.push_back(mesh);
        prefabRenderer.model.materials.push_back(unitMaterial);
        builder.Add(root, prefabRenderer);

        unitPrefab = builder.Build();
    }

    {
        f32 minX = -5.0f;
        f32 minY = -5.0f;
        f32 maxX = 5.0f;
        f32 maxY = 5.0f;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution xDist(minX, maxX);
        std::uniform_real_distribution yDist(minY, maxY);

        for (int32 i = 0; i < 6; ++i)
        {
            auto entity = world.Instantiate(unitPrefab);
            auto* transform = world.GetComponent<Transform>(entity);
            transform->position.x = xDist(gen);
            transform->position.z = yDist(gen);
        }
    }

    // Asset lifecycle example. The image stays on disk when removing its asset.
    {
        auto runAssetExample = [&]() -> Expected<void, String>
        {
            AssetArchiveFactory archives = [](const ByteBuffer& bytes) -> OwnedPtr<Archive>
            {
                if (bytes.empty())
                    return MakeOwned<YamlArchive>();

                return MakeOwned<YamlArchive>(bytes);
            };

            AssetManager assets;
            auto serializer = assets.RegisterSerializer(String(TextureAsset::TYPE), MakeOwned<TextureAssetSerializer>(archives));
            if (!serializer)
                return serializer;

            // Create from an existing image and save Floor.asset alongside it.
            AssetInfo info{
                AssetID("6d5998b2-33b0-4a69-9288-13e4c8890ed2"),
                String(TextureAsset::TYPE),
                "game://Assets/Floor.asset"
            };
            TextureAsset draft;
            draft.SetSource("Floor.png");
            auto created = assets.Create(info, draft);
            if (!created)
                return Unexpected(created.error());

            auto saved = assets.Save(*created);
            if (!saved)
                return saved;

            // Remove from memory, then discover/register/load the saved descriptor.
            auto removed = assets.Remove(*created);
            if (!removed)
                return removed;
            assets.ReleaseRetired(); // No render snapshots use this example's handles.

            auto bytes = fileSystem.ReadAll(info.path);
            if (!bytes)
                return Unexpected(bytes.error().message);
            YamlArchive archive(*bytes);
            auto discovered = ReadAssetInfo(archive, info.path);
            if (!discovered)
                return Unexpected(discovered.error());
            auto registered = assets.Register(*discovered);
            if (!registered)
                return registered;

            auto loaded = assets.Load(AssetRef<TextureAsset>{discovered->id});
            if (!loaded)
                return Unexpected(loaded.error());

            // Edit a draft. Replacement creates a new immutable runtime texture.
            TextureAsset edited;
            edited.SetSource(assets.Get(*loaded)->GetSource());
            edited.SetSource("game://Assets/Floor.png");
            AssetHandle<> handle{loaded->index, loaded->generation};
            auto replaced = assets.Replace(handle, edited);
            if (!replaced)
                return replaced;
            saved = assets.Save(handle);
            if (!saved)
                return saved;

            removed = assets.Remove(handle);
            if (!removed)
                return removed;
            assets.ReleaseRetired();
            assets.Shutdown();
            LOG_INFO("Asset example completed: {}", info.path);
            return {};
        };

        auto result = runAssetExample();
        if (!result)
            LOG_WARNING("Asset example failed: {}", result.error());
    }

    {
        System system{};
        system.name = "Transform System";
        system.stage = SS_PostTick;
        system.Tick = &TransformSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Render System";
        system.stage = SS_PostTick;
        system.Tick = &RenderSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Move System";
        system.stage = SS_Tick;
        system.Tick = &MoveSystem;
        systemManager.RegisterSystem(system);
    }
}

void DrawSelectionBox(
    RenderScene& scene,
    const glm::vec3& start,
    const glm::vec3& end)
{
    auto& renderer = Services::Get<Renderer>();

    constexpr float y = 0.05f;

    glm::vec3 p0(start.x, y, start.z);
    glm::vec3 p1(end.x, y, start.z);
    glm::vec3 p2(end.x, y, end.z);
    glm::vec3 p3(start.x, y, end.z);

    const glm::vec4 color(
        0.1f,
        1.0f,
        0.1f,
        1.0f);

    renderer.DrawDebugLine(scene, p0, p1, color);
    renderer.DrawDebugLine(scene, p1, p2, color);
    renderer.DrawDebugLine(scene, p2, p3, color);
    renderer.DrawDebugLine(scene, p3, p0, color);
}

void RenderSystem(GameContext& gameContext)
{
    auto& renderer = Services::Get<Renderer>();

    RenderScene scene{};

    for (const auto& [entity, transform, mesh] : gameContext.world.Query<Transform, MeshRenderer>())
    {
        for (uint32 i = 0; i < mesh.model.meshes.size(); ++i)
        {
            RenderData data{};
            data.mesh = mesh.model.meshes[i];
            data.material = mesh.model.materials[i];
            data.transform = transform.world;
            scene.data.push_back(data);
        }
    }

    for (const auto& [entity, selection] : gameContext.world.Query<Selection>())
    {
        if (!selection.active)
            continue;

        DrawSelectionBox(scene, selection.start, selection.end);
    }

    for (const auto& [entity, transform, selectable] :
        gameContext.world.Query<Transform, Selectable>())
    {
        if (!selectable.selected)
            continue;

        renderer.DrawDebug2DBox(scene, {transform.position.x, transform.position.z}, {0.5f, 0.5f}, 0, {1, 0, 0, 1});
    }

    glm::mat4x4 projView{};
    for (const auto& [_, cam, trans] : gameContext.world.Query<Camera, Transform>())
    {
        glm::mat4 worldMatrix = trans.world;
        glm::mat4 view = glm::inverse(worldMatrix);
        glm::mat4 proj = glm::perspective(cam.fov, cam.aspect, cam.nearPlane, cam.farPlane);
        projView = proj * view;
        break;
    }

    auto& app = Services::Get<Application>();

    RenderView renderView{};
    renderView.viewport = {.x = 0, .y = 0, .width = app.GetInfo().viewportWidth, .height = app.GetInfo().viewportHeight};
    renderView.projectionView = projView;
    renderer.Render(scene, renderView);
}

void UpdateTransformHierarchy(World& world, const Entity entity, const glm::mat4& parentTransform,
                              const UnorderedMap<Entity, List<Entity>>& children)
{
    const auto transform = world.GetComponent<Transform>(entity);

    const glm::mat4 worldMatrix = parentTransform * GetMatrix(*transform);
    transform->world = worldMatrix;

    const auto it = children.find(entity);
    if (it == children.end())
        return;

    for (const Entity child : it->second)
    {
        UpdateTransformHierarchy(world, child, worldMatrix, children);
    }
}

void TransformSystem(GameContext& gameContext)
{
    UnorderedMap<Entity, List<Entity>> children;
    for (const auto& [entity, relation] : gameContext.world.Query<Relationship>())
    {
        if (relation.parent != InvalidEntity)
        {
            children[relation.parent].push_back(entity);
        }
    }

    for (const auto& [entity, relation, transform] : gameContext.world.Query<Relationship, Transform>())
    {
        // We only want to process Root entities
        if (relation.parent != InvalidEntity)
            continue;

        UpdateTransformHierarchy(gameContext.world, entity, glm::mat4(1.0f), children);
    }
}

void MoveSystem(GameContext& gameContext)
{
    for (const auto& [entity, transform, move, target] :
         gameContext.world.Query<Transform, Movement, MoveTarget>())
    {
        if (!target.active)
            continue;

        glm::vec3 toTarget = target.position - transform.position;

        toTarget.y = 0.0f;

        const f32 distance = glm::length(toTarget);

        if (distance <= target.stoppingDistance)
        {
            transform.position.x = target.position.x;
            transform.position.z = target.position.z;

            move.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            target.active = false;
            continue;
        }

        const glm::vec3 direction = toTarget / distance;
        move.velocity = direction * move.maxSpeed;
        transform.position += move.velocity * gameContext.deltaTime;
    }
}
