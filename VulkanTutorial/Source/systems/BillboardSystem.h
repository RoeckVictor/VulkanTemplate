#pragma once

#include "RenderSystem.h"

namespace VulkanTutorial
{
	class BillboardSystem : public RenderSystem
	{
	public:
		BillboardSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		~BillboardSystem();

		BillboardSystem(const BillboardSystem&) = delete;
		BillboardSystem& operator=(const BillboardSystem&) = delete;

		void Render(FrameInfo& frameInfo) override;

		void CreateTextureSet(DescriptorSetLayout& setLayout, DescriptorPool& pool);

	private:
		VkPushConstantRange CreatePushConstantRange() override;
		void CreatePipeline(VkRenderPass renderPass) override;

		VkDescriptorSet textureSet{ VK_NULL_HANDLE }; //texture defaulted to null
		Texture texture;
	};
}