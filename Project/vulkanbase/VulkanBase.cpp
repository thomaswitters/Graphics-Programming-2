#include "VulkanBase.h"

void VulkanBase::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void VulkanBase::initVulkan()
{
	auto queueFamilyIndices{ vkUtils::FindQueueFamilies(physicalDevice, m_Surface) };
	m_pCommandPool = std::make_unique<CommandPool>(device, queueFamilyIndices.graphicsFamily.value());
	m_pCommandBuffer = std::make_unique<CommandBuffer>(device, m_pCommandPool->GetVkCommandPool());
	
	// week 06
	createInstance();
	setupDebugMessenger();
	createSurface();

	// week 05
	pickPhysicalDevice();
	createLogicalDevice();

	// week 04 
	createSwapChain();
	createImageViews();

	// week 03
	m_GradientShader.Init(device);
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffers();
	// week 02
	createVertexBuffer();

	// week 06
	createSyncObjects();
}

void VulkanBase::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// week 06
		DrawFrame();
	}
	vkDeviceWaitIdle(device);
}

void VulkanBase::cleanup()
{
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	vkDestroyFence(device, inFlightFence, nullptr);

	vkDestroyCommandPool(device, m_pCommandPool->GetVkCommandPool(), nullptr);
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	if (enableValidationLayers) {
		vkUtils::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, m_Surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();

	//cleanupSwapChain();

	vkDestroyBuffer(device, vertexBuffer, nullptr);
}