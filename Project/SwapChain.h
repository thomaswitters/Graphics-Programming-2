#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include "VulkanDeviceManager.h"
#include <limits>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "vulkanbase/VulkanUtil.h"

class SwapChain {
public:
    SwapChain() = default;
    ~SwapChain() = default;

    void initialize(const VkDevice& device, const VkSurfaceKHR& surface, GLFWwindow* window, const VulkanDeviceManager& deviceManager);
    void cleanup(const VkDevice& device);

    VkSwapchainKHR getSwapChain() const { return m_SwapChain; }
    const std::vector<VkImage>& getSwapChainImages() const { return m_SwapChainImages; }
    const std::vector<VkImageView>& getSwapChainImageViews() const { return m_SwapChainImageViews; }
    VkImageView getDepthImageView() const { return m_DepthImageView; }
    VkFormat getSwapChainImageFormat() const { return m_SwapChainImageFormat; }
    VkExtent2D getSwapChainExtent() const { return m_SwapChainExtent; }
    size_t getImageCount() const { return m_SwapChainImages.size(); }

private:
    void createSwapChain(const VkDevice& device, const VkSurfaceKHR& surface, GLFWwindow* window, const VulkanDeviceManager& deviceManager);
    void createImageViews(const VkDevice& device);
    void createDepthResources(const VkDevice& device, const VulkanDeviceManager& deviceManager);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, const VkSurfaceKHR& surface);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkImageView m_DepthImageView;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;

    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
};
