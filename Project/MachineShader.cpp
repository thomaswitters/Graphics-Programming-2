#include "MachineShader.h"
#include <stdexcept>
#include "vulkanbase/VulkanUtil.h"

MachineShader::MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
    : m_VertexShaderFile{ vertexShaderFile }, m_FragmentShaderFile{ fragmentShaderFile }
{
}

void MachineShader::initialize(const VkDevice& device)
{
    m_ShaderStages.clear();
    m_ShaderStages.emplace_back(createVertexShaderInfo(device));
    m_ShaderStages.emplace_back(createFragmentShaderInfo(device));
}

std::vector<VkPipelineShaderStageCreateInfo>& MachineShader::getShaderStages()
{
    return m_ShaderStages;
}

void MachineShader::destroyShaderModules(const VkDevice& vkDevice)
{
    for (auto& stageInfo : m_ShaderStages)
    {
        vkDestroyShaderModule(vkDevice, stageInfo.module, nullptr);
    }
    m_ShaderStages.clear();
}

VkPipelineShaderStageCreateInfo MachineShader::createVertexShaderInfo(const VkDevice& device)
{
    std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = m_VSEntryPoint.c_str();
    return vertShaderStageInfo;
}

VkPipelineShaderStageCreateInfo MachineShader::createFragmentShaderInfo(const VkDevice& device)
{
    std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = m_FSEntryPoint.c_str();
    return fragShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo MachineShader::createVertexInputStateInfo()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo MachineShader::createInputAssemblyStateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

VkShaderModule MachineShader::createShaderModule(const VkDevice& device, const std::vector<char>& code)
{
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

