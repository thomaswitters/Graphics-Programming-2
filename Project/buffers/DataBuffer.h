#pragma once
#include <stdexcept>
#include "vulkan/vulkan.h"
#include <stdexcept>
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "vulkanbase/VulkanUtil.h"
#include <iostream>
#include <cstring>

class DataBuffer
{
public:
    DataBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size);
    ~DataBuffer() = default;

    void upload(VkDeviceSize size, void* data);
    void map(VkDeviceSize size);
    void cleanup(VkDevice device);
    VkBuffer getVkBuffer() const;
    VkDeviceMemory getVkDeviceMemory() const;
    VkDeviceSize getSizeInBytes() const;

    void copyBuffer(QueueFamilyIndices queueFamInd, DataBuffer dstBuffer, VkQueue graphicsQueue);
private:
    void createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    VkDevice m_Device;
    VkDeviceSize m_Size;
    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;

    void* m_pBufferData = nullptr;
};
