#pragma once
#include "vulkan/vulkan_core.h"
#include <string>
#include "MachineShader.h"
#include "SwapChain.h"
#include "buffers/CommandBuffer.h"
#include "buffers/DataBuffer.h"
#include <chrono>                  
#include <cstring>                 
#include <glm/glm.hpp>            
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    
#include <glm/gtc/matrix_inverse.hpp> 
#include <DescriptorPool.h>
#include <texture/Material.h>

class GraphicsPipeline {
public:
    GraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
        : m_PipelineLayout(), m_GraphicsPipeline(), m_RenderPass(), m_MachineShader(vertexShaderFile, fragmentShaderFile)
    {}
    ~GraphicsPipeline() = default;

    void initialize(const VkDevice& device, const VkPhysicalDevice& physDevice, SwapChain swapChain, const VkRenderPass& renderPass, VkDeviceSize uboSize);
    template<typename VertexType>
    void createGraphicsPipeline(const VkDevice& device, SwapChain swapChain, uint32_t pushConstrantSize = 0, std::optional<VkDescriptorSetLayout> materialSetLayout = std::nullopt);

    void cleanup(const VkDevice& device);

    VkPipelineLayout getPipelineLayout() const { return m_PipelineLayout; }
    VkPipeline getGraphicsPipeline() const { return m_GraphicsPipeline; }

    void bind(VkCommandBuffer commandBuffer, SwapChain swapChain, int imageIndex);
    void updateUBO(int imageIndex, void* uboData, VkDeviceSize uboSize);
    void updatePushConstrant(VkCommandBuffer commandBuffer, void* pushConstrants, uint32_t pushConstrantSize = 0U);
    void updateMaterial(VkCommandBuffer commandBuffer, const Material& material);
private:
    void createDescriptorSetLayout(const VkDevice& device);
    void createUniformbuffers(const VkDevice& device, const VkPhysicalDevice& physDevice, size_t maxFramesCount, VkDeviceSize uboBufferSize);

    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;
    VkRenderPass m_RenderPass;
    MachineShader m_MachineShader;

    VkDescriptorSetLayout m_DescriptorSetlayout{};
    std::vector<std::unique_ptr<DataBuffer>> m_pUniformBuffers;
    std::unique_ptr<DescriptorPool> m_pDescriptorPool;
};

template<typename VertexType>
void GraphicsPipeline::createGraphicsPipeline(const VkDevice& device, SwapChain swapChain, uint32_t pushConstrantSize, std::optional<VkDescriptorSetLayout> materialSetLayout) {
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Setup multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Setup depth stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = pushConstrantSize;

    std::vector<VkDescriptorSetLayout> pipelineLayoutSets{ m_DescriptorSetlayout };
    if (materialSetLayout.has_value()) {
        pipelineLayoutSets.push_back(materialSetLayout.value());
    }

    // Pipeline Layout Creation
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(pipelineLayoutSets.size());
    pipelineLayoutInfo.pSetLayouts = pipelineLayoutSets.data();

    if (pushConstrantSize > 0) {
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    }
    else {
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
    }

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // Vertex Input State
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = VertexType::getBindingDescription();
    auto attributeDescription = VertexType::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

    // Pipeline Creation
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = m_MachineShader.getShaderStages().data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &m_MachineShader.createInputAssemblyStateInfo();
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    m_pDescriptorPool = std::make_unique<DescriptorPool>(device,
        swapChain.getImageCount(),
        std::vector{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        m_DescriptorSetlayout,
        m_pUniformBuffers);

    m_MachineShader.destroyShaderModules(device);
}
