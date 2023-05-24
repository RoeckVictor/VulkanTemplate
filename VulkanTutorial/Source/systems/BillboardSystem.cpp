#include "BillboardSystem.h"

#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	BillboardSystem::BillboardSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
	: RenderSystem(device, renderPass, descriptorSetLayouts),
	  texture("Resources/Textures/PointLight.png", device)
	{
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderPass);
	}

	BillboardSystem::~BillboardSystem()
	{
	}

	VkPushConstantRange BillboardSystem::CreatePushConstantRange()
	{
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		return pushConstantRange;
	}

	void BillboardSystem::CreatePipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfig pipelineConfig{};
		Pipeline::DefaultConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampling.rasterizationSamples = device.GetMaxUsableSampleCount();
		pipeline = std::make_unique<Pipeline>(device, "Resources/Shaders/billboard.vert.spv", "Resources/Shaders/billboard.frag.spv", pipelineConfig);
	}

	void BillboardSystem::Render(FrameInfo& frameInfo)
	{

		pipeline->BindCommandBuffer(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &frameInfo.globalDescriptorSet, 0, nullptr
		);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			1, 1, &textureSet, 0, nullptr
		);

		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr)
				continue;

			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.0f);
			push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}

	void BillboardSystem::CreateTextureSet(DescriptorSetLayout& setLayout, DescriptorPool& pool)
	{
		VkDescriptorImageInfo imageInfo = texture.GetImageInfo();

		DescriptorWriter(setLayout, pool)
			.WriteImage(0, &imageInfo)
			.Build(textureSet);
	}
}