#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan_core.h>
#include "Texture.h"

class Material final {
public:
    Material(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
    ~Material() = default;

    void cleanup(const VkDevice& device);
    VkDescriptorSet getDescriptorSet() const { return m_DescriptorSet; }

private:
    std::vector<std::shared_ptr<Texture>> m_pTextures;
    VkDescriptorSet m_DescriptorSet{};
    VkDescriptorSetLayout m_DescriptorSetLayout{};
    VkDescriptorPool m_DescriptorPool{};
};
