#pragma once
#include <stdexcept>
#include "vulkan/vulkan.h"
#include "BaseBuffer.h"
#include <stdexcept>

class DataBuffer
{
public:
    DataBuffer(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkDeviceSize size
    );

    void upload(VkDeviceSize size, void* data);
    void map(VkDeviceSize size, void* data);
    void destroy(VkDevice device);
    void bindAsVertexBuffer(CommandBuffer commandBuffer);
    void bindAsIndexBuffer(CommandBuffer commandBuffer);
    VkBuffer getVkBuffer() const;
    VkDeviceSize getSizeInBytes() const;

    void copyBuffer(QueueFamilyIndices queueFamInd, DataBuffer dstBuffer, VkQueue graphicsQueue);
private:
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    VkDevice m_Device;
    VkDeviceSize m_Size;
    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
};
