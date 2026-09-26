#include "pch.h"
#include "Sandbox.h"

#include "ECS/WorldManager.h"
#include "Engine/EngineComponents.h"
#include "Engine/FileSystem.h"
#include "Renderer/MeshPrimitiveFactory.h"

void RenderSystem(World& world, const FrameData& frameData);
void TransformSystem(World& world, const FrameData& frameData);

void Sandbox::Init()
{
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
    //
    // {
    //     auto rig = world.CreateEntity("CameraRig");
    //     world.AddComponent<Transform>(rig);
    //
    //     auto entity = world.CreateEntity("Camera");
    //     auto trans = world.AddComponent<Transform>(entity);
    //     trans->position = glm::vec3(0.f, 20.f, 20.f);
    //
    //     glm::vec3 direction = glm::normalize(glm::vec3(0.f) - trans->position);
    //     trans->rotation = glm::quatLookAt(direction, glm::vec3(0.f, 1.f, 0.f));
    //
    //     world.AddComponent<Camera>(entity);
    //
    //     auto relationship = world.AddComponent<Relationship>(entity);
    //     relationship->parent = rig;
    // }

    {
        mPlayer.Init(world);
    }

    {
        System system{};
        system.name = "Render System";
        system.stage = SS_PostTick;
        system.Tick = &RenderSystem;
        systemManager.RegisterSystem(std::move(system));
    }

    {
        System system{};
        system.name = "Transform System";
        system.stage = SS_PostTick;
        system.Tick = &TransformSystem;
        systemManager.RegisterSystem(std::move(system));
    }
}

void Sandbox::Tick(World& world, const FrameData& frameData)
{
    mPlayer.Tick(world, frameData);
    Services::Get<SystemManager>().TickSystems(world, frameData);
}

void RenderSystem(World& world, const FrameData& frameData)
{
    auto& renderer = Services::Get<Renderer>();

    RenderScene scene{};

    for (const auto& [entity, transform, mesh] : world.Query<Transform, MeshRenderer>())
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

void TransformSystem(World& world, const FrameData& frameData)
{
    UnorderedMap<Entity, List<Entity>> children;
    for (const auto& [entity, relation] : world.Query<Relationship>())
    {
        if (relation.parent != InvalidEntity)
        {
            children[relation.parent].push_back(entity);
        }
    }

    for (const auto& [entity, relation, transform] : world.Query<Relationship, Transform>())
    {
        // We only want to process Root entities
        if (relation.parent != InvalidEntity)
            continue;

        UpdateTransformHierarchy(world, entity, glm::mat4(1.0f), children);
    }
}
