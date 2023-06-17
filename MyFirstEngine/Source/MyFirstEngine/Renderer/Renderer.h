#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cassert>

#include "Platform/VulkanWindow.h"
#include "Device.h"
#include "Swapchain.h"

namespace MyFirstEngine
{
	class Renderer
	{
	public:
		Renderer(VulkanWindow& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

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

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

		VulkanWindow& window;
		Device& device;
		std::unique_ptr<SwapChain> swapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}