#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
} ubo;

layout(push_constant) uniform PushConstants
{
    mat4 model;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.viewProjection * push.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = texCoord;
}

