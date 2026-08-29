#version 460 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aUV;
layout(location = 5) in ivec4 aBones;
layout(location = 6) in vec4 aWeight;

uniform mat4 uProjectionView;

uniform mat4 uModelTransform;
uniform mat4 uBones[100];

void main()
{
   mat4 skin =
   aWeight.x * uBones[aBones.x] +
   aWeight.y * uBones[aBones.y] +
   aWeight.z * uBones[aBones.z] +
   aWeight.w * uBones[aBones.w];

   vec4 skinned = skin * aPos;

   vec4 worldPos = uModelTransform * skinned;
   gl_Position = projView * worldPos;
}