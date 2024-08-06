#pragma once
#include "vulkan/vulkan.h"
#include <CommandPool.h>


class BaseBuffer {
//public:
//    BaseBuffer() = default;
//    ~BaseBuffer() = default;
//
//    virtual void initialize(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const CommandPool& commandPool, const VkQueue& graphicsQueue);
//
//    void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
//    void copyFrom(const VkBuffer& buffer, const VkBuffer& srcBuffer, const VkDeviceSize& size);
//    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
//
//protected:
//    VkCommandBuffer beginSingleTimeCommands();
//    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
//
//    VkDevice m_Device;
//    VkPhysicalDevice m_PhysicalDevice;
//    CommandPool m_CommandPool;
//    VkQueue m_GraphicsQueue;
};


