#pragma once
#include <vector>
#include "vulkan/vulkan.h"
#include <glm/glm.hpp>
#include <vulkanbase/VulkanUtil.h>
#include <CommandBuffer.h>
#include <meshes/Mesh.h>


template <typename VertexType>
class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void Create2DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue);

    void AddMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue);

    void draw(CommandBuffer commandBuffer, int imageIndex);
    void cleanUp(const VkDevice& device);
private:
    std::vector<Mesh<VertexType>> m_pMeshes;
};

template <typename VertexType>
void Scene<VertexType>::Create2DScene(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue) {
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

    Mesh<Vertex2D> myRectangle = Mesh<Vertex2D>::CreateRectangle({ 0.8f, 0.2f }, 0.3f, 0.3f, { 1.0f, 0.1f, 0.0f });
    Mesh<Vertex2D> myRectangle2 = Mesh<Vertex2D>::CreateRectangle({ 0.8f, 0.6f }, 0.3f, 0.3f, { colors });

    Mesh<Vertex2D> myEllipse = Mesh<Vertex2D>::CreateEllipse({ 0.8f, -0.6f }, 0.3f, 0.3f, {0.0f, 0.5f, 0.5f}, 10);
    Mesh<Vertex2D> myEllipse2 = Mesh<Vertex2D>::CreateEllipse({ 0.8f, -0.2f }, 0.3f, 0.3f, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, 30);

    AddMesh(myRectangle, device, physDevice, queueFamily, graphicsQueue);
    AddMesh(myRectangle2, device, physDevice, queueFamily, graphicsQueue);
    AddMesh(myEllipse, device, physDevice, queueFamily, graphicsQueue);
    AddMesh(myEllipse2, device, physDevice, queueFamily, graphicsQueue);
}

template <typename VertexType>
void Scene<VertexType>::AddMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue) {
    mesh.initialize(device, physDevice, queueFamily, graphicsQueue, mesh.getVertices(), mesh.getIndices());
    m_pMeshes.push_back(std::move(mesh));
}

template <typename VertexType>
void Scene<VertexType>::draw(CommandBuffer commandBuffer, int imageIndex) {
    for (auto& mesh : m_pMeshes) {
        mesh.draw(commandBuffer);
    }
}

template <typename VertexType>
void Scene<VertexType>::cleanUp(const VkDevice& device)
{
    for (auto& mesh : m_pMeshes) {
        mesh.cleanUp(device);
    }
}