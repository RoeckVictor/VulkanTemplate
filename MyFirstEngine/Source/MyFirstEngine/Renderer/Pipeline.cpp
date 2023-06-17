#include "Mfepch.h"
#include "Pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace MyFirstEngine
{
	Pipeline::Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config)
		: device(device)
	{
		CreateGraphicsPipeline(vertPath, fragPath, config);
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
		vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
	}

	void Pipeline::DefaultConfigInfo(PipelineConfig& configInfo)
	{
		configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

		configInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportState.viewportCount = 1;
		configInfo.viewportState.pViewports = nullptr;
		configInfo.viewportState.scissorCount = 1;
		configInfo.viewportState.pScissors = nullptr;

		configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizer.depthClampEnable = VK_FALSE;
		configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizer.lineWidth = 1.0f;
		configInfo.rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		configInfo.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizer.depthBiasEnable = VK_FALSE;
		configInfo.rasterizer.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizer.depthBiasClamp = 0.0f;
		configInfo.rasterizer.depthBiasSlopeFactor = 0.0f;

		configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampling.sampleShadingEnable = VK_FALSE;
		configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampling.minSampleShading = 1.0f;
		configInfo.multisampling.pSampleMask = nullptr;
		configInfo.multisampling.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampling.alphaToOneEnable = VK_FALSE;

		configInfo.colorBlendAttachment.colorWriteMask = 
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlending.logicOpEnable = VK_FALSE;
		configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlending.attachmentCount = 1;
		configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlending.blendConstants[0] = 0.0f;
		configInfo.colorBlending.blendConstants[1] = 0.0f;
		configInfo.colorBlending.blendConstants[2] = 0.0f;
		configInfo.colorBlending.blendConstants[3] = 0.0f;

		configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencil.depthTestEnable = VK_TRUE;
		configInfo.depthStencil.depthWriteEnable = VK_TRUE;
		configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencil.minDepthBounds = 0.0f;
		configInfo.depthStencil.maxDepthBounds = 1.0f;
		configInfo.depthStencil.stencilTestEnable = VK_FALSE;
		configInfo.depthStencil.front = {};
		configInfo.depthStencil.back = {};

		configInfo.dynamicState = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicState.size());
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicState.data();
		configInfo.dynamicStateInfo.flags = 0;

		configInfo.bindingDescriptions = Model::Vertex::GetBindingDescriptions();
		configInfo.attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config)
	{
		auto vertCode = ReadFile(vertPath);
		auto fragCode = ReadFile(fragPath);

		CreateShaderModule(vertCode, &vertShaderModule);
		CreateShaderModule(fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo vertShaderStage = {};
		vertShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStage.module = vertShaderModule;
		vertShaderStage.pName = "main";
		vertShaderStage.flags = 0;
		vertShaderStage.pNext = nullptr;
		vertShaderStage.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStage = {};
		fragShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStage.module = fragShaderModule;
		fragShaderStage.pName = "main";
		fragShaderStage.flags = 0;
		fragShaderStage.pNext = nullptr;
		fragShaderStage.pSpecializationInfo = nullptr;

		auto& bindingDescription = config.bindingDescriptions;
		auto& attributeDescription = config.attributeDescriptions;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = new VkPipelineShaderStageCreateInfo[2]{ vertShaderStage, fragShaderStage };
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pViewportState = &config.viewportState;
		pipelineInfo.pInputAssemblyState = &config.inputAssembly;
		pipelineInfo.pRasterizationState = &config.rasterizer;
		pipelineInfo.pMultisampleState = &config.multisampling;

		pipelineInfo.pColorBlendState = &config.colorBlending;
		pipelineInfo.pDepthStencilState = &config.depthStencil;
		pipelineInfo.pDynamicState = &config.dynamicStateInfo;

		pipelineInfo.layout = config.pipelineLayout;
		pipelineInfo.renderPass = config.renderPass;
		pipelineInfo.subpass = config.subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
	}

	void Pipeline::BindCommandBuffer(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}