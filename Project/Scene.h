#pragma once
#include <vector>
#include "vulkan/vulkan.h"
#include <glm/glm.hpp>
#include <vulkanbase/VulkanUtil.h>
#include <CommandBuffer.h>
#include <meshes/Mesh.h>
#include <Camera.h>
#include <GraphicsPipeline.h>
#include <loadObjFile.h>
#include <texture/MaterialManager.h>


template <typename VertexType>
class Scene {
public:
    Scene() = default;

    void create2DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue);
    void create3DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue);
    void create3DScene_PBR(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager);

    void addMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue);
    void cleanUp(const VkDevice& device);

    void draw2D(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex);
    void draw3D(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex);
    void draw3D_PBR(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex);


    void update(float deltaTime);
private:
    std::vector<Mesh<VertexType>> m_Meshes;
    float m_RotationAngle = 0.0f;
};

template <typename VertexType>
void Scene<VertexType>::update(float deltaTime) {
    constexpr float rotationSpeed = glm::radians(90.0f);
    m_RotationAngle += rotationSpeed * deltaTime;

    if (!m_Meshes.empty()) {
        m_Meshes[0].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-8.5f, 0.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), { 1.0f, 1.0f, 1.0f });
    }
    if (!m_Meshes.empty()) {
        m_Meshes[3].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, 0.5f, 0.0f)) * glm::rotate(glm::mat4(1.0f), m_RotationAngle, glm::vec3(1.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });
    }

    if (m_Meshes.size() > 1) {
        float radiusX = 15.0f;
        float radiusZ = 15.0f;
        float radiusY = 15.0f;
        float speed = 1.0f;
        float time = m_RotationAngle * speed;

        float x = radiusX * cos(time);
        float z = radiusZ * sin(time);
        float y = radiusY * sin(time * 2);

        glm::vec2 direction = glm::normalize(glm::vec2(cos(time), sin(time)));

        float forwardAngle = atan2(direction.y, direction.x);

        float verticalSpeed = radiusY * cos(time * 2);
        float tiltAngle = atan2(verticalSpeed, sqrt(x * x + z * z));

        m_Meshes[1].m_ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3( -11.5f, 0.5f, 0 ))
            * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
            * glm::rotate(glm::mat4(1.0f), forwardAngle, glm::vec3(0.0f, -1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f), tiltAngle, glm::vec3(0.0f, 0.0f, 1.0f))
            * glm::rotate(glm::mat4(1.0f), m_RotationAngle * 2.5f, glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

    }
}

template <typename VertexType>
void Scene<VertexType>::create2DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue) {
    std::vector<glm::vec3> colors{
     {1.f, 0.f, 0.f},    // Red
     {0.f, 1.f, 0.f},    // Green
     {0.f, 0.f, 1.f},    // Blue
     {0.5f, 0.5f, 0.5f}, // Gray
     {1.f, 0.5f, 0.f},   // Orange
     {0.5f, 0.f, 0.5f},  // Purple
     {0.f, 1.f, 1.f},    // Cyan
     {1.f, 0.f, 1.f},    // Magenta
     {1.f, 1.f, 0.f},    // Yellow
     {0.5f, 0.25f, 0.f}, // Brown
     {0.25f, 0.5f, 0.75f}, // Light Blue
     {0.75f, 0.5f, 0.25f}, // Tan
     {0.5f, 0.75f, 0.5f}, // Light Green
     {0.25f, 0.25f, 0.75f}, // Slate Blue
     {0.75f, 0.25f, 0.75f}  // Light Purple
    };

    std::vector<glm::vec3> colors2{ {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f} };

    Mesh<VertexType> myRectangle = Mesh<VertexType>::CreateRectangle({ 1.0f, 0.2f }, 0.3f, 0.3f, { 0.0f, 0.0f, 0.3f });
    Mesh<VertexType> myRectangle2 = Mesh<VertexType>::CreateRectangle({ 1.0f, 0.6f }, 0.3f, 0.3f, { colors });

    Mesh<VertexType> myEllipse = Mesh<VertexType>::CreateEllipse({ 1.0f, -0.6f }, 0.3f, 0.3f, { 0.5f, 0.0f, 0.0f }, 10);
    Mesh<VertexType> myEllipse2 = Mesh<VertexType>::CreateEllipse({ 1.0f, -0.2f }, 0.3f, 0.3f, { 0.5f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.3f }, 30);

    addMesh(myRectangle, device, physDevice, queueFamily, graphicsQueue);
    addMesh(myRectangle2, device, physDevice, queueFamily, graphicsQueue);
    addMesh(myEllipse, device, physDevice, queueFamily, graphicsQueue);
    addMesh(myEllipse2, device, physDevice, queueFamily, graphicsQueue);
}

template <typename VertexType>
void Scene<VertexType>::create3DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue) 
{
    Mesh<Vertex3D> meshPyramid;

    std::vector<Vertex3D> pyramidVertices{
        {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{1.f, 0.f, 0.f}, glm::vec2{0.f, 0.f}},
        {glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec2{1.f, 0.f}},
        {glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{0.f, 0.f, 1.f}, glm::vec2{1.f, 1.f}},
        {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{1.f, 1.f, 0.f}, glm::vec2{0.f, 1.f}},

        {glm::vec3{ 0.0f, 0.5f, 0.0f}, glm::vec3{1.f, 1.f, 1.f}, glm::vec2{0.5f, 1.0f}},
    };

    std::vector<uint32_t> pyramidIndices{
        0, 3, 2,
        0, 2, 1,

        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    meshPyramid.setVertices(pyramidVertices);
    meshPyramid.setIndices(pyramidIndices);

    meshPyramid.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -14.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

    addMesh(meshPyramid, device, physDevice, queueFamily, graphicsQueue);

    Mesh<VertexType> meshSquare;

    std::vector<VertexType> squareVertices{
        {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{1.f, 0.f, 1.f}, glm::vec2{0.f, 0.f}},
        {glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec3{0.f, 1.f, 1.f}, glm::vec2{1.f, 0.f}},
        {glm::vec3{0.5f, -0.5f,  0.5f}, glm::vec3{0.f, 0.f, 1.f}, glm::vec2{1.f, 1.f}},
        {glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{1.f, 1.f, 0.f}, glm::vec2{0.f, 1.f}},
    };

    std::vector<uint32_t> squareIndices{
        0, 1, 2,
        0, 2, 3
    };

    meshSquare.setVertices(squareVertices);
    meshSquare.setIndices(squareIndices);

    meshSquare.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -17.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

    addMesh(meshSquare, device, physDevice, queueFamily, graphicsQueue);


    Mesh<VertexType> meshCube;

    std::vector<VertexType> cubeVertices{
        {glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{1.f, 0.f, 0.f}, glm::vec2{0.f, 0.f}},
        {glm::vec3{0.5f, -0.5f, -0.5f}, glm::vec3{0.f, 1.f, 0.f}, glm::vec2{1.f, 0.f}},
        {glm::vec3{0.5f, 0.5f, -0.5f}, glm::vec3{0.f, 0.f, 1.f}, glm::vec2{1.f, 1.f}},
        {glm::vec3{-0.5f, 0.5f, -0.5f}, glm::vec3{1.f, 1.f, 0.f}, glm::vec2{0.f, 1.f}},

        {glm::vec3{-0.5f, -0.5f, 0.5f}, glm::vec3{1.f, 0.f, 1.f}, glm::vec2{0.f, 0.f}},
        {glm::vec3{0.5f, -0.5f, 0.5f}, glm::vec3{0.f, 1.f, 1.f}, glm::vec2{1.f, 0.f}},
        {glm::vec3{0.5f, 0.5f, 0.5f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec2{1.f, 1.f}},
        {glm::vec3{-0.5f, 0.5f, 0.5f}, glm::vec3{1.f, 1.f, 1.f}, glm::vec2{0.f, 1.f}}
    };

    std::vector<uint32_t> cubeIndices{
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        0, 3, 7,
        0, 7, 4,

        1, 5, 6,
        1, 6, 2,

        3, 2, 6,
        3, 6, 7,

        0, 4, 5,
        0, 5, 1
    };

    meshCube.setVertices(cubeVertices);
    meshCube.setIndices(cubeIndices);

    meshCube.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -11.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

    addMesh(meshCube, device, physDevice, queueFamily, graphicsQueue);
}

template <typename VertexType>
void Scene<VertexType>::create3DScene_PBR(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager)
{
    auto texture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_diffuse.png");
    auto texture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_normal.png");
    auto texture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_specular.png");
    auto texture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/vehicle/vehicle_gloss.png");

    auto myMaterial = materialManager.createMaterial(device, { texture, texture2, texture3, texture4 });

    auto brickTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_diffuse.png");
    auto brickTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_normal_small.png");
    auto brickTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_specular.png");
    auto brickTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/bricks/Bricks_gloss.png");

    auto myBrickMaterial = materialManager.createMaterial(device, { brickTexture, brickTexture2, brickTexture3, brickTexture4 });

    auto defaultTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/uv_grid_3.png");
    auto defaultTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/defaultNormal.png");
    auto defaultTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/defaultBlack.png");
    auto defaultTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/uv_grid/uv_grid.png");

    auto mydefaultTextureMaterial = materialManager.createMaterial(device, { defaultTexture, defaultTexture2, defaultTexture3, defaultTexture4 });

    auto dirtTexture = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Diffuse_small.png");
    auto dirtTexture2 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Normal_small.png");
    auto dirtTexture3 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Specular_small.png");
    auto dirtTexture4 = std::make_shared<Texture>(device, physDevice, commandPool, graphicsQueue, "models/dirt/Dirt wet_4K_Gloss_small.png");

    auto myDirtTextureMaterial = materialManager.createMaterial(device, { dirtTexture, dirtTexture2, dirtTexture3, dirtTexture4 });


    Mesh<VertexType> sphere;
    std::vector<VertexType> sphereVertices;
    std::vector<uint32_t> sphereIndices;


    if (ObjLoader::loadObjFile("models/sphere.obj", sphereVertices, sphereIndices)) {
        sphere.setVertices(sphereVertices);
        sphere.setIndices(sphereIndices);
        sphere.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -8.5f, 0.5f, 0 });
        sphere.m_Material = myDirtTextureMaterial;

        addMesh(sphere, device, physDevice, queueFamily, graphicsQueue);
    }

    Mesh<VertexType> vehicle;
    std::vector<VertexType> vehicleVertices;
    std::vector<uint32_t> vehicleIndices;

    if (ObjLoader::loadObjFile("models/vehicle.obj", vehicleVertices, vehicleIndices)) {
        vehicle.setVertices(vehicleVertices);
        vehicle.setIndices(vehicleIndices);
        vehicle.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { 0.5f, 0.5f, 0 }) * 
            glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });
        vehicle.m_Material = myMaterial;

        addMesh(vehicle, device, physDevice, queueFamily, graphicsQueue);
    }

    Mesh<VertexType> diorama;
    std::vector<VertexType> dioramaVertices;
    std::vector<uint32_t> dioramaIndices;

    if (ObjLoader::loadObjFile("models/diorama.obj", dioramaVertices, dioramaIndices)) {
        diorama.setVertices(dioramaVertices);
        diorama.setIndices(dioramaIndices);
        diorama.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { 20.0f, 0, 0 }) * rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), { 0.2f, 0.2f, 0.2f });
        diorama.m_Material = mydefaultTextureMaterial;

        addMesh(diorama, device, physDevice, queueFamily, graphicsQueue);
    }


    Mesh<VertexType> square;
    std::vector<VertexType> squareVertices;
    std::vector<uint32_t> squareIndices;

    if (ObjLoader::loadObjFile("models/square.obj", squareVertices, squareIndices)) {
        square.setVertices(squareVertices);
        square.setIndices(squareIndices);
        square.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -5.5f, 0.5, 0 }) * rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
        square.m_Material = myBrickMaterial;

        addMesh(square, device, physDevice, queueFamily, graphicsQueue);
    }
}

template <typename VertexType>
void Scene<VertexType>::addMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue) {
    mesh.initialize(device, physDevice, queueFamily, graphicsQueue, mesh.getVertices(), mesh.getIndices());
    m_Meshes.push_back(std::move(mesh));
}

template <typename VertexType>
void Scene<VertexType>::draw2D(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex) {
    UniformBufferObject2D ubo2D{};
    ubo2D.proj = camera.GetOrthoProjectionMatrix();

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo2D, sizeof(ubo2D));

    for (auto& mesh : m_Meshes) {

        MeshPushConstants meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        mesh.draw(commandBuffer, graphicsPipeline.getPipelineLayout());
    }
}

template <typename VertexType>
void Scene<VertexType>::draw3D(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex) {
    UniformBufferObject3D ubo3D{};
    ubo3D.viewProjection = camera.GetViewProjection(0.1f, 100.f);
    ubo3D.viewPosition = glm::vec4(camera.origin, 1.0f);

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

    for (auto& mesh : m_Meshes) {
        MeshPushConstants meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        mesh.draw(commandBuffer, graphicsPipeline.getPipelineLayout());
    }
}

template <typename VertexType>
void Scene<VertexType>::draw3D_PBR(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex) {
    UniformBufferObject3D ubo3D{};
    ubo3D.viewProjection = camera.GetViewProjection(0.1f, 100.f);
    ubo3D.viewPosition = glm::vec4(camera.origin, 1.0f);

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

    for (auto& mesh : m_Meshes) {
        MeshPushConstants meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        if (mesh.m_Material != nullptr)
        {
            graphicsPipeline.updateMaterial(commandBuffer.getVkCommandBuffer(), *mesh.m_Material);
        }

        mesh.draw(commandBuffer, graphicsPipeline.getPipelineLayout());
    }
}

template <typename VertexType>
void Scene<VertexType>::cleanUp(const VkDevice& device) {
    for (auto& mesh : m_Meshes) {
        if (mesh.m_Material != nullptr)
        {
            mesh.m_Material->cleanup(device);
        }
        mesh.cleanUp(device);
    }
}