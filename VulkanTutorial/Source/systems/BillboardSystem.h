#pragma once

#include "RenderSystem.h"

namespace VulkanTutorial
{
	class BillboardSystem : public RenderSystem
	{
	public:
		BillboardSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayouts);
		~BillboardSystem();

		BillboardSystem(const BillboardSystem&) = delete;
		BillboardSystem& operator=(const BillboardSystem&) = delete;

		void Render(FrameInfo &frameInfo) override;

	private:
		VkPushConstantRange CreatePushConstantRange() override;
		void CreatePipeline(VkRenderPass renderPass) override;
	};
}