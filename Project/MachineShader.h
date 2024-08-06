#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <string>
#include <stdexcept>
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"

class MachineShader
{
public:
	MachineShader(const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	) : m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile }
	{

	}

	// shader stages and shader modules creeren met vulkan
	void initialize(const VkDevice& device){
		m_ShaderStages.clear();
		m_ShaderStages.emplace_back(createVertexShaderInfo(device));
		m_ShaderStages.emplace_back(createFragmentShaderInfo(device));
	}

	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() {
		return m_ShaderStages;
	}

	void DestroyShaderModules(const VkDevice& vkDevice)
	{
		for (auto& stageInfo : m_ShaderStages)
		{
			vkDestroyShaderModule(vkDevice, stageInfo.module, nullptr);
		}
		m_ShaderStages.clear();
	}

	//"shaders/shader.frag.spv"
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& device) {
		std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
		VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = m_FSEntryPoint.c_str();// Hardcoded

		return fragShaderStageInfo;
	}

	//"shaders/shader.vert.spv"
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& device) {
		std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
		VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = m_VSEntryPoint.c_str();
		return vertShaderStageInfo;
	}

	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo()
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		return vertexInputInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		return inputAssembly;
	}

	VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	~MachineShader() = default;

private:
	std::string m_VertexShaderFile;
	std::string m_VSEntryPoint{"main"};
	std::string m_FragmentShaderFile;
	std::string m_FSEntryPoint{"main"};

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};
