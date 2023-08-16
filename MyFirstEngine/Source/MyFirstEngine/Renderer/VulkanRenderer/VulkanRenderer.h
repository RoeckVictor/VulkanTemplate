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

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void SetClearColor(float r, float g, float b, float a) { m_ClearColor = { r, g, b, a }; }

		bool IsFrameInProgress() const { return m_IsFrameStarted; }
		VkRenderPass GetSwapChainRenderPass() const { return m_Swapchain->GetRenderPass(); }
		float GetAspectRatio() const { return m_Swapchain->ExtentAspectRatio(); }
		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			MFE_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame is not in progress");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}
		int GetFrameIndex() const
		{
			MFE_CORE_ASSERT(m_IsFrameStarted, "Cannot get frame index when frame is not in progress");
			return m_CurrentFrameIndex;
		}

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

		VulkanGlfwWindow& m_Window;
		Device& m_Device;
		std::unique_ptr<SwapChain> m_Swapchain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex;
		bool m_IsFrameStarted;

		VkClearColorValue m_ClearColor = { 0.01f, 0.01f, 0.01f, 1.0f };
	};
}