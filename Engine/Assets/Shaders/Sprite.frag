#version 460 core

layout(location = 0) in vec2 vUV;

layout(binding = 0) uniform sampler2D uTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 color = texture(uTexture, vUV);

    if(color.a < 0.01)
        discard;

    FragColor = color;
}