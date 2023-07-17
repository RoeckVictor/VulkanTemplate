#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cassert>

#include "Platform/VulkanGlfwWindow.h"
#include "Device.h"
#include "Swapchain.h"

namespace MyFirstEngine
{
	class VulkanRenderer
	{
	public:
		VulkanRenderer(VulkanGlfwWindow& window, Device& device);
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return swapchain->GetRenderPass(); }
		float GetAspectRatio() const { return swapchain->ExtentAspectRatio(); }
		bool IsFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int GetFrameIndex() const 
		{ 
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex; 
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		void SetClearColor(float r, float g, float b, float a) { clearColor = { r, g, b, a }; }

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

		VulkanGlfwWindow& window;
		Device& device;
		std::unique_ptr<SwapChain> swapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;

		VkClearColorValue clearColor = { 0.01f, 0.01f, 0.01f, 1.0f };
	};
}