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
		RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual void Render(FrameInfo& frameInfo);

	protected:
		virtual VkPushConstantRange CreatePushConstantRange();
		void CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
		virtual void CreatePipeline(VkRenderPass renderPass);

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}