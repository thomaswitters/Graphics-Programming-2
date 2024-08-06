#pragma once
#include "vulkan/vulkan_core.h"
#include <string>
#include "MachineShader.h"
#include "SwapChain.h"
#include "CommandBuffer.h"

class GraphicsPipeline {
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	void initialize(const VkDevice& device, const SwapChain& swapChain);
	void cleanup(const VkDevice& device);

	const std::vector<VkFramebuffer>& getSwapChainFramebuffers() const {return m_SwapChainFramebuffers;}
	VkPipelineLayout getPipelineLayout() const {return m_PipelineLayout;}
	VkPipeline getGraphicsPipeline() const {return m_GraphicsPipeline;}
	VkRenderPass getRenderPass() const { return m_RenderPass;}
private:
	void createRenderPass(const VkDevice& device, const SwapChain& swapChain);
	void createGraphicsPipeline(const VkDevice& device);
	void createFrameBuffers(const VkDevice& device, const SwapChain& swapChain);

	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkRenderPass m_RenderPass;
	MachineShader m_MachineShader{"shaders/shader.vert.spv", "shaders/shader.frag.spv"};

};
