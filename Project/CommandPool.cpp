#include "CommandPool.h"

void CommandPool::initialize(const VkDevice& device, const QueueFamilyIndices& queue)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queue.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void CommandPool::cleanup(const VkDevice& device)
{
	vkDestroyCommandPool(device, m_CommandPool, nullptr);
}

CommandBuffer CommandPool::createCommandBuffer(const VkDevice& device) const
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;

	if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	CommandBuffer cmdBuffer;
	cmdBuffer.setVkCommandBuffer(commandBuffer);

	return cmdBuffer;
}