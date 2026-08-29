#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aUV;

layout(location = 0) out vec2 vUV;

uniform mat4 uProjectionView;
uniform mat4 uModel;
uniform vec4 uUvRect;

void main()
{
    vUV = mix(uUvRect.xy, uUvRect.zw, aUV);
    //vUV = aUV;

    gl_Position = uProjectionView * uModel * vec4(aPosition, 1.0);
}