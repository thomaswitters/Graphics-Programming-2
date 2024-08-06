#include "CommandBuffer.h"

void CommandBuffer::reset() const
{
    if (m_CommandBuffer == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Command buffer is not initialized!");
    }
    if (vkResetCommandBuffer(m_CommandBuffer, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command buffer!");
    }
}

void CommandBuffer::beginRecording(VkCommandBufferUsageFlags flags) const
{
    if (m_CommandBuffer == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Command buffer is not initialized!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
}

void CommandBuffer::endRecording() const
{
    if (m_CommandBuffer == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Command buffer is not initialized!");
    }

    if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

 /*void CommandBuffer::drawFrame(uint32_t imageIndex) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

    drawScene();
    vkCmdEndRenderPass(m_CommandBuffer);
}*/

void CommandBuffer::submit(VkSubmitInfo& info) const
{

    if (m_CommandBuffer == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Command buffer is not initialized!");
    }

    info.commandBufferCount = 1;
    info.pCommandBuffers = &m_CommandBuffer;
}

void CommandBuffer::setVkCommandBuffer(VkCommandBuffer buffer)
{
    m_CommandBuffer = buffer;
}

VkCommandBuffer CommandBuffer::getVkCommandBuffer() const
{
    return m_CommandBuffer;
}