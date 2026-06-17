#include <iostream>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb/stb_image.h"

constexpr float WindowSizeX = 1280;
constexpr float WindowSizeY = 720;

constexpr float playerHalfWidth = 0.35f;
constexpr float playerHalfHeight = 0.35f;

constexpr int tileSize = 64;
constexpr int roomWidth = 15;
constexpr int roomHeight = 11;

constexpr float roomOffsetX = (WindowSizeX - roomWidth * tileSize) * 0.5f;
constexpr float roomOffsetY = (WindowSizeY - roomHeight * tileSize) * 0.5f;

const char* roomTiles[roomHeight] =
{
    "XXXXXXXXXXXXXXX",
    "XOOOOOOOOOOOOOX",
    "XOOOOOOOOOOOOOX",
    "XOOXXXOOOXXXOOX",
    "XOOOOOOOOOOOOOX",
    "XOOOOOOOOOOOOOX",
    "XOOOOOOOOOOOOOX",
    "XOOXXXOOOXXXOOX",
    "XOOOOOOOOOOOOOX",
    "XOOOOOOOOOOOOOX",
    "XXXXXXXXXXXXXXX"
};

static GLuint LoadTexture(const char* path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels = 0;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (data == nullptr)
    {
        std::cerr << "Failed to load image! " << path << std::endl;
        std::cerr << "Error: " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLenum format = GL_RGB;

    if (channels == 4)
    {
        format = GL_RGBA;
    }
    else if (channels == 3)
    {
        format = GL_RGB;
    }
    else if (channels == 1)
    {
        format = GL_RED;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

static GLuint CompileShader(const GLuint shaderType, const char* shaderSrc)
{
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed! " << log << std::endl;
    }

    return shader;
}

static GLuint CreateShaderProgram()
{
    const auto vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec2 aPosition;
        layout (location = 1) in vec2 aTexCoord;

        uniform vec2 uPosition;
        uniform vec2 uWindowSize;

        out vec2 vTexCoord;

        void main()
        {
            vec2 pixelPosition = aPosition + uPosition;

            vec2 clipPosition;
            clipPosition.x = (pixelPosition.x / uWindowSize.x) * 2.0 - 1.0;
            clipPosition.y = 1.0 - (pixelPosition.y / uWindowSize.y) * 2.0;

            vTexCoord = aTexCoord;
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

    const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

bool IsWall(int x, int y)
{
    if (x < 0 || y < 0 || x >= roomWidth || y >= roomHeight)
    {
        return true;
    }

    return roomTiles[y][x] == 'X';
}

bool CollidesWithWall(float x, float y)
{
    int left   = static_cast<int>(std::floor(x - playerHalfWidth));
    int right  = static_cast<int>(std::floor(x + playerHalfWidth));
    int top    = static_cast<int>(std::floor(y - playerHalfHeight));
    int bottom = static_cast<int>(std::floor(y + playerHalfHeight));

    for (int tileY = top; tileY <= bottom; ++tileY)
    {
        for (int tileX = left; tileX <= right; ++tileX)
        {
            if (IsWall(tileX, tileY))
            {
                return true;
            }
        }
    }

    return false;
}

float TileToPixelX(float tileX)
{
    return roomOffsetX + tileX * tileSize;
}

float TileToPixelY(float tileY)
{
    return roomOffsetY + tileY * tileSize;
}

int main(int argc, char* argv[])
{
    std::cout << "Base path: " << SDL_GetBasePath() << std::endl;

    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_Window* window = SDL_CreateWindow("Unicorn Engine", WindowSizeX, WindowSizeY, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "SDL_GL_CreateContext failed! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
    {
        std::cerr << "Failed to load OpenGL through GLAD!" << std::endl;
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const GLuint shaderProgram = CreateShaderProgram();

    constexpr float halfTileSize = tileSize * 0.5f;

    constexpr float vertices[] =
    {
        // position                    // uv
        -halfTileSize, -halfTileSize,   0.0f, 0.0f,
         halfTileSize, -halfTileSize,   1.0f, 0.0f,
         halfTileSize,  halfTileSize,   1.0f, 1.0f,
        -halfTileSize,  halfTileSize,   0.0f, 1.0f,
    };

    const uint32_t indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const GLuint playerTexture = LoadTexture("../Assets/hacker.png");
    if (playerTexture == 0)
    {
        return 0;
    }

    const GLuint floorTexture = LoadTexture("../Assets/Floor.png");
    if (floorTexture == 0)
    {
        return 0;
    }

    const GLuint wallTexture = LoadTexture("../Assets/Wall.png");
    if (wallTexture == 0)
    {
        return 0;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float playerX = 7.5f;
    float playerY = 5.5f;
    const float playerSpeed = 4.f;

    const GLint positionLocation = glGetUniformLocation(shaderProgram, "uPosition");
    const GLint windowSizeLocation = glGetUniformLocation(shaderProgram, "uWindowSize");
    if (positionLocation == -1)
    {
        std::cerr << "Could not find uPosition uniform" << std::endl;
    }
    if (windowSizeLocation == -1)
    {
        std::cerr << "Could not find uWindowSize uniform" << std::endl;
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

        const bool* keyboard = SDL_GetKeyboardState(nullptr);

        float moveX = 0;
        float moveY = 0;

        if (keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_LEFT])
        {
            moveX -= 1.0f;
        }

        if (keyboard[SDL_SCANCODE_D] || keyboard[SDL_SCANCODE_RIGHT])
        {
            moveX += 1.0f;
        }

        if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP])
        {
            moveY -= 1.0f;
        }

        if (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN])
        {
            moveY += 1.0f;
        }

        if (moveX != 0.0f || moveY != 0.0f)
        {
            const float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX /= length;
            moveY /= length;
        }

        float nextX = playerX + moveX * playerSpeed * deltaTime;
        float nextY = playerY + moveY * playerSpeed * deltaTime;

        if (!CollidesWithWall(nextX, playerY))
        {
            playerX = nextX;
        }

        if (!CollidesWithWall(playerX, nextY))
        {
            playerY = nextY;
        }

        glViewport(0, 0, WindowSizeX, WindowSizeY);
        glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform2f(positionLocation, playerX, playerY);
        glUniform2f(windowSizeLocation, WindowSizeX, WindowSizeY);
        glBindVertexArray(vao);

        for (int y = 0; y < roomHeight; ++y)
        {
            for (int x = 0; x < roomWidth; ++x)
            {
                char tile = roomTiles[y][x];

                if (tile == 'X')
                {
                    glBindTexture(GL_TEXTURE_2D, wallTexture);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, floorTexture);
                }

                glUniform2f(
            positionLocation,
            TileToPixelX(static_cast<float>(x) + 0.5f),
            TileToPixelY(static_cast<float>(y) + 0.5f)
        );
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }

        glBindTexture(GL_TEXTURE_2D, playerTexture);
        glUniform2f(
            positionLocation,
            TileToPixelX(playerX),
            TileToPixelY(playerY)
        );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);
    }

    glDeleteTextures(1, &playerTexture);
    glDeleteTextures(1, &wallTexture);
    glDeleteTextures(1, &floorTexture);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
