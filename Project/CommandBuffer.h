#pragma once
#include "vulkan/vulkan_core.h"
#include <stdexcept>

class CommandBuffer
{
public:
	CommandBuffer() = default;
	~CommandBuffer() = default;

	void reset() const;
	void beginRecording(VkCommandBufferUsageFlags flags = 0U) const;
	void endRecording() const;

	//void drawFrame(uint32_t imageIndex);

	void submit(VkSubmitInfo& info)const;

	void setVkCommandBuffer(VkCommandBuffer buffer);
	VkCommandBuffer getVkCommandBuffer() const;
private:
	VkCommandBuffer m_CommandBuffer;
};