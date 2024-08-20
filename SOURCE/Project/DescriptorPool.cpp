#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(const VkDevice& device, size_t frameCount, const std::vector<VkDescriptorType>& types,
    VkDescriptorSetLayout descriptorSetLayout,
    const std::vector<std::unique_ptr<DataBuffer>>& buffers) :
    m_Device(device)
{
    createPool(frameCount, types);
    createSets(frameCount, types, descriptorSetLayout, buffers);
}

void DescriptorPool::cleanup(const VkDevice& device)
{
    vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

void DescriptorPool::createPool(size_t frameCount, const std::vector<VkDescriptorType>& types)
{
    std::vector<VkDescriptorPoolSize> poolSizes{};
    poolSizes.reserve(types.size());

    for (const auto& type : types)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = type; 
        poolSize.descriptorCount = static_cast<uint32_t>(frameCount);

        poolSizes.emplace_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;
    poolInfo.maxSets = static_cast<uint32_t>(frameCount);
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void DescriptorPool::createSets(size_t frameCount, const std::vector<VkDescriptorType>& types,
    VkDescriptorSetLayout descriptorSetLayout,
    const std::vector<std::unique_ptr<DataBuffer>>& buffers)
{
    std::vector<VkDescriptorSetLayout> layouts(frameCount, descriptorSetLayout);

    VkDescriptorSetAllocateInfo descriptorAllocateInfo{};
    descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocateInfo.descriptorPool = m_DescriptorPool;
    descriptorAllocateInfo.descriptorSetCount = static_cast<uint32_t>(frameCount);
    descriptorAllocateInfo.pSetLayouts = layouts.data();


    m_DescriptorSets.resize(frameCount);
    if (vkAllocateDescriptorSets(m_Device, &descriptorAllocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets! For General Descriptor Pool");

    for (size_t i = 0; i < frameCount; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers[i]->getVkBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;



        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


        std::vector<VkWriteDescriptorSet> descriptorWrites{};
        descriptorWrites.reserve(types.size());

        for (size_t typeIndex{}; typeIndex < types.size(); ++typeIndex)
        {
            VkWriteDescriptorSet descriptorSet{};
            descriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorSet.dstSet = m_DescriptorSets[i];
            descriptorSet.dstBinding = static_cast<uint32_t>(typeIndex);
            descriptorSet.dstArrayElement = 0;
            descriptorSet.descriptorCount = 1;
            descriptorSet.descriptorType = types[typeIndex];
            descriptorSet.pImageInfo = nullptr;
            descriptorSet.pBufferInfo = nullptr;
            descriptorSet.pTexelBufferView = nullptr;

            if (types[typeIndex] == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                descriptorSet.pImageInfo = &imageInfo;
            else if (types[typeIndex] == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                descriptorSet.pBufferInfo = &bufferInfo;

            descriptorWrites.emplace_back(descriptorSet);
        }

        vkUpdateDescriptorSets(
            m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    }
}