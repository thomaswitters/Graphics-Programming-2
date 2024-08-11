#include "GraphicsPipeline.h"


void GraphicsPipeline::initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, SwapChain swapChain, const VkRenderPass& renderPass, VkDeviceSize uboSize)
{
    m_MachineShader.initialize(device);
    m_RenderPass = renderPass;

    createDescriptorSetLayout(device);
    createUniformbuffers(device, physDevice, swapChain.getImageCount(), uboSize);
}

void GraphicsPipeline::cleanup(const VkDevice& device) {
    m_MachineShader.destroyShaderModules(device);
    vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

    for (auto& buffer : m_UniformBuffers)
    {
        buffer->destroy(device);
    }

    m_DescriptorPool->cleanup(device);
    vkDestroyDescriptorSetLayout(device, m_DescriptorSetlayout, nullptr);
}

void GraphicsPipeline::bind(VkCommandBuffer commandBuffer, SwapChain swapChain, int imageIndex)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain.getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain.getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain.getSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, m_DescriptorPool->getDescriptorSet(imageIndex), 0, nullptr);
}

void GraphicsPipeline::createDescriptorSetLayout(const VkDevice& device)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetlayout) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void GraphicsPipeline::createUniformbuffers(const VkDevice& device, const VkPhysicalDevice& physDevice, size_t maxFramesCount, VkDeviceSize uboBufferSize)
{
    void* data = nullptr;
    m_UniformBuffers.reserve(maxFramesCount);

    for (size_t i = 0; i < maxFramesCount; i++)
    {
        m_UniformBuffers.emplace_back(
            std::make_unique<DataBuffer>(physDevice, device,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uboBufferSize));

        m_UniformBuffers[i]->map(uboBufferSize);
    }
}

void GraphicsPipeline::updateUBO(int imageIndex, void* uboData, VkDeviceSize uboSize)
{
    m_UniformBuffers[imageIndex]->upload(uboSize, uboData);
}

void GraphicsPipeline::updatePushConstrant(VkCommandBuffer commandBuffer, void* pushConstrants, uint32_t pushConstrantSize)
{
    vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, pushConstrantSize, pushConstrants);
}

void GraphicsPipeline::updateMaterial(VkCommandBuffer commandBuffer, const Material& material)
{
    auto* descriptorSet = material.getDescriptorSet();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 1, 1, &descriptorSet, 0, nullptr);
}



