#include "BaseBuffer.h"
#include <stdexcept>
#include <cstring>

//void BaseBuffer::initialize(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const CommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//    m_Device = device;
//    m_PhysicalDevice = physicalDevice;
//    m_CommandPool = commandPool;
//    m_GraphicsQueue = graphicsQueue;
//}
//
//void BaseBuffer::createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size = size;
//    bufferInfo.usage = usage;
//    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create buffer!");
//    }
//
//    VkMemoryRequirements memRequirements;
//    vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//    allocInfo.allocationSize = memRequirements.size;
//    allocInfo.memoryTypeIndex = findMemoryType(m_PhysicalDevice, memRequirements.memoryTypeBits, properties);
//
//    if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate buffer memory!");
//    }
//
//    vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
//}
//
//void BaseBuffer::copyFrom(const VkBuffer& buffer, const VkBuffer& srcBuffer, const VkDeviceSize& size) {
//    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
//
//    VkBufferCopy copyRegion{};
//    copyRegion.srcOffset = 0;
//    copyRegion.dstOffset = 0;
//    copyRegion.size = size;
//    vkCmdCopyBuffer(commandBuffer, srcBuffer, buffer, 1, &copyRegion);
//
//    endSingleTimeCommands(commandBuffer);
//}
//
//uint32_t BaseBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
//
//VkCommandBuffer BaseBuffer::beginSingleTimeCommands() {
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = m_CommandPool.getCommandPool();
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//    return commandBuffer;
//}
//
//void BaseBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &commandBuffer;
//
//    vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//    vkQueueWaitIdle(m_GraphicsQueue);
//
//    vkFreeCommandBuffers(m_Device, m_CommandPool.getCommandPool(), 1, &commandBuffer);
//}