#pragma once
#include "vulkan/vulkan_core.h"
#include "Material.h"

class MaterialManager {
public:
    MaterialManager() = default;

    void createMaterialPool(const VkDevice& device, int maxMaterialCount, int maxTexturesPerMaterial);
    std::shared_ptr<Material> createMaterial(const VkDevice& device, const std::vector<std::shared_ptr<Texture>>& textures);

    VkDescriptorSetLayout getMaterialSetLayout() const { return m_DescriptorSetLayout; }

    void cleanup(const VkDevice& device);
private:
    VkDescriptorSetLayout m_DescriptorSetLayout{};
    VkDescriptorPool m_DescriptorPool{};

    std::vector<std::shared_ptr<Material>> m_Materials;
};
