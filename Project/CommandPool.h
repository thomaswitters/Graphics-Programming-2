#pragma once
#include "vulkan/vulkan_core.h"
#include "buffers/CommandBuffer.h"
#include "vulkanbase/VulkanUtil.h"

class CommandPool
{
public:
	CommandPool() = default;
	~CommandPool() = default;

	void initialize(const VkDevice& device, const QueueFamilyIndices& queue);
	void cleanup(const VkDevice& device);

	CommandBuffer createCommandBuffer(const VkDevice& device) const;

	const VkCommandPool& getCommandPool() const { return m_CommandPool; }
private:
	VkCommandPool m_CommandPool;
};
