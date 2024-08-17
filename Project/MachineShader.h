#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include "Vertex.h"

class MachineShader
{
public:
    MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
    ~MachineShader() = default;

    void initialize(const VkDevice& device);
    std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages();
    void destroyShaderModules(const VkDevice& vkDevice);

    VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
    VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();

private:
    VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& device);
    VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& device);
    VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code);

    std::string m_VertexShaderFile;
    std::string m_VSEntryPoint{ "main" };
    std::string m_FragmentShaderFile;
    std::string m_FSEntryPoint{ "main" };

    std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};
