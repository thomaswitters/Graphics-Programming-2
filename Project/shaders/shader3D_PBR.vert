#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 viewProjection;
    vec4 viewPosition;
    vec3 lightDirection;
} ubo;

layout(push_constant) uniform PushConstants
{
    mat4 model;
    int renderMode;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;   
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec2 outUV;

void main() {
    fragColor = inColor;

    outWorldPosition = vec3(push.model * vec4(inPosition, 1.0));
    
    mat3 normalMatrix = transpose(inverse(mat3(push.model)));
    outNormal = normalize(normalMatrix * inNormal);
    outTangent = normalize(normalMatrix * inTangent);

    outUV = inUV;

    gl_Position = ubo.viewProjection * vec4(outWorldPosition, 1.0);
}
