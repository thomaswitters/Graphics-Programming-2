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
#include <DataBuffer.h>
#include <VulkanDeviceManager.h>
#include <SwapChain.h>
#include <GraphicsPipeline.h>
#include <Scene.h>
#include <Camera.h>
#include "texture/MaterialManager.h"

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

		createRenderPass();

		m_commandPool.initialize(device, findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface));
		m_commandBuffer = m_commandPool.createCommandBuffer(device);

		materialManager.createMaterialPool(device, 4, 4);
		// week 03
		//2D scene
		m_graphicsPipeline2D.initialize(device, m_DeviceManager.getPhysicalDevice(), m_swapChain, m_renderPass, sizeof(UniformBufferObject2D));
		myScene2D.create2DScene(device, m_DeviceManager.getPhysicalDevice(), m_commandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface), m_DeviceManager.getGraphicsQueue());
		m_graphicsPipeline2D.createGraphicsPipeline<Vertex2D>(device, m_swapChain, sizeof(MeshPushConstants));


		//3D Scene
		m_graphicsPipeline3D.initialize(device, m_DeviceManager.getPhysicalDevice(), m_swapChain, m_renderPass, sizeof(UniformBufferObject3D));
		myScene3D.create3DScene(device, m_DeviceManager.getPhysicalDevice(), m_commandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface), m_DeviceManager.getGraphicsQueue());
		m_graphicsPipeline3D.createGraphicsPipeline<Vertex3D>(device, m_swapChain, sizeof(MeshPushConstants));

		
		m_graphicsPipeline3D_PBR.initialize(device, m_DeviceManager.getPhysicalDevice(), m_swapChain, m_renderPass, sizeof(UniformBufferObject3D));
		myScene3D_PBR.create3DScene_PBR(device, m_DeviceManager.getPhysicalDevice(), m_commandPool.getCommandPool(), findQueueFamilies(m_DeviceManager.getPhysicalDevice(), surface), m_DeviceManager.getGraphicsQueue(), materialManager);
		m_graphicsPipeline3D_PBR.createGraphicsPipeline<Vertex3D_PBR>(device, m_swapChain, sizeof(MeshPushConstants), materialManager.getMaterialSetLayout());
		
		

		createFrameBuffers();
		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			m_camera.update(window);
			myScene3D_PBR.update3D_PBR(m_camera.elapsedSec);

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
		m_swapChain.cleanup(device);

		myScene2D.cleanUp(device);
		m_graphicsPipeline2D.cleanup(device);

		myScene3D.cleanUp(device);
		m_graphicsPipeline3D.cleanup(device);

		myScene3D_PBR.cleanUp(device);
		m_graphicsPipeline3D_PBR.cleanup(device);
		
		materialManager.cleanup(device);
		
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		
		}
		for (auto framebuffer : m_swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyRenderPass(device, m_renderPass, nullptr);

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

	void createFrameBuffers();

	void createRenderPass();
	void beginRenderPass(const VkExtent2D& swapChainExtent, uint32_t imageIndex);
	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.
	GLFWwindow* window;
	// important to initialize before creating the graphics pipeline
	void initWindow();

	MaterialManager materialManager;

	VkRenderPass m_renderPass;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	CommandPool m_commandPool;
	CommandBuffer m_commandBuffer;

	SwapChain m_swapChain;
	GraphicsPipeline m_graphicsPipeline2D{ "shaders/shader.vert.spv", "shaders/shader.frag.spv"};
	GraphicsPipeline m_graphicsPipeline3D{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv"};
	GraphicsPipeline m_graphicsPipeline3D_PBR{ "shaders/shader3D_PBR.vert.spv", "shaders/shader3D_PBR.frag.spv"};

	VulkanDeviceManager m_DeviceManager;
	Scene<Vertex2D> myScene2D;
	Scene<Vertex3D> myScene3D;
	Scene<Vertex3D_PBR> myScene3D_PBR;

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

	Camera m_camera{glm::vec3(-2.f, 5.f, -30.f), 45.f, WIDTH, HEIGHT };

	float lastX = 400, lastY = 300;
	bool firstMouse = true;
	void MouseMove(GLFWwindow* window, double xpos, double ypos);
	void MouseEvent(GLFWwindow* window, int button, int action, int mods);
};