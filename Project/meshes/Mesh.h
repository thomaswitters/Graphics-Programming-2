#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <glm/fwd.hpp>
#include <Vertex.h>
#include "DescriptorPool.h"
#include "VulkanBufferHandler.h"
#include "CommandBuffer.h"
#include "texture/Material.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename VertexType>
class Mesh {
public:
    Mesh() = default;

    void initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue, const std::vector<VertexType> vertices, std::vector<uint32_t> indices);
    void draw(CommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const;
    void cleanUp(const VkDevice& device);

    const std::vector<VertexType>& getVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_Indices; }

    void setVertices(const std::vector<VertexType>& vertices) { m_Vertices = vertices; }
    void setIndices(const std::vector<uint32_t>& indices) { m_Indices = indices; }

    const DescriptorPool& getDescriptorPool() const { return m_DescriptorPool; }

    static  Mesh<Vertex2D> CreateRectangle(glm::vec2 center, float width, float height, const glm::vec3& color);
    static Mesh<Vertex2D> CreateRectangle(glm::vec2 center, float width, float height, const std::vector<glm::vec3>& colors);

    static Mesh<Vertex2D> CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& color, int nrOfVertexes);
    static Mesh<Vertex2D> CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& innerColor, const glm::vec3& outerColor, int nrOfVertexes);

    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    std::shared_ptr<Material> m_Material;
private:
    std::unique_ptr<DataBuffer> m_VertexBuffer;
    std::unique_ptr<DataBuffer> m_IndexBuffer;

    std::vector<VertexType> m_Vertices;
    std::vector<uint32_t> m_Indices;
};



template <typename VertexType>
void Mesh<VertexType>::initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, QueueFamilyIndices queueFamily, VkQueue graphicsQueue, const std::vector<VertexType> vertices, std::vector<uint32_t> indices) {
    m_Vertices = vertices;
    m_Indices = indices;

    size_t vertexBufferSize = sizeof(VertexType) * m_Vertices.size();
    size_t indexBufferSize = sizeof(uint32_t) * m_Indices.size();

    auto stagingVertexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBufferSize
    );

    stagingVertexBuffer->upload(vertexBufferSize, m_Vertices.data());

    m_VertexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBufferSize
    );


    stagingVertexBuffer->copyBuffer(queueFamily, *m_VertexBuffer, graphicsQueue);
    stagingVertexBuffer->destroy(device);

    auto stagingIndexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        indexBufferSize
    );


    stagingIndexBuffer->upload(indexBufferSize, m_Indices.data());

    m_IndexBuffer = std::make_unique<DataBuffer>(
        physDevice,
        device,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBufferSize
    );

    stagingIndexBuffer->copyBuffer(queueFamily, *m_IndexBuffer, graphicsQueue);
    stagingIndexBuffer->destroy(device);
}

template <typename VertexType>
void Mesh<VertexType>::draw(CommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const {
    VkBuffer vertexBuffers[] = { m_VertexBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer.getVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer.getVkCommandBuffer(), m_IndexBuffer->getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer.getVkCommandBuffer(), static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

template <typename VertexType>
void Mesh<VertexType>::cleanUp(const VkDevice& device) {
    m_VertexBuffer->destroy(device);
    m_IndexBuffer->destroy(device);
}
