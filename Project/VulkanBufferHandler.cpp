#include "VulkanBufferHandler.h"
#include <iostream>
#include <cstring>


DataBuffer::DataBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkDeviceSize size
) : m_Device(device), m_Size(size), m_Buffer(VK_NULL_HANDLE), m_BufferMemory(VK_NULL_HANDLE)
{
    createBuffer(physicalDevice, device, size, usage, properties);
}

void DataBuffer::upload(VkDeviceSize size, void* data) {
    void* mappedData;
    vkMapMemory(m_Device, m_BufferMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, (size_t)size);
    vkUnmapMemory(m_Device, m_BufferMemory);
}

void DataBuffer::map(VkDeviceSize size, void* data) {
    vkMapMemory(m_Device, m_BufferMemory, 0, size, 0, &data);
}

void DataBuffer::destroy(VkDevice device) {
    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}

void DataBuffer::bindAsVertexBuffer(CommandBuffer commandBuffer) {
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer.getVkCommandBuffer(), 0, 1, &m_Buffer, offsets);
}

void DataBuffer::bindAsIndexBuffer(CommandBuffer commandBuffer) {
    vkCmdBindIndexBuffer(commandBuffer.getVkCommandBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
}

VkBuffer DataBuffer::getVkBuffer() const {
    return m_Buffer;
}

VkDeviceSize DataBuffer::getSizeInBytes() const {
    return m_Size;
}

uint32_t DataBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void DataBuffer::createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
}

void DataBuffer::copyBuffer(QueueFamilyIndices queueFamInd, DataBuffer dstBuffer, VkQueue graphicsQueue)
{
    CommandPool commandPool{};
    commandPool.initialize(m_Device, queueFamInd);
    CommandBuffer commandBuffer = commandPool.createCommandBuffer(m_Device);

    commandBuffer.beginRecording();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = m_Size;
    vkCmdCopyBuffer(commandBuffer.getVkCommandBuffer(), m_Buffer, dstBuffer.getVkBuffer() , 1, &copyRegion);

    commandBuffer.endRecording();
   
   
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    commandBuffer.submit(submitInfo);
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    auto rawBuffer = commandBuffer.getVkCommandBuffer();
    vkFreeCommandBuffers(m_Device, commandPool.getCommandPool(), 1, &rawBuffer);
    commandPool.cleanup(m_Device);
}

