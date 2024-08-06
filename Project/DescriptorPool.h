#pragma once
#include "vulkan/vulkan_core.h"
#include <initializer_list>
#include <vector>
#include <stdexcept>

class DescriptorPool
{
public:
    DescriptorPool() = default;

    DescriptorPool(VkDevice device, VkDeviceSize size, size_t count);
    ~DescriptorPool();
    void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout,
        std::initializer_list<VkBuffer> buffers);

    void bindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index);

    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return m_DescriptorSets; }
private:
    VkDevice m_Device;
    VkDeviceSize m_Size;
    VkDescriptorPool m_DescriptorPool;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    size_t m_Count;
};

