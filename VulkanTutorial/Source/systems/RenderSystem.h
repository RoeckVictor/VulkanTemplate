#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"
#include "Camera.h"
#include "FrameInfo.h"

namespace VulkanTutorial
{
	class RenderSystem 
	{
	public:
		RenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual void Render(FrameInfo& frameInfo);

		Pipeline& GetPipeline() { return *pipeline; }

	protected:
		virtual VkPushConstantRange CreatePushConstantRange();
		void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
		virtual void CreatePipeline(VkRenderPass renderPass);

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}