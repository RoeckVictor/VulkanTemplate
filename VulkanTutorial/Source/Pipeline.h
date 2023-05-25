#pragma once

#include <string>
#include <vector>

#include "Device.h"
#include "Model.h"

namespace VulkanTutorial
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
		Pipeline(Device &device, const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		static void DefaultConfigInfo(PipelineConfig& configInfo);
		void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		void BindCommandBuffer(VkCommandBuffer commandBuffer);

		VkSampleCountFlagBits GetMsaaSamples() const { return msaaSamples; }

	private:
		static std::vector<char> ReadFile(const std::string& path);

		Device& device;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	};
}