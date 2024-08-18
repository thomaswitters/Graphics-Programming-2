#pragma once
#include "SceneBase.h"

template <typename VertexType>
class Scene3D : public SceneBase<VertexType> {
public:
    void createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) override;
    void draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode = 0) override;
    void update(float deltaTime) override;
};

template <typename VertexType>
void Scene3D<VertexType>::createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) {
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

    meshPyramid.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -29.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

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

    meshSquare.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -32.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

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

    meshCube.m_ModelMatrix = glm::translate(glm::mat4(1.0f), { -26.5f, 0.5f, 0 }) * glm::scale(glm::mat4(1.0f), { 2.0f, 2.0f, 2.0f });

    addMesh(meshCube, device, physDevice, queueFamily, graphicsQueue);
}

template <typename VertexType>
void Scene3D<VertexType>::draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode) {
    UniformBufferObject3D ubo3D{};
    ubo3D.viewProjection = camera.getViewProjection(0.1f, 200.f);
    ubo3D.viewPosition = glm::vec4(camera.getOrigin(), 1.0f);

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo3D, sizeof(ubo3D));

    for (auto& mesh : m_Meshes) {
        PushConstants meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        mesh.draw(commandBuffer);
    }
}

template <typename VertexType>
void Scene3D<VertexType>::update(float deltaTime) {
    // 3D scene update logic
}