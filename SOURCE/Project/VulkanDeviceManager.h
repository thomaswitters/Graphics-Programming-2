#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include <stdexcept>
#include <optional>
#include <set>
#include "vulkanbase/VulkanUtil.h"

class VulkanDeviceManager {
public:
    VulkanDeviceManager() = default;
    ~VulkanDeviceManager() = default;

    void initialize(VkDevice& device, const VkInstance& instance, const VkSurfaceKHR& surface);

    const VkPhysicalDevice& getPhysicalDevice() const;

    const VkQueue& getGraphicsQueue() const;

    const VkQueue& getPresentQueue() const;
private:
    void pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface);
    void createLogicalDevice(VkDevice& device, const VkSurfaceKHR& surface);
    bool isDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
};
