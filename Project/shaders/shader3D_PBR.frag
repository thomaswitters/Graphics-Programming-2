#version 450


const float M_PI = 3.14159265358979323846;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 viewProjection;
    vec4 viewPosition;
} ubo;

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

vec3 lightDirection = normalize(vec3(0.0, 0.8, -0.6));
vec3 lightColor = vec3(2.0, 2.0, 2.0);

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Cook-Torrance BRDF
vec3 cookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 F0, float roughness) {
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float k = roughness * roughness / 2.0;
    float G = NdotL / (NdotL * (1.0 - k) + k);
    G *= NdotV / (NdotV * (1.0 - k) + k);

    float alpha = roughness * roughness;
    float D = (alpha * alpha) / (M_PI * pow(NdotH * NdotH * (alpha * alpha - 1.0) + 1.0, 2.0));

    return (F * G * D) / max(4.0 * NdotL * NdotV, 0.001);
}

void main() {
    vec3 diffuse = texture(diffuseSample, inUV).rgb;
    vec3 normalMap = texture(normalSample, inUV).rgb;
    vec3 specularColor = texture(specularSample, inUV).rgb;
    float roughness = texture(roughnessSample, inUV).r;

    // Correctly transform the normal map
    vec3 normal = normalize(normalMap * 2.0 - 1.0);
    
    // Create TBN matrix to transform normal
    mat3 TBN = mat3(normalize(inTangent), normalize(cross(inNormal, inTangent)), normalize(inNormal));
    normal = normalize(TBN * normal);

    // Calculate view and light directions
    vec3 viewDir = normalize(ubo.viewPosition.xyz - inWorldPosition);
    vec3 lightDir = lightDirection;

    // Use a fixed F0 value for non-metallic materials
    vec3 F0 = vec3(0.04); // Base reflectance for non-metallic surfaces
    vec3 specular = cookTorranceBRDF(normal, viewDir, lightDir, F0, roughness);

    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLighting = diffuse * lightColor * NdotL;

    // Combine diffuse and specular contributions
    vec3 finalColor = diffuseLighting + specular * lightColor;

    // Final output color
    outColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}