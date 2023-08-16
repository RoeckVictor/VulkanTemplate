#pragma once

#include <string>
#include <vector>

#include "Device.h"
#include "VulkanModel.h"

namespace MyFirstEngine
{
	struct PipelineConfig
	{
		PipelineConfig(const PipelineConfig&) = default;
		PipelineConfig& operator=(const PipelineConfig&) = default;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlending;
		VkPipelineDepthStencilStateCreateInfo depthStencil;
		std::vector<VkDynamicState> dynamicState;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		unsigned int subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline(Device &device, VkShaderModule vertShader, VkShaderModule fragShader, const PipelineConfig& config);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		static void DefaultConfigInfo(PipelineConfig& configInfo);
		void CreateGraphicsPipeline(VkShaderModule vertShader, VkShaderModule fragShader, const PipelineConfig& config);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void BindCommandBuffer(VkCommandBuffer commandBuffer);

		VkSampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; }

	private:
		Device& m_Device;
		VkPipeline m_GraphicsPipeline;

		VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	};
}