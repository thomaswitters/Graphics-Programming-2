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
#include <CommandBuffer.h>
#include <CommandPool.h>
#include <VulkanBufferHandler.h>
#include <VulkanDeviceManager.h>
#include <SwapChain.h>
#include <GraphicsPipeline.h>
#include <Scene.h>

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:

	void initVulkan() {
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		m_DeviceManager.initialize(device, instance, surface);

		// week 04 
		m_swapChain.initialize(device, surface, window, m_DeviceManager);

		// week 03
		m_graphicsPipeline.initialize(device, m_swapChain);

		// week 02
		m_commandPool.initialize(device, findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface));
		m_commandBuffer = m_commandPool.createCommandBuffer(device);

		myScene.Create2DScene(device, m_DeviceManager.getPhysicalDevice(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface), m_DeviceManager.getGraphicsQueue());
		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_commandPool.cleanup(device);
		m_graphicsPipeline.cleanup(device);
		m_swapChain.cleanup(device);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		myScene.cleanUp(device);

		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	

	

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	
	void beginRenderPass(GraphicsPipeline graphicsPipeline, const VkExtent2D& swapChainExtent, uint32_t imageIndex);
	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.
	GLFWwindow* window;
	// important to initialize before creating the graphics pipeline
	void initWindow();
	void drawScene();

	VkRenderPass m_renderPass;
	CommandPool m_commandPool;
	CommandBuffer m_commandBuffer;

	GraphicsPipeline m_graphicsPipeline;
	SwapChain m_swapChain;
	VulkanDeviceManager m_DeviceManager;
	Scene<Vertex2D> myScene;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};