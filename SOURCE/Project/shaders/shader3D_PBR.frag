#version 450

const float M_PI = 3.14159265358979323846;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
    vec4 viewPosition;
    vec3 lightDirection;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    int renderMode;
} push;

layout(set = 1, binding = 0) uniform sampler2D diffuseSample;
layout(set = 1, binding = 1) uniform sampler2D normalSample;
layout(set = 1, binding = 2) uniform sampler2D specularSample;
layout(set = 1, binding = 3) uniform sampler2D roughnessSample;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inUV;

layout(location = 0) out vec4 outColor;

vec3 lightColor = vec3(1.0);
vec3 ambientLight = vec3(0.03);

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float a = roughness * roughness;
    float k = (a * a) / 2.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return a / denom;
}

vec3 cookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 F0, float roughness) {
    vec3 H = normalize(V + L);
    float D = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    return (D * G * F) / max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.001);
}

void main() {
    vec3 diffuse = texture(diffuseSample, inUV).rgb;
    vec3 normalMap = texture(normalSample, inUV).rgb;
    vec3 specularColor = texture(specularSample, inUV).rgb;
    float roughness = texture(roughnessSample, inUV).r;

    vec3 normal = normalize(normalMap * 2.0 - 1.0);
    vec3 T = normalize(inTangent - inNormal * dot(inNormal, inTangent));
    vec3 B = normalize(cross(inNormal, T));
    mat3 TBN = mat3(T, B, inNormal);
    normal = normalize(TBN * normal);

    vec3 viewDir = normalize(ubo.viewPosition.xyz - inWorldPosition);
    vec3 lightDir = normalize(ubo.lightDirection);

    vec3 F0 = specularColor;

    vec3 specular = cookTorranceBRDF(normal, viewDir, lightDir, F0, roughness);

    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLighting = diffuse * lightColor * NdotL;

    vec3 ambient = ambientLight * diffuse;

    vec3 finalColor;
    switch (push.renderMode) {
        case 0:
            finalColor = diffuseLighting;
            break;
        case 1:
            finalColor = normal;
            break;
        case 2:
            finalColor = specular;
            break;
        default:
            finalColor = ambient + (diffuseLighting * (1.0 - max(max(specular.r, specular.g), specular.b)) + specular * lightColor);
            break;
    }

    outColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}