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
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	VkPushConstantRange RenderSystem::CreatePushConstantRange()
	{
		return VkPushConstantRange();
	}

	void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayouts)
	{
		VkPushConstantRange pushConstantRange = CreatePushConstantRange();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayouts;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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