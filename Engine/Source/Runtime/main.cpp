/*#include "pch.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Application.h"
#include "AssetRegistry.h"
#include "CollisionSystem.h"
#include "InputSystem.h"
#include "PlayerMovement.h"
#include "Renderer.h"
#include "game/AnimationSystem.h"
#include "game/AttackInputSystem.h"
#include "game/AttackSystem.h"
#include "game/HitDetectionSystem.h"
#include "game/InvincibilitySystem.h"
#include "game/KillSystem.h"
#include "game/PlayerAnimationSystem.h"
#include "game/RenderSystem.h"

Application GApp;
Renderer GRenderer;
AssetRegistry GAssetRegistry;

constexpr float WindowSizeX = 1280;
constexpr float WindowSizeY = 720;

constexpr int tileSize = 64;

const auto vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec2 aPosition;
        layout (location = 1) in vec2 aTexCoord;

        uniform vec2 uPosition;
        uniform vec2 uWindowSize;
        uniform vec4 uUvRect; // x, y, width, height

        out vec2 vTexCoord;

        void main()
        {
            vec2 pixelPosition = aPosition + uPosition;

            vec2 clipPosition;
            clipPosition.x = (pixelPosition.x / uWindowSize.x) * 2.0 - 1.0;
            clipPosition.y = 1.0 - (pixelPosition.y / uWindowSize.y) * 2.0;

            vTexCoord = uUvRect.xy + aTexCoord * uUvRect.zw;
            gl_Position = vec4(clipPosition, 0.0, 1.0);
        }
    )";

const auto fragmentSource = R"(
        #version 460 core

        in vec2 vTexCoord;
        out vec4 FragColor;

        uniform sampler2D uTexture;

        void main()
        {
            FragColor = texture(uTexture, vTexCoord);
        }
    )";

int main(int argc, char* argv[])
{
    std::cout << "Base path: " << SDL_GetBasePath() << std::endl;

    (void)argc;
    (void)argv;

    ApplicationWindowCreateInfo windowCreateInfo{};
    windowCreateInfo.useOpenGL = true;
    windowCreateInfo.major = 4;
    windowCreateInfo.minor = 6;
    windowCreateInfo.profile = SDL_GL_CONTEXT_PROFILE_CORE;
    windowCreateInfo.doubleBuffer = true;
    windowCreateInfo.depthBits = 24;
    windowCreateInfo.stencilBits = 8;
    windowCreateInfo.flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (!GApp.Init(SDL_INIT_VIDEO, windowCreateInfo))
    {
        std::cerr << "App Init failed!" << std::endl;
        return 1;
    }

    GRenderer.Init(GApp);

    const GLuint vertexShader = GRenderer.CompileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = GRenderer.CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    ShaderProgramCreateInfo programCreateInfo{};
    programCreateInfo.vertexShader = vertexShader;
    programCreateInfo.fragmentShader = fragmentShader;
    const GLuint shaderProgram = GRenderer.CreateProgram(programCreateInfo);

    constexpr float halfTileSize = tileSize * 0.5f;

    float vertices[] =
    {
        // position                    // uv
        -halfTileSize, -halfTileSize, 0.0f, 0.0f,
        halfTileSize, -halfTileSize, 1.0f, 0.0f,
        halfTileSize, halfTileSize, 1.0f, 1.0f,
        -halfTileSize, halfTileSize, 0.0f, 1.0f,
    };

    uint32_t indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    MeshCreateInfo meshCreateInfo{};
    meshCreateInfo.vertices = vertices;
    meshCreateInfo.indices = indices;
    meshCreateInfo.vertexCount = sizeof(vertices) / sizeof(float);
    meshCreateInfo.indexCount = sizeof(indices) / sizeof(uint32_t);
    Mesh quadMesh = GRenderer.CreateMesh(meshCreateInfo);

    Texture playerWalkTexture = GAssetRegistry.GetTexture("Default/walk.png");
    if (playerWalkTexture.id == 0)
    {
        return 1;
    }

    Texture playerIdleTexture = GAssetRegistry.GetTexture("Default/idle.png");
    if (playerIdleTexture.id == 0)
    {
        return 1;
    }

    Texture floorTexture = GAssetRegistry.GetTexture("Floor.png");
    if (floorTexture.id == 0)
    {
        return 1;
    }

    Texture wallTexture = GAssetRegistry.GetTexture("Wall.png");
    if (wallTexture.id == 0)
    {
        return 1;
    }

    Texture killerRobo1Texture = GAssetRegistry.GetTexture("killerRobo1.png");
    if (killerRobo1Texture.id == 0)
    {
        return 1;
    }

    TileMap tileMap{};
    tileMap.shader = shaderProgram;
    tileMap.mesh = quadMesh;
    tileMap.textures.insert({"wall", wallTexture});
    tileMap.textures.insert({"floor", floorTexture});

    World world{};
    world.width = 15;
    world.height = 11;
    world.tiles = {
        "XXXXXXXXXXXXXXX",
        "XOOOOOOOOOOOOOX",
        "XOOOOOOOOOOOOOX",
        "XOOXXXOOOXXXOOX",
        "XOOOOOOOOOOOOOX",
        "XOOOOOOOOOOOOOX",
        "XOOOOOOOOOOOOOX",
        "XOOXXXOXOXXXOOX",
        "XOOOOOOOOOOOOOX",
        "XOOOOOOOOOOOOOX",
        "XXXXXXXXXXXXXXX"
    };

    RenderView renderView{};
    renderView.viewport.x = 0;
    renderView.viewport.y = 0;
    renderView.viewport.width = WindowSizeX;
    renderView.viewport.height = WindowSizeY;
    renderView.camera.pixelsPerUnit = tileSize;
    renderView.camera.offsetX = (WindowSizeX - world.width * renderView.camera.pixelsPerUnit) * 0.5f;
    renderView.camera.offsetY = (WindowSizeY - world.height * renderView.camera.pixelsPerUnit) * 0.5f;

    //Player
    {
        Transform2D playerTransform{};
        playerTransform.x = 7.5f;
        playerTransform.y = 5.5f;

        MovementData movementData{};
        movementData.speed = 3.0f;

        Collider2D collider{};
        collider.halfWidth = 0.64f / 2;
        collider.halfHeight = 0.64f / 2;

        Sprite playerSprite{};
        playerSprite.texture = playerIdleTexture;
        playerSprite.mesh = quadMesh;
        playerSprite.shader = shaderProgram;

        AnimationClip animWalkDown{};
        animWalkDown.firstFrame = 0;
        animWalkDown.totalFrames = 4;
        animWalkDown.columns = 4;
        animWalkDown.rows = 3;
        animWalkDown.frameTime = 1.0f / 8.f;
        animWalkDown.texture = playerWalkTexture;

        AnimationClip animWalkRight{};
        animWalkRight.firstFrame = 4;
        animWalkRight.totalFrames = 4;
        animWalkRight.columns = 4;
        animWalkRight.rows = 3;
        animWalkRight.frameTime = 1.0f / 8.f;
        animWalkRight.texture = playerWalkTexture;

        AnimationClip animWalkUp{};
        animWalkUp.firstFrame = 8;
        animWalkUp.totalFrames = 4;
        animWalkUp.columns = 4;
        animWalkUp.rows = 3;
        animWalkUp.frameTime = 1.0f / 8.f;
        animWalkUp.texture = playerWalkTexture;

        AnimationClip animIdle{};
        animIdle.firstFrame = 0;
        animIdle.totalFrames = 2;
        animIdle.columns = 2;
        animIdle.rows = 3;
        animIdle.frameTime = 1.0f / 8.f;
        animIdle.texture = playerIdleTexture;

        AnimationPlayer animPlayer{};
        animPlayer.clips["down"] = animWalkDown;
        animPlayer.clips["right"] = animWalkRight;
        animPlayer.clips["up"] = animWalkUp;
        animPlayer.clips["idle"] = animIdle;
        animPlayer.currentClip = "idle";

        Attack attack{};
        attack.damage = 10;
        attack.duration = 0.5f;

        Hitbox hitbox{};
        hitbox.offsetX = 1.0f;
        hitbox.offsetY = 0.0f;
        hitbox.halfWidth = 0.4f;
        hitbox.halfHeight = 0.3f;

        Entity playerEntity = world.CreateEntity();
        world.transforms.insert({playerEntity, playerTransform});
        world.movements.insert({playerEntity, movementData});
        world.colliders.insert({playerEntity, collider});
        world.sprites.insert({playerEntity, playerSprite});
        world.animations.insert({playerEntity, animPlayer});
        world.playerTag.insert({playerEntity, PlayerTag{}});
        world.attacks.insert({playerEntity, attack});
        world.hitboxes.insert({playerEntity, hitbox});

        world.AddEntity(playerEntity);
    }

    // enemy
    {
        Entity enemy = world.CreateEntity();

        Transform2D transform{};
        transform.x = 9;
        transform.y = 5;

        Sprite sprite{};
        sprite.texture = killerRobo1Texture;
        sprite.mesh = quadMesh;
        sprite.shader = shaderProgram;

        Health health{};
        health.current = 50;
        health.max = 50;

        Hurtbox hurtbox{};
        hurtbox.halfWidth = 0.3f;
        hurtbox.halfHeight = 0.3f;

        Invincibility invincibility{};

        world.transforms[enemy] = transform;
        world.sprites[enemy] = sprite;
        world.healths[enemy] = health;
        world.hurtboxes[enemy] = hurtbox;
        world.invincibility[enemy] = invincibility;

        world.AddEntity(enemy);
    }

    uint64_t previousTime = SDL_GetTicks();
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                {
                    running = false;
                    break;
                }
            default: ;
            }
        }

        uint64_t currentTime = SDL_GetTicks();
        float deltaTime = static_cast<float>(currentTime - previousTime) / 1000.f;
        previousTime = currentTime;

        FrameData frameData{};
        frameData.deltaTime = deltaTime;

        InputSystem(world);
        AttackInputSystem(world);
        PlayerDefaultMovement(world, frameData);
        PlayerCollisionSystem(world);

        InvincibilitySystem(world, frameData);
        HitDetectionSystem(world);
        AttackSystem(world, frameData);

        KillSystem(world);

        PlayerAnimationSystem(world);

        AnimationSystem(world, frameData);

        RenderSystem(world, renderView, tileMap);

        GRenderer.Swap(GApp);
    }

    GRenderer.DestroyTexture(playerWalkTexture);
    GRenderer.DestroyTexture(wallTexture);
    GRenderer.DestroyTexture(floorTexture);
    GRenderer.DestroyMesh(quadMesh);
    GRenderer.DestroyProgram(shaderProgram);

    GRenderer.Destroy();
    GApp.Destroy();

    return 0;
}
*/