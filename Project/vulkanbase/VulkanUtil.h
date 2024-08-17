#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <fstream>
#include <optional>
#include <set>
#include <glm/ext/matrix_float4x4.hpp>

const uint32_t WIDTH = 1550;
const uint32_t HEIGHT = 1260;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isFullyDefined() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject2D {
    glm::mat4 proj;
};

struct UniformBufferObject3D {
    glm::mat4 viewProjection;
    glm::vec4 viewPosition;
};

struct UniformBufferObject3D_PBR {
    glm::mat4 viewProjection;
    glm::vec4 viewPosition;
    glm::vec3 lightDirection;
};

struct MeshPushConstantsPBR {
    glm::mat4 model;
    int renderMode;
};

struct MeshPushConstants {
    glm::mat4 model;
};

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

std::vector<char> readFile(const std::string& filename);

void createImage(const VkDevice& device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

VkImageView createImageView(const VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

bool checkDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR& surface);
