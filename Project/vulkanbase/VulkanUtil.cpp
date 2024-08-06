#include "VulkanUtil.h"
#include "CommandPool.h"

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

VkImageView CreateImageView(const VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	/*viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;*/
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

//VkCommandBuffer BeginSingleTimeCommands(const VkDevice& device, const CommandPool& commandPool)
//{
//	VkCommandBufferAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = commandPool.getCommandPool();
//	allocInfo.commandBufferCount = 1;
//
//	VkCommandBuffer commandBuffer;
//	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//	VkCommandBufferBeginInfo beginInfo{};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//	vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//	return commandBuffer;
//}

//void EndSingleTimeCommands(const VkDevice& device, VkCommandBuffer commandBuffer, const CommandPool& commandPool, const VkQueue& queue)
//{
//	vkEndCommandBuffer(commandBuffer);
//
//	VkSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &commandBuffer;
//
//	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(queue);
//
//	vkFreeCommandBuffers(device, commandPool.getCommandPool(), 1, &commandBuffer);
//}

//void TransitionImageLayout(const VkDevice& device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, const CommandPool& commandPool, const VkQueue& queue)
//{
//	{
//		VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);
//
//		VkImageMemoryBarrier barrier{};
//		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		barrier.oldLayout = oldLayout;
//		barrier.newLayout = newLayout;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.image = image;
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		barrier.subresourceRange.baseMipLevel = 0;
//		barrier.subresourceRange.levelCount = 1;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//
//		VkPipelineStageFlags sourceStage;
//		VkPipelineStageFlags destinationStage;
//
//		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		}
//		else {
//			throw std::invalid_argument("unsupported layout transition!");
//		}
//
//		vkCmdPipelineBarrier(
//			commandBuffer,
//			sourceStage, destinationStage,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &barrier
//		);
//
//
//		EndSingleTimeCommands(device, commandBuffer, commandPool, queue);
//	}
//
//}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
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

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR& surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isFullyDefined()) {
			break;
		}

		i++;
	}

	return indices;
}
