#version 460 core

layout(std140, binding = 0) uniform FrameData
{
      mat4 projView;
      vec3 cameraPos;
      float framePad0;
};

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aUV;

uniform mat4 uModelTransform;

out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vUV;
out vec3 vWorldPos;

void main()
{
   vec4 worldPos = uModelTransform * aPos;
   gl_Position = projView * worldPos;

   vWorldPos = worldPos.xyz;
   mat3 normalMatrix = transpose(inverse(mat3(uModelTransform)));
   vNormal = normalMatrix * aNormal;
   vTangent = mat3(uModelTransform) * aTangent;
   vBitangent = mat3(uModelTransform) * aBitangent;
   vUV = aUV;
}