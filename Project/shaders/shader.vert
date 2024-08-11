#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 projection;
} ubo;

layout(push_constant) uniform PushConstants
{
    mat4 model;
} push;

layout(location = 0) out vec3 fragColor;

layout(location = 0) in vec2 inPos; 
layout(location = 1) in vec3 inColor;

void main() {
    gl_Position = ubo.projection * push.model * vec4(inPos, 0.0, 1.0);
    fragColor = inColor;
}

