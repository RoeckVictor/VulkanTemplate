#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

namespace MyFirstEngine
{
	class SwapChain 
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& deviceRef, VkExtent2D windowExtent);
		SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool CompareSwapFormats(const SwapChain& swapChain) const
		{
			return swapChain.m_ScDepthFormat == m_ScDepthFormat && swapChain.m_ScImageFormat == m_ScImageFormat;
		}
		VkFormat FindDepthFormat();
		size_t ImageCount() { return m_ScImages.size(); }
		uint32_t Width() { return m_ScExtent.width; }
		uint32_t Height() { return m_ScExtent.height; }
		float ExtentAspectRatio() { return static_cast<float>(m_ScExtent.width) / static_cast<float>(m_ScExtent.height);}

		VkFramebuffer GetFrameBuffer(int index) { return m_ScFramebuffers[index]; }
		VkRenderPass GetRenderPass() { return m_RenderPass; }
		VkImageView GetImageView(int index) { return m_ScImageViews[index]; }
		VkFormat GetSwapChainImageFormat() { return m_ScImageFormat; }
		VkExtent2D GetSwapChainExtent() { return m_ScExtent; }

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateColorResources();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_ScImageFormat;
		VkFormat m_ScDepthFormat;
		VkExtent2D m_ScExtent;

		std::vector<VkFramebuffer> m_ScFramebuffers;
		VkRenderPass m_RenderPass;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemorys;
		std::vector<VkImageView> m_DepthImageViews;

		std::vector<VkImage> m_ColorImages;
		std::vector<VkDeviceMemory> m_ColorImageMemorys;
		std::vector<VkImageView> m_ColorImageViews;

		std::vector<VkImage> m_ScImages;
		std::vector<VkImageView> m_ScImageViews;

		Device& m_Device;
		VkExtent2D m_WindowExtent;

		VkSwapchainKHR m_SwapChain;
		std::shared_ptr<SwapChain> m_OldSwapChain = nullptr;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		size_t m_CurrentFrame = 0;
	};
}
