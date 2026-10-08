#include "pch.h"
#include "Sandbox.h"

#include "AIComponents.h"
#include "Selectable.h"
#include "ECS/WorldManager.h"
#include "../../Engine/Source/Runtime/Game/EngineComponents.h"
#include "Engine/FileSystem.h"
#include "Engine/MeshImporter.h"
#include "Engine/TextureLoader.h"
#include "Game/AnimationSystem.h"
#include "Renderer/MeshPrimitiveFactory.h"
#include "Core/Logging/Logs.h"
#include "Yaml/YamlArchive.h"
#include "UI/UI.h"

void RenderSystem(GameContext& gameContext);
void TransformSystem(GameContext& gameContext);
void MoveSystem(GameContext& gameContext);
void EnemyTargetSystem(GameContext& gameContext);
void AttackSystem(GameContext& gameContext);
void DeathSystem(GameContext& gameContext);
void UnitAnimationSystem(GameContext& gameContext);
void HealthBarSystem(GameContext& gameContext);

void Sandbox::Init()
{
    {
        refl::ClassRegistrator<Selectable>("Selectable Tag", refl::TypeID("150c14a7-f199-4976-a491-955d11d50405"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<Team>("Team", refl::TypeID("563c94cf-ddca-4d2e-8141-2f460a177f32"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<Health>("Health", refl::TypeID("a2f1c443-e6c2-4ab5-b23d-1cd03d65b6a5"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<Attack>("Attack", refl::TypeID("aec4a3f0-8c7a-436b-a71b-159bd6090446"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<Movement>("Movement", refl::TypeID("150c14a2-f199-4976-a491-955d11d50405"))
                .Category("Component")
                .Finish();

        refl::ClassRegistrator<MoveTarget>("Move Target", refl::TypeID("150c24a7-f199-4976-a491-955d11d50405"))
           .Category("Component")
           .Finish();

        refl::ClassRegistrator<UnitAnimations>("Unit Animations", refl::TypeID("1d5da945-0a26-4341-a8ba-307e08093daa"))
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

    {
        auto floor = world.CreateEntity("Floor");
        auto transform = world.AddComponent<Transform>(floor);
        transform->scale = glm::vec3(20.0f, 1.0f, 20.0f);

        auto meshRenderer = world.AddComponent<MeshRenderer>(floor);

        auto planeCreateInfo = MeshPrimitiveFactory::CreatePlane();
        auto planeMesh = renderer.CreateMesh(planeCreateInfo);
        meshRenderer->model.meshes.push_back(planeMesh);
        meshRenderer->model.materials.push_back(grassMaterial);
    }

    MaterialHandle shamanMaterial;
    MaterialHandle enemyShamanMaterial;
    {
        auto file = fileSystem.ReadAll("game://Assets/Shaman/CH_PL_Shaman_19G1M_SK.fbx");
        if (!file)
            FATAL("Can't read Shaman mesh: {}", file.error().message);

        auto imported = MeshImporter::Import(*file, "fbx");
        if (!imported)
            FATAL("Can't import Shaman mesh: {}", imported.error());
        if (!imported->skeleton || imported->meshes.empty())
            FATAL("Shaman needs a mesh and skeleton");

        // Mesh 0 is the body; the other meshes are alternate staffs.
        auto& body = imported->meshes.front();
        if (body.skinJoints.empty() || body.skinJoints.size() > MaxSkinJoints)
            FATAL("Shaman needs between 1 and {} skin joints", MaxSkinJoints);
        mShamanSkeleton = std::move(*imported->skeleton);

        auto idleFile = fileSystem.ReadAll("game://Assets/Shaman/Animations/CH_PL_Shaman@idle_01_19G1M_AN.fbx");
        if (!idleFile)
            FATAL("Can't read Shaman idle animation: {}", idleFile.error().message);
        auto idle = MeshImporter::Import(*idleFile, "fbx", &mShamanSkeleton);
        if (!idle)
            FATAL("Can't import Shaman idle animation: {}", idle.error());
        if (idle->animations.empty())
            FATAL("Shaman idle file contains no animation");
        mShamanIdle = std::move(idle->animations.front());

        auto runFile = fileSystem.ReadAll("game://Assets/Shaman/Animations/CH_PL_Shaman@run_01_19G1M_AN.fbx");
        if (!runFile)
            FATAL("Can't read Shaman run animation: {}", runFile.error().message);
        auto run = MeshImporter::Import(*runFile, "fbx", &mShamanSkeleton);
        if (!run)
            FATAL("Can't import Shaman run animation: {}", run.error());
        if (run->animations.empty())
            FATAL("Shaman run file contains no animation");
        mShamanRun = std::move(run->animations.front());

        auto attackFile = fileSystem.ReadAll("game://Assets/Shaman/Animations/CH_PL_Shaman@attack_01_19G1M_AN.fbx");
        if (!attackFile)
            FATAL("Can't read Shaman attack animation: {}", attackFile.error().message);
        auto attack = MeshImporter::Import(*attackFile, "fbx", &mShamanSkeleton);
        if (!attack)
            FATAL("Can't import Shaman attack animation: {}", attack.error());
        if (attack->animations.empty())
            FATAL("Shaman attack file contains no animation");
        mShamanAttack = std::move(attack->animations.front());

        auto colourFile = fileSystem.ReadAll("game://Assets/Shaman/CH_PL_Shaman_19G1M_SK_C.png");
        if (!colourFile)
            FATAL("Can't read Shaman colour texture: {}", colourFile.error().message);
        auto colour = TextureLoader::LoadTexture(*colourFile);
        if (colour.data.empty())
            FATAL("Can't decode Shaman colour texture");

        auto vertexSource = fileSystem.ReadAll("engine://Assets/Shaders/SkinnedMesh.vert");
        if (!vertexSource)
            FATAL("Can't read Shaman vertex shader: {}", vertexSource.error().message);
        auto fragmentSource = fileSystem.ReadAll("engine://Assets/Shaders/Mesh.frag");
        if (!fragmentSource)
            FATAL("Can't read Shaman fragment shader: {}", fragmentSource.error().message);

        ShaderSourceCreateInfo shaderInfo{};
        shaderInfo.vertexSource = std::move(*vertexSource);
        shaderInfo.fragmentSource = std::move(*fragmentSource);
        shaderInfo.uniformLocations = {"uLightDirection", "uAmbientStrength", "uTint"};
        auto shaderProgram = renderer.CreateProgram(shaderInfo);
        if (!shaderProgram)
            FATAL("Can't create Shaman shader program: {}", shaderProgram.error());

        // Keep only runtime resources after this block; the import containers are discarded.
        mShamanMesh.joints = std::move(body.skinJoints);
        mShamanMesh.nodeTransform = body.nodeTransform;
        MeshCreateInfo meshCreateInfo{};
        meshCreateInfo.vertices = std::move(body.vertices);
        meshCreateInfo.indices = std::move(body.indices);
        meshCreateInfo.skinVertices = std::move(body.skinVertices);
        mShamanMesh.mesh = renderer.CreateMesh(meshCreateInfo);

        TextureCreateInfo textureInfo{};
        textureInfo.data = std::move(colour.data);
        textureInfo.width = colour.width;
        textureInfo.height = colour.height;
        textureInfo.channels = colour.channels;

        MaterialCreateInfo materialInfo{};
        materialInfo.shaderProgram = *shaderProgram;
        materialInfo.textures[0] = renderer.CreateTexture(textureInfo);
        materialInfo.textureCount = 1;
        materialInfo.parameterCount = 3;
        materialInfo.parameters[0].name = "uLightDirection";
        materialInfo.parameters[0].value = glm::normalize(glm::vec3(0.5f, 1.0f, 0.7f));
        materialInfo.parameters[1].name = "uAmbientStrength";
        materialInfo.parameters[1].value = 0.35f;
        materialInfo.parameters[2].name = "uTint";
        materialInfo.parameters[2].value = glm::vec3(1.0f);
        shamanMaterial = renderer.CreateMaterial(materialInfo);

        // A second immutable material shares the shader and texture, with a red tint.
        materialInfo.parameters[2].value = glm::vec3(1.0f, 0.3f, 0.3f);
        enemyShamanMaterial = renderer.CreateMaterial(materialInfo);
    }

    Prefab unitPrefab{};
    {
        PrefabBuilder builder{};
        auto root = builder.CreateRoot("Shaman");
        builder.Add(root, Transform());
        builder.Add(root, Selectable());
        builder.Add(root, Team());
        builder.Add(root, Health());
        // Leave a short pause after each complete attack animation.
        builder.Add(root, Attack{.interval = mShamanAttack.duration + 0.25f});
        builder.Add(root, Movement());
        builder.Add(root, MoveTarget());
        builder.Add(root, UnitAnimations{&mShamanIdle, &mShamanRun, &mShamanAttack});

        Animator animator{};
        animator.skeleton = &mShamanSkeleton;
        animator.clip = &mShamanIdle;
        builder.Add(root, animator);

        AnimatedMeshRenderer prefabRenderer{};
        prefabRenderer.mesh = &mShamanMesh;
        prefabRenderer.material = shamanMaterial;
        builder.Add(root, prefabRenderer);

        unitPrefab = builder.Build();
    }

    // Keep the example at the center, using the same selectable unit prefab.
    world.Instantiate(unitPrefab);

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
            const auto* movement = world.GetComponent<Movement>(entity);
            glm::vec3 position;
            bool occupied;
            // Start with separate circles; the movement sweep then keeps them separate.
            do
            {
                position = {xDist(gen), 0.0f, yDist(gen)};
                occupied = false;
                for (const auto& [other, otherTransform, otherMovement] : world.Query<Transform, Movement>())
                {
                    const glm::vec3 offset = position - otherTransform.position;
                    const f32 radius = movement->radius + otherMovement.radius;
                    if (other != entity && glm::dot(offset, offset) < radius * radius)
                    {
                        occupied = true;
                        break;
                    }
                }
            } while (occupied);
            transform->position = position;
        }
    }

    // Enemies use the same prefab, and acquire player targets through EnemyTargetSystem.
    for (uint32 i = 0; i < 3; ++i)
    {
        const Entity enemy = world.Instantiate(unitPrefab);
        world.GetComponent<NameComponent>(enemy)->name = "Enemy Shaman";
        world.GetComponent<Transform>(enemy)->position = {2.0f * static_cast<f32>(i) - 2.0f, 0.0f, -7.0f};
        world.GetComponent<Team>(enemy)->id = Team::Enemy;
        world.GetComponent<AnimatedMeshRenderer>(enemy)->material = enemyShamanMaterial;
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
        system.name = "Unit Animation System";
        system.stage = SS_PostTick;
        system.Tick = &UnitAnimationSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Animation System";
        system.stage = SS_PostTick;
        system.Tick = &AnimationSystem;
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
        system.name = "Health Bar System";
        system.stage = SS_PostTick;
        system.Tick = &HealthBarSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Enemy Target System";
        system.stage = SS_Tick;
        system.Tick = &EnemyTargetSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Attack System";
        system.stage = SS_Tick;
        system.Tick = &AttackSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Move System";
        system.stage = SS_Tick;
        system.Tick = &MoveSystem;
        systemManager.RegisterSystem(system);
    }

    {
        System system{};
        system.name = "Death System";
        system.stage = SS_Tick;
        system.Tick = &DeathSystem;
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

    for (const auto& [entity, transform, meshRenderer, animator] :
         gameContext.world.Query<Transform, AnimatedMeshRenderer, Animator>())
    {
        const auto& mesh = *meshRenderer.mesh;
        const glm::mat4 modelToMesh = glm::inverse(mesh.nodeTransform);
        meshRenderer.jointMatrices.resize(mesh.joints.size());
        for (uint32 i = 0; i < mesh.joints.size(); ++i)
        {
            const auto& joint = mesh.joints[i];
            // Mesh-local -> bone bind space -> animated model space -> mesh-local.
            meshRenderer.jointMatrices[i] = modelToMesh * animator.pose[joint.skeletonNode] * joint.inverseBind;
        }

        RenderData data{};
        data.mesh = mesh.mesh;
        data.material = meshRenderer.material;
        data.transform = transform.world * mesh.nodeTransform;
        data.jointMatrices = meshRenderer.jointMatrices;
        scene.data.push_back(data);
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

    auto& app = Services::Get<Application>();
    glm::mat4x4 projView{};
    for (const auto& [_, cam, trans] : gameContext.world.Query<Camera, Transform>())
    {
        glm::mat4 worldMatrix = trans.world;
        glm::mat4 view = glm::inverse(worldMatrix);
        cam.aspect = static_cast<f32>(app.GetInfo().viewportWidth) / std::max(1, app.GetInfo().viewportHeight);
        glm::mat4 proj = glm::perspective(cam.fov, cam.aspect, cam.nearPlane, cam.farPlane);
        projView = proj * view;
        break;
    }

    auto& renderView = gameContext.renderView;
    renderView.viewport = {.x = 0, .y = 0, .width = app.GetInfo().viewportWidth, .height = app.GetInfo().viewportHeight};
    renderView.projectionView = projView;
    renderer.Render(scene, renderView);
}

void HealthBarSystem(GameContext& gameContext)
{
    const auto& renderView = gameContext.renderView;
    const auto& projView = renderView.projectionView;
    auto& ui = Services::Get<UI>();
    const glm::vec2 size(renderView.viewport.width, renderView.viewport.height);
    for (const auto& [entity, transform, team, health] : gameContext.world.Query<Transform, Team, Health>())
    {
        if (health.maximum <= 0.0f)
            continue;
        const glm::vec4 clip = projView * glm::vec4(transform.position + glm::vec3(0.0f, 2.2f, 0.0f), 1.0f);
        if (clip.w <= 0.0f)
            continue;
        const glm::vec3 ndc = glm::vec3(clip) / clip.w;
        if (glm::any(glm::greaterThan(glm::abs(ndc), glm::vec3(1.0f))))
            continue;

        const glm::vec2 position((ndc.x * 0.5f + 0.5f) * size.x - 25.0f,
                                 (0.5f - ndc.y * 0.5f) * size.y);
        const f32 fill = glm::clamp(health.current / health.maximum, 0.0f, 1.0f);
        const glm::vec4 colour = team.id == Team::Player ? glm::vec4(0.1f, 0.9f, 0.1f, 1.0f)
                                                       : glm::vec4(0.9f, 0.1f, 0.1f, 1.0f);
        ui.Rectangle({position.x, position.y, 50.0f, 6.0f}, {0.05f, 0.05f, 0.05f, 1.0f});
        ui.Rectangle({position.x + 1.0f, position.y + 1.0f, 48.0f * fill, 4.0f}, colour);
    }
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

namespace
{
    // Return the first circle hit along this entire step, preventing tunnelling on slow frames.
    f32 SweepUnits(World& world, Entity entity, glm::vec2 position, glm::vec2 step,
                   f32 radius, glm::vec2& normal)
    {
        const f32 lengthSquared = glm::dot(step, step);
        f32 fraction = 1.0f;
        for (const auto& [other, transform, movement] : world.Query<Transform, Movement>())
        {
            if (other == entity)
                continue;

            const glm::vec2 offset = position - glm::vec2(transform.position.x, transform.position.z);
            const f32 combinedRadius = radius + movement.radius;
            const f32 b = glm::dot(offset, step);
            if (b >= 0.0f)
                continue; // Moving away from this circle.

            const f32 c = glm::dot(offset, offset) - combinedRadius * combinedRadius;
            const f32 discriminant = b * b - lengthSquared * c;
            if (discriminant <= 0.0f)
                continue;

            const f32 hit = std::max(0.0f, (-b - std::sqrt(discriminant)) / lengthSquared);
            if (hit < fraction)
            {
                normal = glm::normalize(offset + step * hit);
                // Leave a millimetre of clearance to avoid repeated floating-point contact.
                fraction = std::max(0.0f, hit - 0.001f / std::sqrt(lengthSquared));
            }
        }
        return fraction;
    }
}

void MoveSystem(GameContext& gameContext)
{
    for (const auto& [entity, transform, move, target] :
         gameContext.world.Query<Transform, Movement, MoveTarget>())
    {
        move.velocity = glm::vec3(0.0f);
        if (!target.active || gameContext.deltaTime <= 0.0f)
            continue;

        glm::vec3 toTarget = target.position - transform.position;

        toTarget.y = 0.0f;

        const f32 distance = glm::length(toTarget);

        if (distance <= target.stoppingDistance)
        {
            target.active = false;
            continue;
        }

        // Stop unreachable orders instead of circling an occupied destination forever.
        if (distance < target.closestDistance - 0.01f)
        {
            target.closestDistance = distance;
            target.timeWithoutProgress = 0.0f;
        }
        else
        {
            target.timeWithoutProgress += gameContext.deltaTime;
            if (target.timeWithoutProgress >= 1.0f)
            {
                target.active = false;
                continue;
            }
        }

        const glm::vec2 start(transform.position.x, transform.position.z);
        glm::vec2 position = start;
        const glm::vec2 direction = glm::vec2(toTarget.x, toTarget.z) / distance;
        const glm::vec2 right(direction.y, -direction.x);
        glm::vec2 steering = direction;
        // Begin passing on our right before contact, avoiding opposing movers choosing the same gap.
        for (const auto& [other, otherTransform, otherMovement] : gameContext.world.Query<Transform, Movement>())
        {
            if (other == entity)
                continue;

            const glm::vec2 offset = glm::vec2(otherTransform.position.x, otherTransform.position.z) - start;
            const f32 radius = move.radius + otherMovement.radius;
            const f32 ahead = glm::dot(offset, direction);
            if (ahead > 0.0f && ahead < 2.0f * radius && std::abs(glm::dot(offset, right)) < radius + 0.1f)
                steering += right * (2.0f - ahead / radius);
        }
        glm::vec2 remaining = glm::normalize(steering) * std::min(distance, move.maxSpeed * gameContext.deltaTime);

        // Move to contact, then slide the rest of the step along the neighbouring circle.
        for (uint32 contact = 0; contact < 3 && glm::dot(remaining, remaining) > 0.00000001f; ++contact)
        {
            glm::vec2 normal{};
            const f32 fraction = SweepUnits(gameContext.world, entity, position, remaining, move.radius, normal);
            position += remaining * fraction;
            if (fraction == 1.0f)
                break;

            remaining *= 1.0f - fraction;
            const f32 remainingLength = glm::length(remaining);
            remaining -= normal * std::min(glm::dot(remaining, normal), 0.0f);
            // Pick our right side at head-on contact so both movers can pass each other.
            if (glm::dot(remaining, remaining) < 0.00000001f)
                remaining = glm::vec2(-normal.y, normal.x) * remainingLength;
        }

        transform.position.x = position.x;
        transform.position.z = position.y;
        const glm::vec2 travelled = position - start;
        if (glm::dot(travelled, travelled) > 0.00000001f)
        {
            move.velocity = glm::vec3(travelled.x, 0.0f, travelled.y) / gameContext.deltaTime;
            // The Shaman's local forward axis is +Z. Retain the last heading when stopped.
            transform.rotation = glm::angleAxis(std::atan2(travelled.x, travelled.y), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        const glm::vec2 toDestination(target.position.x - position.x, target.position.z - position.y);
        if (glm::length(toDestination) <= target.stoppingDistance)
        {
            target.active = false;
            move.velocity = glm::vec3(0.0f);
        }
    }
}

void UnitAnimationSystem(GameContext& gameContext)
{
    for (const auto& [entity, movement, animations, animator] :
         gameContext.world.Query<Movement, UnitAnimations, Animator>())
    {
        // Let a stationary attack finish; moving orders interrupt it.
        if (!animator.loop && animator.clip && animator.currentTime < animator.clip->duration &&
            glm::dot(movement.velocity, movement.velocity) == 0.0f)
            continue;

        const AnimationClip* clip = glm::dot(movement.velocity, movement.velocity) > 0.0f
            ? animations.run : animations.idle;
        animator.loop = true;
        if (animator.clip != clip)
        {
            animator.clip = clip;
            animator.currentTime = 0.0f;
        }
    }
}

void EnemyTargetSystem(GameContext& gameContext)
{
    constexpr f32 detectionRange = 6.0f;
    auto& world = gameContext.world;
    for (const auto& [entity, transform, team, health, attack, moveTarget] :
         world.Query<Transform, Team, Health, Attack, MoveTarget>())
    {
        if (team.id != Team::Enemy || health.current <= 0.0f)
            continue;

        if (attack.target != InvalidEntity)
        {
            const auto* targetHealth = world.GetComponent<Health>(attack.target);
            const auto* targetTeam = world.GetComponent<Team>(attack.target);
            const auto* targetTransform = world.GetComponent<Transform>(attack.target);
            // Keep a living target instead of switching whenever another unit gets closer.
            if (targetHealth && targetHealth->current > 0.0f && targetTeam && targetTeam->id == Team::Player && targetTransform)
                continue;

            attack.target = InvalidEntity;
            moveTarget.active = false;
        }

        f32 closestDistanceSquared = detectionRange * detectionRange;
        for (const auto& [candidate, candidateTransform, candidateTeam, candidateHealth] :
             world.Query<Transform, Team, Health>())
        {
            if (candidateTeam.id != Team::Player || candidateHealth.current <= 0.0f)
                continue;

            glm::vec3 offset = candidateTransform.position - transform.position;
            offset.y = 0.0f;
            const f32 distanceSquared = glm::dot(offset, offset);
            if (distanceSquared <= closestDistanceSquared)
            {
                closestDistanceSquared = distanceSquared;
                attack.target = candidate;
            }
        }

        if (attack.target != InvalidEntity)
        {
            // A new target starts a fresh chase; existing targets keep their movement progress.
            moveTarget.closestDistance = std::numeric_limits<f32>::max();
            moveTarget.timeWithoutProgress = 0.0f;
        }
    }
}

void AttackSystem(GameContext& gameContext)
{
    if (gameContext.deltaTime <= 0.0f)
        return;

    auto& world = gameContext.world;
    for (const auto& [entity, transform, team, health, attack, moveTarget, animations, animator] :
         world.Query<Transform, Team, Health, Attack, MoveTarget, UnitAnimations, Animator>())
    {
        attack.cooldown = std::max(0.0f, attack.cooldown - gameContext.deltaTime);
        if (health.current <= 0.0f)
        {
            attack.target = InvalidEntity;
            moveTarget.active = false;
            continue;
        }
        if (attack.target == InvalidEntity)
            continue; // Leave ordinary movement orders alone.

        auto* enemyTransform = world.GetComponent<Transform>(attack.target);
        auto* enemyTeam = world.GetComponent<Team>(attack.target);
        auto* enemyHealth = world.GetComponent<Health>(attack.target);
        if (!enemyTransform || !enemyTeam || !enemyHealth || enemyHealth->current <= 0.0f || enemyTeam->id == team.id)
        {
            attack.target = InvalidEntity;
            moveTarget.active = false;
            continue;
        }

        glm::vec3 toEnemy = enemyTransform->position - transform.position;
        toEnemy.y = 0.0f;
        const f32 distance = glm::length(toEnemy);
        if (distance > attack.range)
        {
            // Follow a moving target without resetting the blocked-movement timer every frame.
            if (moveTarget.position != enemyTransform->position)
            {
                moveTarget.closestDistance = std::numeric_limits<f32>::max();
                moveTarget.timeWithoutProgress = 0.0f;
            }
            moveTarget.position = enemyTransform->position;
            moveTarget.active = true;
            continue;
        }

        moveTarget.active = false;
        if (distance > 0.000001f)
            transform.rotation = glm::angleAxis(std::atan2(toEnemy.x, toEnemy.z), glm::vec3(0.0f, 1.0f, 0.0f));
        if (attack.cooldown > 0.0f)
            continue;

        // Damage happens at the start of the swing; no animation events are needed yet.
        enemyHealth->current = std::max(0.0f, enemyHealth->current - attack.damage);
        attack.cooldown = attack.interval;
        animator.clip = animations.attack;
        animator.currentTime = 0.0f;
        animator.loop = false;
    }
}

void DeathSystem(GameContext& gameContext)
{
    List<Entity> dead;
    for (const auto& [entity, health] : gameContext.world.Query<Health>())
    {
        if (health.current <= 0.0f)
            dead.push_back(entity);
    }

    // Finish querying before removing components.
    for (const Entity entity : dead)
        gameContext.world.DestroyEntity(entity);
}
