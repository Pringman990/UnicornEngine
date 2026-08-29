#version 460 core

layout(location = 0) in vec2 vUV;

uniform vec4 uColor;

layout(location = 0) out vec4 FragColor;

void main()
{
    if(uColor.a < 0.01)
        discard;

    FragColor = uColor;
}