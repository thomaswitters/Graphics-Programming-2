#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <glm/ext/vector_int2.hpp>
#include <CommandBuffer.h>

class Texture {
public:
    Texture(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, const std::string& texturePath);

    void cleanup(const VkDevice& device);

    void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createTextureSampler(const VkDevice& device, const VkPhysicalDevice& physDevice, VkSamplerAddressMode addressMode);

    const VkDescriptorImageInfo& getDescriptorInfo() const { return m_DescriptorImageInfo; }

    VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool);
    void endSingleTimeCommands(const VkDevice& device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue, VkCommandPool commandPool);
private:
    void createTextureImage(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, const std::string& texturePath);

    VkDescriptorImageInfo m_DescriptorImageInfo{};

    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;

    VkCommandBuffer m_TempCommandBuffer;
};
