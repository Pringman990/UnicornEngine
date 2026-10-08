#version 460 core

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vNormal;

layout(binding = 0) uniform sampler2D uAlbedo;
uniform vec3 uLightDirection; // Normalized direction towards the light, in world space.
uniform float uAmbientStrength;
uniform vec3 uTint = vec3(1.0);

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 colour = texture(uAlbedo, vUV);
    if (colour.a < 0.01)
        discard;

    colour.rgb *= uTint;

    float diffuse = max(dot(normalize(vNormal), uLightDirection), 0.0);
    float illumination = mix(uAmbientStrength, 1.0, diffuse);

    // The current texture upload and framebuffer use no automatic sRGB conversion.
    // Approximate it here so lighting is applied to linear colour.
    vec3 linearColour = pow(colour.rgb, vec3(2.2));
    FragColor = vec4(pow(linearColour * illumination, vec3(1.0 / 2.2)), colour.a);
}
