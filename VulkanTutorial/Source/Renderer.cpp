#include "Renderer.h"

#include <stdexcept>
#include <array>

namespace VulkanTutorial
{
	Renderer::Renderer(Window& window, Device& device)
		: window(window),
		  device(device),
		  isFrameStarted(false),
		  currentFrameIndex(0)
	{
		RecreateSwapchain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		assert(!isFrameStarted && "Cannot start frame when frame is already in progress");

		// [COMMENT] Use vkAcquireNextImageKHR to acquire an image from the swap chain
		VkResult result = swapchain->AcquireNextImage(&currentImageIndex);

		// [COMMENT] If the swap chain is out of date (eg. the window was resized), recreate it
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return nullptr;
		}

		// [COMMENT] Other errors will throw a runtime error (except for VK_SUBOPTIMAL_KHR which is not an error)
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image");

		isFrameStarted = true;

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

		// [COMMENT] Resets the command buffer to a state where it can be re-recorded
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// [COMMENT] Specifies how the command buffer is used
		beginInfo.flags = 0;
		// [COMMENT] Optional inheritance info for secondary command buffers
		beginInfo.pInheritanceInfo = nullptr;

		// [COMMENT] Beging recording commands to the command buffer and check for errors
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer");

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		assert(isFrameStarted && "Cannot end frame when frame is not in progress");

		// [COMMENT] Ends the current command buffer and check for errors
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer");

		VkResult result = swapchain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasResized())
		{
			window.ResetResized();
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to present swap chain image");

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot create new frame is old frame is still in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render poss on command buffer from a different frame");

		// [COMMENT] To draw a frame we first start by beginning the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		// [COMMENT] The render pass we created in the swapchain
		renderPassInfo.renderPass = swapchain->GetRenderPass();
		// [COMMENT] The framebuffer containing the attachments that are going to be used as render targets in the render pass
		// [COMMENT] We get the right framebuffer using the current image index
		renderPassInfo.framebuffer = swapchain->GetFrameBuffer(currentImageIndex);
		// [COMMENT] The size of the render area (where the shaders load and store pixels)
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->GetSwapChainExtent();
		// [COMMENT] The values used to clear the framebuffer color and depth data
		// [COMMENT] clearValues[0] is for the color buffer, we set it to dark gray
		// [COMMENT] clearValues[1] is for the depth buffer
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		// [COMMENT] 1 is at farthest and 0 is at closest
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// [COMMENT] We can now begin the render pass
		// [COMMENT] 3rd parameter defines how the drawing commands within the render pass will be provided
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// [COMMENT] We set the viewport and scissor definition here so we can resize the window dynamically
		// [COMMENT] A viewport describes the region of the framebuffer that the output will be rendered to.
		VkViewport viewport = {};
		// [COMMENT] Here the viewport goest from (0, 0) to the (width, height) of the swapchain.
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain->GetSwapChainExtent().height);
		// [COMMENT] Range of depth values to use for the framebuffer
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		// [COMMENT] A scissor rectangle defines in which regions pixels will actually be stored
		// [COMMENT] Every pixel outside the scissor rectangle will be discarded, so it works like a filter
		// [COMMENT] Here we want to draw to the entire framebuffer, so we use the same size as the viewport
		VkRect2D scissor = { {0, 0}, swapchain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot end frame when frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		// [COMMENT] We end the render pass to end the frame
		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::CreateCommandBuffers()
	{
		// [COMMENT] Set the number of command buffers to allocate to the maximum number of frames in flight
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		// [COMMENT] Allocate command buffers from the command pool
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		// [COMMENT] The command buffer is primary, which means that it can be submitted directly to the queue
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		// [COMMENT] Specify the command pool to allocate from
		allocInfo.commandPool = device.GetCommandPool();
		// [COMMENT] The number of command buffers to allocate
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		// [COMMENT] Allocate the command buffers and check for errors
		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers");
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(device.device(), device.GetCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void Renderer::RecreateSwapchain()
	{
		// [COMMENT] Get the new extent of the window
		VkExtent2D extent = window.GetExtent();
		// [COMMENT] If the width and height are 0, it means the window is minimized, so we just pause until it's not minimized anymore
		while (extent.width == 0 || extent.height == 0)
		{
			extent = window.GetExtent();
			glfwWaitEvents();
		}

		// [COMMENT] Wait for the device to be idle before recreating the swapchain
		vkDeviceWaitIdle(device.device());

		// [COMMENT] If there are no swapchain (for example at the first frame), we create one
		if (swapchain == nullptr)
			swapchain = std::make_unique<SwapChain>(device, extent);
		else
		{
			// [COMMENT] Store the old swapchain
			std::shared_ptr<SwapChain> oldSwapchain = std::move(swapchain);
			// [COMMENT] Create a new swapchain
			swapchain = std::make_unique<SwapChain>(device, extent, oldSwapchain);

			// [COMMENT] Makes sure the old swapchain is compatible with the new one
			if (!oldSwapchain->CompareSwapFormats(*swapchain.get()))
				throw std::runtime_error("Swapchain image format or depth format has changed");
		}

		// Todo: If the render pass is compatible with the new swap chain we don't need to recreate it
		// CreatePipeline();
	}
}