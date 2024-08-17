#pragma once
#include "SceneBase.h"

template <typename VertexType>
class Scene2D : public SceneBase<VertexType> {
public:
    void createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) override;
    void draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode = 0) override;
    void update(float deltaTime) override {}
};

template <typename VertexType>
void Scene2D<VertexType>::createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) {
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
void Scene2D<VertexType>::draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode) {
    UniformBufferObject2D ubo2D{};
    ubo2D.proj = camera.getOrthoProjectionMatrix();

    graphicsPipeline.bind(commandBuffer.getVkCommandBuffer(), swapChain, imageIndex);
    graphicsPipeline.updateUBO(imageIndex, &ubo2D, sizeof(ubo2D));

    for (auto& mesh : m_Meshes) {

        PushConstants meshPushConstant{};
        meshPushConstant.model = mesh.m_ModelMatrix;

        graphicsPipeline.updatePushConstrant(commandBuffer.getVkCommandBuffer(), &meshPushConstant, sizeof(meshPushConstant));

        mesh.draw(commandBuffer);
    }
}