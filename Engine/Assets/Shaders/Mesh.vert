#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 4) in vec2 aUV;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec3 vNormal;

uniform mat4 uProjectionView;
uniform mat4 uModel;

void main()
{
    vUV = aUV;
    // Keep normals perpendicular to the surface when the model has nonuniform scale.
    vNormal = transpose(inverse(mat3(uModel))) * aNormal;
    gl_Position = uProjectionView * uModel * vec4(aPosition, 1.0);
}
