#pragma once
#include "vulkan/vulkan_core.h"
#include <initializer_list>
#include <stdexcept>
#include <vector>
#include <memory>
#include <VulkanBufferHandler.h>

class DescriptorPool
{
public:
    DescriptorPool(const VkDevice& device, size_t frameCount, const std::vector<VkDescriptorType>& types, VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::unique_ptr<DataBuffer>>& buffers);

    void cleanup(const VkDevice& device);

    VkDescriptorSet* getDescriptorSet(int index) { return &m_DescriptorSets[index]; }

    operator VkDescriptorPool() const { return m_DescriptorPool; }


private:
    void createPool(size_t frameCount, const std::vector<VkDescriptorType>& types);
    void createSets(size_t frameCount, const std::vector<VkDescriptorType>& types,
        VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::unique_ptr<DataBuffer>>& buffers);

    VkDevice m_Device{};
    VkDescriptorPool m_DescriptorPool{};
    std::vector<VkDescriptorSet> m_DescriptorSets{};
};

