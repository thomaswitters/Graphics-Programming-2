#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <MachineShader.h>
#include <Vertex.h>
#include <buffers/CommandBuffer.h>
#include <CommandPool.h>
#include <buffers/DataBuffer.h>
#include <VulkanDeviceManager.h>
#include <SwapChain.h>
#include <GraphicsPipeline.h>
#include <Scene.h>
#include <Camera.h>
#include "texture/MaterialManager.h"

class VulkanBase {
public:
    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void createSurface();
    void createFrameBuffers();
    void createRenderPass();
    void beginRenderPass(const VkExtent2D& swapChainExtent, uint32_t imageIndex);
    void createInstance();
    void setupDebugMessenger();
    void createSyncObjects();
    void drawFrame();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    std::vector<const char*> getRequiredExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    bool checkValidationLayerSupport();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    GLFWwindow* m_pWindow;
    MaterialManager m_MaterialManager;

    VkRenderPass m_RenderPass = VK_NULL_HANDLE;;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;

    CommandPool m_CommandPool;
    CommandBuffer m_CommandBuffer;

    SwapChain m_SwapChain;
    GraphicsPipeline m_GraphicsPipeline2D{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
    GraphicsPipeline m_GraphicsPipeline3D{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv" };
    GraphicsPipeline m_GraphicsPipeline3D_PBR{ "shaders/shader3D_PBR.vert.spv", "shaders/shader3D_PBR.frag.spv" };

    VulkanDeviceManager m_DeviceManager;
    Scene<Vertex2D> m_MyScene2D;
    Scene<Vertex3D> m_MyScene3D;
    Scene<Vertex3D_PBR> m_MyScene3D_PBR;

    VkInstance m_Instance = VK_NULL_HANDLE;;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;;

    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;;
    VkFence m_InFlightFence = VK_NULL_HANDLE;;

    Camera m_Camera{ glm::vec3(-2.f, 15.f, -60.f), 45.f, WIDTH, HEIGHT };

    float m_LastX = 400, m_LastY = 300;
    bool m_FirstMouse = true;

    void MouseMove(GLFWwindow* window, double xpos, double ypos);
    void MouseEvent(GLFWwindow* window, int button, int action, int mods);
};