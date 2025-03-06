#include "Mfepch.h"
#include "VulkanRenderer.h"

#include <stdexcept>
#include <array>

namespace MyFirstEngine
{
	VulkanRenderer::VulkanRenderer(VulkanGlfwWindow& window, Device& device)
		: m_Window(window),
		  m_Device(device),
		  m_IsFrameStarted(false),
		  m_CurrentFrameIndex(0)
	{
		RecreateSwapchain();
		CreateCommandBuffers();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		FreeCommandBuffers();
	}

	VkCommandBuffer VulkanRenderer::BeginFrame()
	{
		MFE_CORE_ASSERT(!m_IsFrameStarted, "Cannot start frame when frame is already in progress");

		VkResult result = m_Swapchain->AcquireNextImage(&m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return nullptr;
		}

		MFE_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image");

		m_IsFrameStarted = true;
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		MFE_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer");

		return commandBuffer;
	}

	void VulkanRenderer::EndFrame()
	{
		MFE_CORE_ASSERT(m_IsFrameStarted, "Cannot end frame when frame is not in progress");

		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer");

		VkResult result = m_Swapchain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized())
		{
			m_Window.ResetResized();
			RecreateSwapchain();
		}
		else
		{
			MFE_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swap chain image");
		}

		m_IsFrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		MFE_CORE_ASSERT(m_IsFrameStarted, "Cannot create new frame is old frame is still in progress");
		MFE_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Can't begin render poss on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_Swapchain->GetRenderPass();
		renderPassInfo.framebuffer = m_Swapchain->GetFrameBuffer(m_CurrentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_Swapchain->GetSwapChainExtent();
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_ClearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_Swapchain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_Swapchain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor = { {0, 0}, m_Swapchain->GetSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		MFE_CORE_ASSERT(m_IsFrameStarted, "Cannot end frame when frame is not in progress");
		MFE_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void VulkanRenderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device.GetCommandPool();
		allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		MFE_CORE_ASSERT(vkAllocateCommandBuffers(m_Device.GetLogicalDevice(), &allocInfo, m_CommandBuffers.data()) == VK_SUCCESS,
						"Failed to allocate command buffers");
	}

	void VulkanRenderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.GetLogicalDevice(), m_Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void VulkanRenderer::RecreateSwapchain()
	{
		VkExtent2D extent = m_Window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.GetLogicalDevice());

		if (m_Swapchain == nullptr)
		{
			m_Swapchain = std::make_unique<SwapChain>(m_Device, extent);
		}	
		else
		{
			std::shared_ptr<SwapChain> oldSwapchain = std::move(m_Swapchain);
			m_Swapchain = std::make_unique<SwapChain>(m_Device, extent, oldSwapchain);

			MFE_CORE_ASSERT(oldSwapchain->CompareSwapFormats(*m_Swapchain.get()), "Swapchain image format or depth format has changed");
		}
	}
}