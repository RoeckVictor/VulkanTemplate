#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cassert>

#include "Window.h"
#include "Device.h"
#include "Swapchain.h"

namespace VulkanTutorial
{
	class Renderer
	{
	public:
		Renderer(Window& window, Device& device);
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

		// [COMMENT] Begins the frame and returns the command buffer to record to
		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		// [COMMENT] Creates a command buffer for each frames in flight	
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		// [COMMENT] Recreates the swap chain (eg. when the window is resized)
		void RecreateSwapchain();

		Window& window;
		Device& device;
		std::unique_ptr<SwapChain> swapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}