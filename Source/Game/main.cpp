#include <iostream>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb/stb_image.h"

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

        out vec2 vTexCoord;

        void main()
        {
            vTexCoord = aTexCoord;
            gl_Position = vec4(aPosition, 0.0, 1.0);
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

    SDL_Window* window = SDL_CreateWindow("Unicorn Engine", 1280, 720, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    const SDL_GLContext context = SDL_GL_CreateContext(window);
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

    constexpr float vertices[] =
    {
        // position     //uv
        -0.15f, -0.15f, 0.0f, 0.0f,
         0.15f, -0.15f, 1.0f, 0.0f,
         0.15f,  0.15f, 1.0f, 1.0f,
        -0.15f,  0.15f, 0.0f, 1.0f,
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

    const GLuint texture = LoadTexture("../Assets/hacker.png");
    if (texture == 0)
    {
        return 0;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        glViewport(0, 0, 1280, 720);
        glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);
    }

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
