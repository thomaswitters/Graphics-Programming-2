#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <glm/ext/vector_int2.hpp>
#include <buffers/CommandBuffer.h>

class Texture {
public:
    Texture(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, const std::string& texturePath);
    ~Texture();

    void cleanup();

    const VkDescriptorImageInfo& getDescriptorInfo() const { return m_DescriptorImageInfo; }

private:
    void createTextureImage(const VkDevice& device, const VkPhysicalDevice& physDevice, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, const std::string& texturePath);
    void createTextureSampler(const VkDevice& device, const VkPhysicalDevice& physDevice, VkSamplerAddressMode addressMode);

    void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool);
    void endSingleTimeCommands(const VkDevice& device, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);

    VkDescriptorImageInfo m_DescriptorImageInfo{};
    VkImage m_TextureImage{ VK_NULL_HANDLE };
    VkDeviceMemory m_TextureImageMemory{ VK_NULL_HANDLE };

    VkDevice m_Device;
    VkCommandPool m_CommandPool;
};
