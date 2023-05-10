#include "SwapChain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace VulkanTutorial 
{

	SwapChain::SwapChain(Device& deviceRef, VkExtent2D extent)
		: device{ deviceRef }, windowExtent{ extent } 
	{
		Init();
	}

	SwapChain::SwapChain(Device& deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous)
		: device{ deviceRef }, windowExtent{ extent }, oldSwapChain(previous)
	{
		Init();

		// destroy old swapchain
		oldSwapChain = nullptr;
	}

	void SwapChain::Init() 
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateColorResources();
		CreateFramebuffers();
		CreateSyncObjects();
	}

	SwapChain::~SwapChain() 
	{
		// [COMMENT] frame buffers
		for (auto framebuffer : swapChainFramebuffers)
			vkDestroyFramebuffer(device.device(), framebuffer, nullptr);

		// [COMMENT] Destroy the image views
		for (auto imageView : swapChainImageViews) 
			vkDestroyImageView(device.device(), imageView, nullptr);
		swapChainImageViews.clear();

		// [COMMENT] Destroy the swap chain
		if (swapChain != nullptr) 
		{
			vkDestroySwapchainKHR(device.device(), swapChain, nullptr);
			swapChain = nullptr;
		}

		// [COMMENT] Destroy the depth buffer resources
		for (int i = 0; i < depthImages.size(); i++) 
		{
			vkDestroyImageView(device.device(), depthImageViews[i], nullptr);
			vkDestroyImage(device.device(), depthImages[i], nullptr);
			vkFreeMemory(device.device(), depthImageMemorys[i], nullptr);
		}

		// [COMMENT] Destroy the color buffer resources
		for (int i = 0; i < colorImages.size(); i++)
		{
			vkDestroyImageView(device.device(), colorImageViews[i], nullptr);
			vkDestroyImage(device.device(), colorImages[i], nullptr);
			vkFreeMemory(device.device(), colorImageMemorys[i], nullptr);
		}

		// [COMMENT] Destroy the render pass
		vkDestroyRenderPass(device.device(), renderPass, nullptr);

		// [COMMENT] Destroy the semaphores and fences
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			vkDestroySemaphore(device.device(), renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device.device(), imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device.device(), inFlightFences[i], nullptr);
		}
	}

	VkResult SwapChain::AcquireNextImage(uint32_t* imageIndex) 
	{
		// [COMMENT] Wait until the previous frame is finished
		vkWaitForFences(device.device(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		VkResult result = vkAcquireNextImageKHR(
			device.device(),
			swapChain,
			std::numeric_limits<uint64_t>::max(),
			imageAvailableSemaphores[currentFrame],  // [COMMENT] must be a non signaled semaphore
			VK_NULL_HANDLE,
			imageIndex);

		return result;
	}

	VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex) 
	{
		if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) 
			vkWaitForFences(device.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);

		imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

		// [COMMENT] Structure used to submit the command buffer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// [COMMENT] Specify which semaphores to wait on before execution begins
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		// [COMMENT] Specify at which stage of the pipeline to wait for each semaphore (with same index in waitSemaphores)
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		// [COMMENT] The number of semaphores to wait on
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		// [COMMENT] How many command buffers to submit
		submitInfo.commandBufferCount = 1;
		// [COMMENT] List of command buffers to submit
		submitInfo.pCommandBuffers = buffers;

		// [COMMENT] Specify which semaphores to signal once the command buffer(s) have finished
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		// [COMMENT] Number of semaphores to signal once the command buffer(s) have finished
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// [COMMENT] Reset the fence to the unsignaled state to be able to use it again
		vkResetFences(device.device(), 1, &inFlightFences[currentFrame]);

		// [COMMENT] Submit the command buffer to the queue and check for errors
		// [COMMENT] Last parameter is the fence that should be signaled once the command buffer(s) have finished executing
		if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer!");

		// [COMMENT] Structure used to present the swap chain image to the screen
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		// [COMMENT] Number of semaphores to wait on before presenting
		presentInfo.waitSemaphoreCount = 1;
		// [COMMENT] Semaphores to wait on before presenting
		presentInfo.pWaitSemaphores = signalSemaphores;

		// [COMMENT] Swap chains to present to
		VkSwapchainKHR swapChains[] = { swapChain };
		// [COMMENT] Number of swap chains to present to
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		// [COMMENT] Index of the image for each swap chain
		presentInfo.pImageIndices = imageIndex;

		// [COMMENT] Submit the request to present an image to the swap chain
		auto result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

		// [COMMENT] Advance to the next frame
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}

	void SwapChain::CreateSwapChain() 
	{
		SwapChainSupportDetails swapChainSupport = device.GetSwapChainSupport();

		// [COMMENT] Chooses surface format, present mode and extent
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		// [COMMENT] Choose number of images in swap chain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		// [COMMENT] Fill in the infos required to create the swap chain
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		// [COMMENT] Specify the surface to be associated with the swap chain
		createInfo.surface = device.surface();

		// [COMMENT] The number of images we picked previously
		createInfo.minImageCount = imageCount;
		// [COMMENT] The surface format we picked previously
		createInfo.imageFormat = surfaceFormat.format;
		// [COMMENT] The color space we picked previously
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		// [COMMENT] The extent we picked previously
		createInfo.imageExtent = extent;
		// [COMMENT] The number of layers each image consists of (always 1 unless developing a stereoscopic 3D application)
		createInfo.imageArrayLayers = 1;
		// [COMMENT] Specify what kind of operations we'll use the images in the swap chain for
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// [COMMENT] Specify how to handle swap chain images that will be used across multiple queue families
		// [COMMENT] Get the queue family indices
		QueueFamilyIndices indices = device.FindPhysicalQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

		// [COMMENT] If the graphics and present families are different, then we'll be using concurrent mode
		if (indices.graphicsFamily != indices.presentFamily) 
		{
			// [COMMENT] Concurrent mode: images can be used across multiple queue families without explicit ownership transfers
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		// [COMMENT] if graphicsFamily == presentFamily, then we can use exclusive mode
		else 
		{
			// [COMMENT] Exclusive mode: an image is owned by one queue family at a time and ownership must be explicitly transferred
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		// [COMMENT] Specifies a transform to be applied to images, here we specify that we do not want any transformation
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		// [COMMENT] Specifies if the alpha channel should be used for blending with other windows, here we ignore the alpha channel
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		// [COMMENT] The present mode we picked before
		createInfo.presentMode = presentMode;
		// [COMMENT] Clipp the pixels of the window that are not visible on the screen
		createInfo.clipped = VK_TRUE;

		// [COMMENT] If we are replacing an old swap chain, we need to specify a reference to that old swap chain
		createInfo.oldSwapchain = oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain;

		// [COMMENT] Creates the swap chain and makes sures it worked successfully
		if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			throw std::runtime_error("failed to create swap chain!");

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		// [COMMENT] Get the number of images that can be inside the swap chain
		vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		// [COMMENT] Get the handles to theses images
		vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());

		// [COMMENT] Stores the image format and extent for later use
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void SwapChain::CreateImageViews() 
	{
		// [COMMENT] Resize the vector to fit the image views (one for each image)
		swapChainImageViews.resize(swapChainImages.size());
		// [COMMENT] Iterate through the images and create their image views
		for (size_t i = 0; i < swapChainImages.size(); i++) 
			swapChainImageViews[i] = device.CreateImageView(swapChainImages[i], swapChainImageFormat, 1);
	}

	void SwapChain::CreateRenderPass() 
	{
		// [COMMENT] Defines de depth buffer
		VkAttachmentDescription depthAttachment{};
		// [COMMENT] Same format as the swap chain images
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = device.GetMaxUsableSampleCount();
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// [COMMENT] Don't care about storing the depth data after rendering since we won't use it after drawing has finished
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// [COMMENT] We don't know or care about the previous depth contents, so we can just use VK_IMAGE_LAYOUT_UNDEFINED
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// [COMMENT] Reference to the attachment for the first subpass
		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// [COMMENT] Defines the color buffer
		VkAttachmentDescription colorAttachment = {};
		// [COMMENT] Set the format (we already defined it in the CreateSwapChain function)
		colorAttachment.format = GetSwapChainImageFormat();
		// [COMMENT] Set the number of samples (we are currently not using multisampling so we set it to 1)
		colorAttachment.samples = device.GetMaxUsableSampleCount();
		// [COMMENT] loadOp and storeOp applies to color and depth data
		// [COMMENT] Defines what to do with the data in the attachment before rendering
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// [COMMENT] Defines what to do with the data in the attachment after rendering
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// [COMMENT] stencilStoreOp and stencilLoadOp applies to stencil data
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		// [COMMENT] Defines the layout the image will be in before the render pass begins
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// [COMMENT] Defines the layout the image will be transitioned to when the render pass finishes
		// [COMMENT] We need VK_IMAGE_LAYOUT_PRESENT_SRC_KHR if we want to present the image to the screen,
		// [COMMENT] this means that the image can't be presented directly, but must first be transformed
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// [COMMENT] Defines a subpass color attachment
		VkAttachmentReference colorAttachmentRef = {};
		// [COMMENT] Specifies the index of the attachment in the attachment descriptions array
		colorAttachmentRef.attachment = 0;
		// [COMMENT] Specifies which layout we would like the attachment to have during the subpass
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// [COMMENT] Defines the subpass
		VkSubpassDescription subpass = {};
		// [COMMENT] Specify the subpass to be a graphics subpass
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		// [COMMENT] Specify the index of the attachment it is referenced from the fragment shader with the layout(location = x) out vec4 outColor directive
		subpass.colorAttachmentCount = 1;
		// [COMMENT] The subpass color attachment we defined previously
		subpass.pColorAttachments = &colorAttachmentRef;
		// [COMMENT] The subpass depth attachment we defined previously
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		// [COMMENT] The subpass resolve attachment we defined previously
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		// [COMMENT] Defines a subpass dependency for the collor attachment and the depth attachment
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstSubpass = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// [COMMENT] Array containing our attachment descriptions
		std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		// [COMMENT] Create the render pass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		// [COMMENT] Specify the number of attachments
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// [COMMENT] Specify the attachments we defined previously
		renderPassInfo.pAttachments = attachments.data();
		// [COMMENT] Specify the number of subpass
		renderPassInfo.subpassCount = 1;
		// [COMMENT] Specify the subpass we defined previously
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		// [COMMENT] Create the render pass and check for errors
		if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("failed to create render pass!");
	}

	void SwapChain::CreateFramebuffers() 
	{
		// [COMMENT] There must be one framebuffer for each image in the swap chain
		swapChainFramebuffers.resize(ImageCount());
		// [COMMENT] Loop through the swap chain images and create a framebuffer for each
		for (size_t i = 0; i < ImageCount(); i++) 
		{
			// [COMMENT] Get the depth, color image from the swap chain and color image for multisampling views
			std::array<VkImageView, 3> attachments = { colorImageViews[i], depthImageViews[i], swapChainImageViews[i] };

			VkExtent2D swapChainExtent = GetSwapChainExtent();
			// [COMMENT] Create the image's framebuffer
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			// [COMMENT] Specify the render pass we already defined
			framebufferInfo.renderPass = renderPass;
			// [COMMENT] Specify the number of attachments
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			// [COMMENT] Specify the attachments
			framebufferInfo.pAttachments = attachments.data();
			// [COMMENT] Specify the width and height of the framebuffer
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			// [COMMENT] Specify the number of layers in the image (always 1 unless doing stereoscopic 3D)
			framebufferInfo.layers = 1;

			// [COMMENT] Create the framebuffer and check for errors
			if (vkCreateFramebuffer(device.device(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void SwapChain::CreateDepthResources() 
	{
		// [COMMENT] Find a suitable depth format
		VkFormat depthFormat = FindDepthFormat();
		swapChainDepthFormat = depthFormat;
		// [COMMENT] Get the swap chain extent (used for the imageInfo)
		VkExtent2D swapChainExtent = GetSwapChainExtent();

		// [COMMENT] We need the same number of depth images as color images
		depthImages.resize(ImageCount());
		depthImageMemorys.resize(ImageCount());
		depthImageViews.resize(ImageCount());

		// [COMMENT] Loop through the depth images and create them
		for (int i = 0; i < depthImages.size(); i++) 
		{
			// [COMMENT] ImageInfo struct to create the image
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = device.GetMaxUsableSampleCount();
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;
			
			// [COMMENT] Create the depth image
			device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				depthImages[i],
				depthImageMemorys[i]);

			// [COMMENT] Struct used to create the depth image view
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = depthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			// [COMMENT] Create the depth image view
			if (vkCreateImageView(device.device(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create texture image view!");
		}
	}

	void SwapChain::CreateColorResources()
	{
		// [COMMENT] Get the swap chain format
		VkFormat colorFormat = GetSwapChainImageFormat();
		// [COMMENT] Get the swap chain extent
		VkExtent2D swapChainExtent = GetSwapChainExtent();
		// [COMMENT] We need the same number of color images as depth images
		colorImages.resize(ImageCount());
		colorImageMemorys.resize(ImageCount());
		colorImageViews.resize(ImageCount());

		// [COMMENT] Loop through the color images and create them
		for (int i = 0; i < colorImages.size(); i++)
		{
			// [COMMENT] ImageInfo struct to create the image
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = colorFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageInfo.samples = device.GetMaxUsableSampleCount();
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;
			// [COMMENT] Create the color image
			device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				colorImages[i],
				colorImageMemorys[i]);
			// [COMMENT] Struct used to create the color image view
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = colorImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = colorFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			// [COMMENT] Create the color image view
			if (vkCreateImageView(device.device(), &viewInfo, nullptr, &colorImageViews[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create texture image view!");
		}
	}

	void SwapChain::CreateSyncObjects() 
	{
		// [COMMENT] Resize the vectors to hold the maximum number of frames in flight
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		imagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

		// [COMMENT] Required to create semaphores
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		// [COMMENT] Required to create fences
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		// [COMMENT] Since we wait for this to be signaled, the first frame will wait indefinitely fo something that will never happen
		// [COMMENT] We can fix this by setting the initial state of the fence to signaled
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		// [COMMENT] Create the semaphores and fences for every possible frame in flight, check for errors
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			if (vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		// [COMMENT] Each VkSurfaceFormatKHR contains a format (color channels and types) and a colorSpace (the organisation of colors ex:SRGB)
		// [COMMENT] Loop the list of available formats to find the one we want
		for (const auto& availableFormat : availableFormats) 
		{
			// [COMMENT] If we found the format we wanted, this is the one we choose
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
				return availableFormat;
		}

		// [COMMENT] If we found none of the formats we wanted, just pick the first one
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
	{
		// [COMMENT] Loop the list of available present mode to find the one we want
		for (const auto& availablePresentMode : availablePresentModes) 
		{
			// [COMMENT] If we found the present mode we wanted, this is the one we choose
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				std::cout << "Present mode: Mailbox" << std::endl;
				return availablePresentMode;
			}
		}

		std::cout << "Present mode: V-Sync" << std::endl;
		// [COMMENT] VK_PRESENT_MODE_FIFO_KHR is guaranteed to be available
		// [COMMENT] If the preffered present modes are not supported, this is the one we will pick
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
	{
		// [COMMENT] [TODO] What the fuck is this?
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else 
		{
			VkExtent2D actualExtent = windowExtent;
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkFormat SwapChain::FindDepthFormat() 
	{
		// [COMMENT] Usses FindSupportedFormat to get the depth format we want
		return device.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

}
