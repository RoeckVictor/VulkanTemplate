#pragma once

#include "Device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace VulkanTutorial
{
	class SwapChain 
	{
	public:
		// [COMMENT] To avoid having to wait for the previous frame to finish before rendering the next on,
		// [COMMENT] we have multiple image "slots" in the swap chain, this is the max number of said slots
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& deviceRef, VkExtent2D windowExtent);
		SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		// [COMMENT] Get the frame buffer at index, used for rendering
		VkFramebuffer GetFrameBuffer(int index) { return swapChainFramebuffers[index]; }
		// [COMMENT] Get the render pass used for the swap chain
		VkRenderPass GetRenderPass() { return renderPass; }
		// [COMMENT] Get the image view for the image at index
		VkImageView GetImageView(int index) { return swapChainImageViews[index]; }
		// [COMMENT] Get the number of images in the swap chain
		size_t ImageCount() { return swapChainImages.size(); }
		// [COMMENT] Get the swap chain image format (color channel, type and space)
		VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
		// [COMMENT] Getters for the swap chain extent (resolution)
		VkExtent2D GetSwapChainExtent() { return swapChainExtent; }
		uint32_t Width() { return swapChainExtent.width; }
		uint32_t Height() { return swapChainExtent.height; }
		float ExtentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);}

		// [COMMENT] Find the right format for the depth image, depending on the available formats and the ones we want
		VkFormat FindDepthFormat();

		// [COMMENT] Wait for and acquire the next image to present to the framebuffer
		VkResult AcquireNextImage(uint32_t* imageIndex);
		// [COMMENT] Submit the fully recorded command buffer to the graphics queue
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		// [COMMENT] Check if the new swap chain is compatible with the old one
		bool CompareSwapFormats(const SwapChain& swapChain) const
		{
			return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
				swapChain.swapChainImageFormat == swapChainImageFormat;
		}

	private:
		// [COMMENT] Creates everything required for the swap chain to work
		void Init();
		// [COMMENT] Creates the actual swap chain
		void CreateSwapChain();
		// [COMMENT] Creates the image views to access the images in the swap chain
		void CreateImageViews();
		// [COMMENT] Creates the depth image views to allow depth testing
		void CreateDepthResources();
		// [COMMENT] Creates the color image views used for multisampling
		void CreateColorResources();
		// [COMMENT] Defines and creates the render pass (description of the framebuffer attachments)
		void CreateRenderPass();
		// [COMMENT] Creates the framebuffers (one for each image in the swap chain)
		void CreateFramebuffers();
		// [COMMENT] Creates the semaphores and fences for synchronization
		void CreateSyncObjects();

		// [COMMENT] Choose which surface format (color depth) the swap chain uses
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		// [COMMENT] Choose the presentation mode (conditions for "swapping" images to the screen)
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		// [COMMENT] Choose the resolution of images in the swap chain
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		// [COMMENT] The swap chain image format (color channel, type and space)
		VkFormat swapChainImageFormat;
		// [COMMENT] The swap chain depth format (color channel, type and space)
		VkFormat swapChainDepthFormat;
		// [COMMENT] The swap chain extent (resolution)
		VkExtent2D swapChainExtent;

		// [COMMENT] The list of framebuffers (one for each image)
		std::vector<VkFramebuffer> swapChainFramebuffers;
		// [COMMENT] The swap chain render pass (defines the framebuffer)
		VkRenderPass renderPass;

		// [COMMENT] Image used for depth testing
		std::vector<VkImage> depthImages;
		// [COMMENT] Memory for the depth image
		std::vector<VkDeviceMemory> depthImageMemorys;
		// [COMMENT] Image view for the depth image
		std::vector<VkImageView> depthImageViews;

		std::vector<VkImage> colorImages;
		std::vector<VkDeviceMemory> colorImageMemorys;
		std::vector<VkImageView> colorImageViews;

		// [COMMENT] Handles to the images inside the swap chain
		std::vector<VkImage> swapChainImages;
		// [COMMENT] Image view for every image in the swap chain (allows us to bind them as render targets)
		std::vector<VkImageView> swapChainImageViews;

		// [COMMENT] Class containing the physical device, logical device and the surface
		Device& device;
		VkExtent2D windowExtent;

		// [COMMENT] The actual swap chain object
		VkSwapchainKHR swapChain;
		std::shared_ptr<SwapChain> oldSwapChain = nullptr;

		// [COMMENT] Each frame in flight has its own set of semaphores and fences
		// [COMMENT] Semaphores allows synchronization inside the GPU
		// [COMMENT] Semaphores telling if the images in the swapchain are available for rendering
		std::vector<VkSemaphore> imageAvailableSemaphores;
		// [COMMENT] Semaphores telling if the images in the swapchain have been rendered and are ready for display
		std::vector<VkSemaphore> renderFinishedSemaphores;
		// [COMMENT] Fences allow synchronization between the CPU-GPU
		// [COMMENT] Fences to wait for the GPU to finish rendering the frame
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		// [COMMENT] To use the right objects (semaphores, fences...) every frame, we need to keep track of the current frame
		size_t currentFrame = 0;
	};
}
