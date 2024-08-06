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

    const VkPhysicalDevice& getPhysicalDevice() const {
         return m_PhysicalDevice;
    }

    const VkQueue& getGraphicsQueue() const {
        return m_GraphicsQueue;
    }

    const VkQueue& getPresentQueue() const {
        return m_PresentQueue;
    }
private:
    void pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface);
    void createLogicalDevice(VkDevice& device, const VkSurfaceKHR& surface);
    bool isDeviceSuitable(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
};
