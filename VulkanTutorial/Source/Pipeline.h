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

		// [COMMENT] Sets up a pipeline config with default values
		static void DefaultConfigInfo(PipelineConfig& configInfo);
		// [COMMENT] Sets up the graphics pipeline
		void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		// [COMMENT] Binds the pipeline to the command buffer
		void BindCommandBuffer(VkCommandBuffer commandBuffer);

	private:
		// [COMMENT] Read all the bytes of a file and return them in a vector
		static std::vector<char> ReadFile(const std::string& path);

		// [COMMENT] The device we are using
		Device& device;
		// [COMMENT] The graphics pipeline object we set up
		VkPipeline graphicsPipeline;
		// [COMMENT] Shader module containing the vertex shader
		VkShaderModule vertShaderModule;
		// [COMMENT] Shader module containing the fragment shader
		VkShaderModule fragShaderModule;

		// [COMMENT] Max usable samples (used in DefaultConfigInfo
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	};
}