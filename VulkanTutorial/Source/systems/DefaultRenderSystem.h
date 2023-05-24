#pragma once

#include "RenderSystem.h"

namespace VulkanTutorial
{
	class DefaultRenderSystem : public RenderSystem
	{
	public:
		DefaultRenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		~DefaultRenderSystem();

		DefaultRenderSystem(const DefaultRenderSystem&) = delete;
		DefaultRenderSystem& operator=(const DefaultRenderSystem&) = delete;

		void Render(FrameInfo &frameInfo) override;

	private:
		VkPushConstantRange CreatePushConstantRange() override;
		void CreatePipeline(VkRenderPass renderPass) override;

		struct PushConstantData 
		{
			glm::mat4 modelMatrix{ 1.0f };
			glm::mat4 normalMatrix{ 1.0f };
		};
	};
}