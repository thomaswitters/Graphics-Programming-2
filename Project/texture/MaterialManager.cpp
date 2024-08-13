#include "MaterialManager.h"

void MaterialManager::createMaterialPool(const VkDevice& device, int maxMaterialCount, int maxTexturesPerMaterial)
{
    if (m_DescriptorPool != VK_NULL_HANDLE && m_DescriptorSetLayout != VK_NULL_HANDLE) {
        return; // Pool and layout already created
    }

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(maxTexturesPerMaterial); // Reserve memory upfront
    for (size_t textureIndex = 0; textureIndex < maxTexturesPerMaterial; ++textureIndex) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = static_cast<uint32_t>(textureIndex);
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = static_cast<uint32_t>(maxTexturesPerMaterial * maxMaterialCount);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = static_cast<uint32_t>(maxMaterialCount);
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

std::shared_ptr<Material> MaterialManager::createMaterial(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures) {
    auto material = std::make_shared<Material>(device, textures, m_DescriptorSetLayout, m_DescriptorPool);
    m_Materials.push_back(material);
    return material;
}

void MaterialManager::cleanup(const VkDevice& device) {
    for (const auto& material : m_Materials) {
        if (material) {
            material->cleanup(device);
        }
    }
    m_Materials.clear();

    if (m_DescriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
        m_DescriptorPool = VK_NULL_HANDLE;
    }

    if (m_DescriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
        m_DescriptorSetLayout = VK_NULL_HANDLE;
    }
}