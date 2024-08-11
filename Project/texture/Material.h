#pragma once
#include <vector>
#include "vulkan/vulkan_core.h"
#include "Texture.h"
#include <memory>

class Material final
{
public:
    Material(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);

    void cleanup(const VkDevice& device);

    VkDescriptorSet getDescriptorSet() const { return m_DescriptorSet; }

private:
    std::vector<std::shared_ptr<Texture>> m_Texture;

    VkDescriptorSet m_DescriptorSet{};
    VkDescriptorSetLayout m_DescriptorSetLayout{};
    VkDescriptorPool m_DescriptorPool{};
};
