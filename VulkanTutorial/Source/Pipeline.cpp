#include "Pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace VulkanTutorial
{
	Pipeline::Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config)
		: device(device)
	{
		CreateGraphicsPipeline(vertPath, fragPath, config);
	}

	Pipeline::~Pipeline()
	{
		// [COMMENT] Destroy the shader modules
		vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
		// [COMMENT] Destroy the graphics pipeline
		vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
	}

	void Pipeline::DefaultConfigInfo(PipelineConfig& configInfo)
	{
		// [COMMENT] Input assembly config: The input assembly collects raw vertex data from the buffers
		configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		// [COMMENT] Specify the primitive topology to use
		configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// [COMMENT] Specify whether primitive restart should be enabled, which is used for indexed draw calls (through ibo)
		configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

		// [COMMENT] Viewport config: A viewport describes the region of the framebuffer that the output will be rendered to
		configInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		// [COMMENT] Specify the number of viewport rectangles to use
		configInfo.viewportState.viewportCount = 1;
		// [COMMENT] Specify the actual viewport to use (we use a dynamic viewport so there is no need for this)
		configInfo.viewportState.pViewports = nullptr;
		// [COMMENT] Specify the number of scissor rectangles to use
		configInfo.viewportState.scissorCount = 1;
		// [COMMENT] Specify the actual scissor to use (we use a dynamic scissor so there is no need for this)
		configInfo.viewportState.pScissors = nullptr;

		// [COMMENT] Rasterizer config: The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and 
		// [COMMENT] turns it into fragments to be colored by the fragment shader
		configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// [COMMENT] Specify whether to clamp fragments that are beyond the near and far planes instead of discarding them
		configInfo.rasterizer.depthClampEnable = VK_FALSE;
		// [COMMENT] Specify whether to discard primitives immediately before the rasterization stage (basically disables output to the framebuffer)
		configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
		// [COMMENT] Specify the polygon drawing mode (fill, line (wireframe), point)
		// [COMMENT] If we want to use an other mode than fill, we need to enable the corresponding GPU feature
		configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		// [COMMENT] Specify the width of the lines in terms of number of fragments, anything thicker than 1.0f requires to enable the wideLines GPU feature
		configInfo.rasterizer.lineWidth = 1.0f;
		// [COMMENT] Specify the faces cull mode to use (none, front, back, both)
		configInfo.rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		// [COMMENT] Specify the front face vertex order to use (clockwise or counter-clockwise)
		configInfo.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		// [COMMENT] Deph bias can alter the fragement's depth values
		configInfo.rasterizer.depthBiasEnable = VK_FALSE;
		// [COMMENT] The different ways we can alter the depth values
		configInfo.rasterizer.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizer.depthBiasClamp = 0.0f;
		configInfo.rasterizer.depthBiasSlopeFactor = 0.0f;

		// [COMMENT] Multisampling config: Multisampling is one of the ways to perform anti-aliasing, requires a GPU feature
		configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampling.sampleShadingEnable = VK_FALSE;
		// [COMMENT] Specify the number of samples to use per fragment
		configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampling.minSampleShading = 1.0f;
		configInfo.multisampling.pSampleMask = nullptr;
		configInfo.multisampling.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampling.alphaToOneEnable = VK_FALSE;

		// [COMMENT] Color blending config: Color blending is used to combine the color of the fragment that 
		// [COMMENT] is already in the framebuffer with the color that the fragment shader returns
		// [COMMENT] Specify which color channels should be written to the framebuffer
		configInfo.colorBlendAttachment.colorWriteMask = 
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		// [COMMENT] Specify whether to enable blending
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		// [COMMENT] Specify the blending factor of the source (new fragment)
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		// [COMMENT] Specify the blending factor of the destination (fragment already in framebuffer)
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		// [COMMENT] Specify the operation to use to combine the source and destination colors
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		// [COMMENT] Same as the previous 3 lines but for the alpha channel
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// [COMMENT] Specify the global color blending settings
		configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlending.logicOpEnable = VK_FALSE;
		configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlending.attachmentCount = 1;
		configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlending.blendConstants[0] = 0.0f;
		configInfo.colorBlending.blendConstants[1] = 0.0f;
		configInfo.colorBlending.blendConstants[2] = 0.0f;
		configInfo.colorBlending.blendConstants[3] = 0.0f;

		// [COMMENT] Depth and stencil testing config
		configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		// [COMMENT] Specify whether to enable depth testing
		configInfo.depthStencil.depthTestEnable = VK_TRUE;
		// [COMMENT] Specify whether to actually write the depth value of the new fragment that passed the depth test
		configInfo.depthStencil.depthWriteEnable = VK_TRUE;
		// [COMMENT] Specify the comparison operation that will be performed to keep or discard the new fragment
		configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		// [COMMENT] Test that let's us discard the fragments that fall outside of the specified depth range (disabled here)
		configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencil.minDepthBounds = 0.0f;
		configInfo.depthStencil.maxDepthBounds = 1.0f;
		// [COMMENT] Configures stencil buffer operations (disabled here)
		configInfo.depthStencil.stencilTestEnable = VK_FALSE;
		configInfo.depthStencil.front = {};
		configInfo.depthStencil.back = {};

		// [COMMENT] Configures the dynamic states, which can be changed without recreating the pipeline
		// [COMMENT] Having the viewport and scissor allows us to resize the window at runtime without recreating the pipeline
		configInfo.dynamicState = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		// [COMMENT] Number of dynamic states
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicState.size());
		// [COMMENT] List of dynamic states
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicState.data();
		configInfo.dynamicStateInfo.flags = 0;

		// [COMMENT] Get the binding and attribute descriptions of a vertex from the Vertex struct
		configInfo.bindingDescriptions = Model::Vertex::GetBindingDescriptions();
		configInfo.attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config)
	{
		// [COMMENT] Read shader code from files
		auto vertCode = ReadFile(vertPath);
		auto fragCode = ReadFile(fragPath);

		// [COMMENT] Before we can pass the code to the pipeline, we have to wrap it in a VkShaderModule object
		CreateShaderModule(vertCode, &vertShaderModule);
		CreateShaderModule(fragCode, &fragShaderModule);

		// [COMMENT] Assigns the vertex shader to a specific pipeline stage
		VkPipelineShaderStageCreateInfo vertShaderStage = {};
		vertShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// [COMMENT] Tells the pipeline to use the shader as a vertex shader
		vertShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		// [COMMENT] Assign the shader module to the pipeline stage
		vertShaderStage.module = vertShaderModule;
		// [COMMENT] Specify the entry point of the shader
		vertShaderStage.pName = "main";
		vertShaderStage.flags = 0;
		vertShaderStage.pNext = nullptr;
		// [COMMENT] Specialization allows you to specify values for shader constants (optional)
		vertShaderStage.pSpecializationInfo = nullptr;

		// [COMMENT] Assigns the fragment shader to a specific pipeline stage
		VkPipelineShaderStageCreateInfo fragShaderStage = {};
		fragShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		// [COMMENT] Tells the pipeline to use the shader as a fragment shader
		fragShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		// [COMMENT] Assign the shader module to the pipeline stage
		fragShaderStage.module = fragShaderModule;
		// [COMMENT] Specify the entry point of the shader
		fragShaderStage.pName = "main";
		fragShaderStage.flags = 0;
		fragShaderStage.pNext = nullptr;
		// [COMMENT] Specialization allows you to specify values for shader constants (optional)
		fragShaderStage.pSpecializationInfo = nullptr;

		// [COMMENT] Binding and Attrubute descriptions we fetched from our Vertex structure
		auto& bindingDescription = config.bindingDescriptions;
		auto& attributeDescription = config.attributeDescriptions;
		// [COMMENT] Defines the format of the vertex data that will be passed to the vertex shader
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		// [COMMENT] Defines the format through the vertex binding and attribute (check the Vertex struct)
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();

		// [COMMENT] Creates the graphics pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// [COMMENT] The number of shader stages
		pipelineInfo.stageCount = 2;
		// [COMMENT] Assigns the shader stages to the pipeline
		pipelineInfo.pStages = new VkPipelineShaderStageCreateInfo[2]{ vertShaderStage, fragShaderStage };
		// [COMMENT] References the configs we created and received through the config parameter
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pViewportState = &config.viewportState;
		pipelineInfo.pInputAssemblyState = &config.inputAssembly;
		pipelineInfo.pRasterizationState = &config.rasterizer;
		pipelineInfo.pMultisampleState = &config.multisampling;

		pipelineInfo.pColorBlendState = &config.colorBlending;
		// [COMMENT] Depth stencil options
		pipelineInfo.pDepthStencilState = &config.depthStencil;
		pipelineInfo.pDynamicState = &config.dynamicStateInfo;

		pipelineInfo.layout = config.pipelineLayout;
		pipelineInfo.renderPass = config.renderPass;
		pipelineInfo.subpass = config.subpass;

		// [COMMENT] Theses two values are used if you want to derive from an existing pipeline (we will use them when resizing the window)
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		// [COMMENT] Create the graphics pipeline and check for errors
		if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		// [COMMENT] Fill in the infos required to create a shader module
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		// [COMMENT] Size of code in bytes
		createInfo.codeSize = code.size();
		// [COMMENT] The shader code itself, need to cast to uint32_t pointer
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		// [COMMENT] Create the shader module and makes sure it was created successfully
		if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");
	}

	void Pipeline::BindCommandBuffer(VkCommandBuffer commandBuffer)
	{
		// [COMMENT] Binds the graphics pipeline to the command buffer
		// [COMMENT] 2nd parameter specifies if the pipeline is a graphics or compute pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& path)
	{
		// [COMMENT] Open a file
		// [COMMENT] std::ios::ate: Start reading at the end of the file
		// [COMMENT] std::ios::binary: Read the file as binary file
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		// [COMMENT] Makes sure we successfully opened the file
		if (!file.is_open())
			throw std::runtime_error("Failed to open file: " + path);

		// [COMMENT] Load the file into the buffer we will return
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}