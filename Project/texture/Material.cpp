#include "Material.h"
#include <stdexcept>

Material::Material(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
    : m_pTextures(textures), m_DescriptorSetLayout(descriptorSetLayout), m_DescriptorPool(descriptorPool) {

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(m_pTextures.size());

    for (size_t i = 0; i < m_pTextures.size(); ++i) {
        const VkDescriptorImageInfo& descriptorInfo = m_pTextures[i]->getDescriptorInfo();

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = m_DescriptorSet;
        writeDescriptorSet.dstBinding = static_cast<uint32_t>(i);
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.pImageInfo = &descriptorInfo;

        descriptorWrites.push_back(writeDescriptorSet);
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Material::cleanup(const VkDevice& device) {
    for (const auto& texture : m_pTextures) {
        if (texture) {
            texture->cleanup();
        }
    }
}