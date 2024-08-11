#include "Material.h"

#include <vulkan/vk_enum_string_helper.h>

#include <stdexcept>
#include <iostream>

Material::Material(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool) :
    m_Texture(textures), m_DescriptorSetLayout(descriptorSetLayout), m_DescriptorPool(descriptorPool)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(m_Texture.size());

    for (size_t textureIndex = 0; textureIndex < m_Texture.size(); ++textureIndex) {
        const VkDescriptorImageInfo& descriptorInfo = m_Texture[textureIndex]->getDescriptorInfo();

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = m_DescriptorSet;
        writeDescriptorSet.dstBinding = static_cast<uint32_t>(textureIndex);
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.pImageInfo = &descriptorInfo;

        descriptorWrites.push_back(writeDescriptorSet);
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Material::cleanup(const VkDevice& device)
{
    for (const auto& texture : m_Texture)
    {
        if (texture != nullptr)
        {
            texture->cleanup(device);
        }
    }
}