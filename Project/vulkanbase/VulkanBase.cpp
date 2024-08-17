#include "VulkanBase.h"

void VulkanBase::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void VulkanBase::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xpos, double ypos)
        {
            void* pUser = glfwGetWindowUserPointer(window);
            VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
            vBase->MouseMove(window, xpos, ypos);
        });
    glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            void* pUser = glfwGetWindowUserPointer(window);
            VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
            vBase->KeyEvent(window, key, scancode, action, mods);
        });
}

void VulkanBase::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();

    m_DeviceManager.initialize(m_Device, m_Instance, m_Surface);

    m_SwapChain.initialize(m_Device, m_Surface, m_pWindow, m_DeviceManager);

    createRenderPass();

    m_CommandPool.initialize(m_Device, findQueueFamilies(m_DeviceManager.getPhysicalDevice(), m_Surface));
    m_CommandBuffer = m_CommandPool.createCommandBuffer(m_Device);

    m_MaterialManager.createMaterialPool(m_Device, 4, 4);

    m_GraphicsPipeline2D.initialize(m_Device, m_DeviceManager.getPhysicalDevice(), m_SwapChain, m_RenderPass, sizeof(UniformBufferObject2D));
    m_MyScene2D.createScene(m_Device, m_DeviceManager.getPhysicalDevice(), m_CommandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), m_Surface), m_DeviceManager.getGraphicsQueue(), m_MaterialManager);
    m_GraphicsPipeline2D.createGraphicsPipeline<Vertex2D>(m_Device, m_SwapChain, sizeof(PushConstants));

    m_GraphicsPipeline3D.initialize(m_Device, m_DeviceManager.getPhysicalDevice(), m_SwapChain, m_RenderPass, sizeof(UniformBufferObject3D));
    m_MyScene3D.createScene(m_Device, m_DeviceManager.getPhysicalDevice(), m_CommandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), m_Surface), m_DeviceManager.getGraphicsQueue(), m_MaterialManager);
    m_GraphicsPipeline3D.createGraphicsPipeline<Vertex3D>(m_Device, m_SwapChain, sizeof(PushConstants));

    m_GraphicsPipeline3D_PBR.initialize(m_Device, m_DeviceManager.getPhysicalDevice(), m_SwapChain, m_RenderPass, sizeof(UniformBufferObject3D_PBR));
    m_MyScene3D_PBR.createScene(m_Device, m_DeviceManager.getPhysicalDevice(), m_CommandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), m_Surface), m_DeviceManager.getGraphicsQueue(), m_MaterialManager);
    m_GraphicsPipeline3D_PBR.createGraphicsPipeline<Vertex3D_PBR>(m_Device, m_SwapChain, sizeof(PushConstantsPBR), m_MaterialManager.getMaterialSetLayout());

    createFrameBuffers();
    createSyncObjects();
}

void VulkanBase::mainLoop() {
    while (!glfwWindowShouldClose(m_pWindow)) {
        glfwPollEvents();
        m_Camera.update(m_pWindow);
        m_MyScene3D_PBR.update(m_Camera.getElapsedSec());
        drawFrame();
    }
    vkDeviceWaitIdle(m_Device);
}

void VulkanBase::cleanup() {
    vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
    vkDestroyFence(m_Device, m_InFlightFence, nullptr);

    m_CommandPool.cleanup(m_Device);
    m_SwapChain.cleanup(m_Device);

    m_MyScene2D.cleanUp(m_Device);
    m_GraphicsPipeline2D.cleanup(m_Device);

    m_MyScene3D.cleanUp(m_Device);
    m_GraphicsPipeline3D.cleanup(m_Device);

    m_MyScene3D_PBR.cleanUp(m_Device);
    m_GraphicsPipeline3D_PBR.cleanup(m_Device);

    m_MaterialManager.cleanup(m_Device);

    if (enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }

    for (auto framebuffer : m_SwapChainFramebuffers) {
        vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);

    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void VulkanBase::createSurface() {
    if (glfwCreateWindowSurface(m_Instance, m_pWindow, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void VulkanBase::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanBase::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (createDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

std::vector<const char*> VulkanBase::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanBase::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VulkanBase::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

bool VulkanBase::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanBase::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void VulkanBase::drawFrame() {
    vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_Device, 1, &m_InFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_Device, m_SwapChain.getSwapChain(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetFences(m_Device, 1, &m_InFlightFence);
    m_CommandBuffer.reset();
    m_CommandBuffer.beginRecording();

    beginRenderPass(m_SwapChain.getSwapChainExtent(), imageIndex);

    m_MyScene2D.draw(m_Camera, m_CommandBuffer, m_GraphicsPipeline2D, m_SwapChain, imageIndex);
    m_MyScene3D.draw(m_Camera, m_CommandBuffer, m_GraphicsPipeline3D, m_SwapChain, imageIndex);
    m_MyScene3D_PBR.draw(m_Camera, m_CommandBuffer, m_GraphicsPipeline3D_PBR, m_SwapChain, imageIndex, static_cast<int>(renderMode));

    vkCmdEndRenderPass(m_CommandBuffer.getVkCommandBuffer());
    m_CommandBuffer.endRecording();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    m_CommandBuffer.submit(submitInfo);

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer tempBuffer{ m_CommandBuffer.getVkCommandBuffer() };
    submitInfo.pCommandBuffers = &tempBuffer;

    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_DeviceManager.getGraphicsQueue(), 1, &submitInfo, m_InFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_SwapChain.getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_DeviceManager.getPresentQueue(), &presentInfo);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBase::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


void VulkanBase::createFrameBuffers() {
    m_SwapChainFramebuffers.resize(m_SwapChain.getSwapChainImageViews().size());
    for (size_t i = 0; i < m_SwapChain.getSwapChainImageViews().size(); i++) {
        VkImageView attachments[] = {
            m_SwapChain.getSwapChainImageViews()[i],
            m_SwapChain.getDepthImageView()
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_SwapChain.getSwapChainExtent().width;
        framebufferInfo.height = m_SwapChain.getSwapChainExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanBase::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChain.getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(m_DeviceManager.getPhysicalDevice());
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanBase::beginRenderPass(const VkExtent2D& swapChainExtent, uint32_t imageIndex)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    VkClearValue clearDepth = { {1.0f, 0} };
    renderPassInfo.clearValueCount = 2;
    VkClearValue clearValues[] = { clearColor, clearDepth };
    renderPassInfo.pClearValues = clearValues;


    vkCmdBeginRenderPass(m_CommandBuffer.getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}


void VulkanBase::MouseMove(GLFWwindow* window, double xpos, double ypos)
{
    if (m_FirstMouse)
    {
        m_LastX = float(xpos);
        m_LastY = float(ypos);
        m_FirstMouse = false;
    }

    m_Camera.onMouseMove(window, xpos, ypos, m_LastX, m_LastY);
}

void VulkanBase::KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        switch (renderMode) {
        case RenderModes::ONLY_ALBEDO:
            renderMode = RenderModes::ONLY_NORMAL;
            std::cout << "Render Mode changed to: Only Normals" << std::endl;
            break;
        case RenderModes::ONLY_NORMAL:
            renderMode = RenderModes::ONLY_SPECULAR;
            std::cout << "Render Mode changed to: Only Specular" << std::endl;
            break;
        case RenderModes::ONLY_SPECULAR:
            renderMode = RenderModes::COMBINED;
            std::cout << "Render Mode changed to: Combined" << std::endl;
            break;
        case RenderModes::COMBINED:
            renderMode = RenderModes::ONLY_ALBEDO;
            std::cout << "Render Mode changed to: Only Albedo" << std::endl;
            break;
        }
        std::cout << "Render Mode changed to: " << static_cast<int>(renderMode) << std::endl;
    }
}