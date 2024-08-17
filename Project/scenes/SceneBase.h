#pragma once
#include <vector>
#include "vulkan/vulkan.h"
#include <glm/glm.hpp>
#include <vulkanbase/VulkanUtil.h>
#include <buffers/CommandBuffer.h>
#include <meshes/Mesh.h>
#include <Camera.h>
#include <GraphicsPipeline.h>
#include <loadObjFile.h>
#include <texture/MaterialManager.h>

template <typename VertexType>
class SceneBase {
public:
    SceneBase() = default;
    virtual ~SceneBase() = default;

    virtual void createScene(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, QueueFamilyIndices queueFamily, const VkQueue& graphicsQueue, MaterialManager& materialManager) = 0;
    virtual void draw(Camera& camera, CommandBuffer& commandBuffer, GraphicsPipeline& graphicsPipeline, SwapChain& swapChain, int imageIndex, int renderMode = 0) = 0;
    virtual void update(float deltaTime) = 0;

    void addMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue);
    void cleanUp(const VkDevice& device);

protected:
    std::vector<Mesh<VertexType>> m_Meshes;
    float m_RotationAngle = 0.0f;
    PhysicsEngine physicsEngine;
};

template <typename VertexType>
void SceneBase<VertexType>::addMesh(Mesh<VertexType>& mesh, const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue) {
    mesh.initialize(device, physDevice, queueFamily, graphicsQueue, mesh.getVertices(), mesh.getIndices());
    m_Meshes.push_back(std::move(mesh));
}

template <typename VertexType>
void SceneBase<VertexType>::cleanUp(const VkDevice& device) {
    for (auto& mesh : m_Meshes) {
        if (mesh.m_pMaterial != nullptr)
        {
            mesh.m_pMaterial->cleanup(device);
        }
        mesh.cleanUp(device);
    }
}