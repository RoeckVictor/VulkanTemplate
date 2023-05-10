#include "RenderSystem.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayouts)
		: device(device)
	{
		// CreatePipelineLayout(globalSetLayout);
		// CreatePipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		// [COMMENT] Destroy the pipeline layout when the system is destroyed
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	VkPushConstantRange RenderSystem::CreatePushConstantRange()
	{
		return VkPushConstantRange();
	}

	void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayouts)
	{
		// [COMMENT] The pipeline layout allows us to pass uniform values and push constants to shaders
		VkPushConstantRange pushConstantRange = CreatePushConstantRange();

		// [COMMENT] Struct used to create the pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// [COMMENT] The number of descriptor set layouts
		pipelineLayoutInfo.setLayoutCount = 1;
		// [COMMENT] The descriptor set layouts we will be using to pass uniforms to the shaders
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayouts;
		// [COMMENT] The number of push constants
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		// [COMMENT] The push constant ranges struct we defined earlier
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		// [COMMENT] Create the pipeline layout and check for errors
		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");
	}

	void RenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
	}

	void RenderSystem::Render(FrameInfo& frameInfo)
	{

	}
}