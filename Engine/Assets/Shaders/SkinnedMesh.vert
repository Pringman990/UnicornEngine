#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 4) in vec2 aUV;
layout(location = 5) in uvec4 aJoints;
layout(location = 6) in vec4 aWeights;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec3 vNormal;

layout(std140, binding = 0) uniform SkinPose
{
    mat4 uJoints[128];
};

uniform mat4 uProjectionView;
uniform mat4 uModel;

void main()
{
    mat4 skin = aWeights.x * uJoints[aJoints.x] +
        aWeights.y * uJoints[aJoints.y] +
        aWeights.z * uJoints[aJoints.z] +
        aWeights.w * uJoints[aJoints.w];

    vUV = aUV;
    vNormal = transpose(inverse(mat3(uModel))) * mat3(skin) * aNormal;
    gl_Position = uProjectionView * uModel * skin * vec4(aPosition, 1.0);
}
