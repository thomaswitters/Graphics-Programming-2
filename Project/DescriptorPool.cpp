#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(VkDevice device, VkDeviceSize size, size_t count)
    : m_Device(device), m_Size(size), m_Count(count), m_DescriptorPool(VK_NULL_HANDLE){

    // Create the descriptor pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // Example type, modify as needed
    poolSize.descriptorCount = static_cast<uint32_t>(count);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1; // We can add more types if needed
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(count);

    if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

DescriptorPool::~DescriptorPool() {
    if (m_DescriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
    }
}

void DescriptorPool::createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
    std::initializer_list<VkBuffer> buffers) {

    // Create the descriptor set info
    std::vector<VkDescriptorSetLayout> layouts(buffers.size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(buffers.size());

    if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // Update the descriptor sets
    for (size_t i = 0; i < buffers.size(); ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers.begin()[i];
        bufferInfo.offset = 0;
        bufferInfo.range = m_Size;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[i];
        descriptorWrite.dstBinding = 0; // Change based on your binding
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // Example type, modify as needed
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
}

void DescriptorPool::bindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index) {
    if (index >= m_DescriptorSets.size()) {
        throw std::out_of_range("Descriptor set index is out of range!");
    }

    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_DescriptorSets[index], 0, nullptr);
}