#include "Mfepch.h"
#include "RenderSystem.h"

#include <stdexcept>
#include <array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MyFirstEngine
{
	RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
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

	void RenderSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
	{
		VkPushConstantRange pushConstantRange = CreatePushConstantRange();

		VkDescriptorSetLayout setLayouts[] = { descriptorSetLayouts[0], descriptorSetLayouts[1] }; // TODO CHANGE WITH LOOP
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = setLayouts;
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