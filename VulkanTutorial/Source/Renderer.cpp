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

		VkResult result = swapchain->AcquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image");

		isFrameStarted = true;

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer");

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		assert(isFrameStarted && "Cannot end frame when frame is not in progress");

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

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->GetRenderPass();
		renderPassInfo.framebuffer = swapchain->GetFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->GetSwapChainExtent();
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = { {0, 0}, swapchain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot end frame when frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::CreateCommandBuffers()
	{
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

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
		VkExtent2D extent = window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device.device());

		if (swapchain == nullptr)
			swapchain = std::make_unique<SwapChain>(device, extent);
		else
		{
			std::shared_ptr<SwapChain> oldSwapchain = std::move(swapchain);
			swapchain = std::make_unique<SwapChain>(device, extent, oldSwapchain);

			if (!oldSwapchain->CompareSwapFormats(*swapchain.get()))
				throw std::runtime_error("Swapchain image format or depth format has changed");
		}

		// Todo: If the render pass is compatible with the new swap chain we don't need to recreate it
		// CreatePipeline();
	}
}