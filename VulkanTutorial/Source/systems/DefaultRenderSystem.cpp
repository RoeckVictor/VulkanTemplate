#include "DefaultRenderSystem.h"

#include <stdexcept>
#include <array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	DefaultRenderSystem::DefaultRenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
	: RenderSystem(device, renderPass, descriptorSetLayouts)
	{
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderPass);
	}

	DefaultRenderSystem::~DefaultRenderSystem()
	{
	}

	VkPushConstantRange DefaultRenderSystem::CreatePushConstantRange()
	{
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);

		return pushConstantRange;
	}

	void DefaultRenderSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfig pipelineConfig{};
		Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampling.rasterizationSamples = device.GetMaxUsableSampleCount();
		// pipeline = std::make_unique<Pipeline>(device, "Resources/Shaders/basic.vert.spv", "Resources/Shaders/basic.frag.spv", pipelineConfig);
		pipeline = std::make_unique<Pipeline>(device, "Resources/Shaders/texture_test.vert.spv", "Resources/Shaders/texture_test.frag.spv", pipelineConfig);
	}

	void DefaultRenderSystem::Render(FrameInfo& frameInfo)
	{
		pipeline->BindCommandBuffer(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &frameInfo.globalDescriptorSet, 0, nullptr
		);

		for (auto& keyValue : frameInfo.gameObjects)
		{
			GameObject& obj = keyValue.second;

			if (obj.model == nullptr) continue;

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
				1, 1, &obj.model->textureSet, 0, nullptr
			);

			PushConstantData push = {};
			push.modelMatrix = obj.transform.transform();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);

			obj.model->Bind(frameInfo.commandBuffer);
			obj.model->Draw(frameInfo.commandBuffer);
		}
	}
}