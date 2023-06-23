#include "Mfepch.h"
#include "VulkanContext.h"

namespace MyFirstEngine
{
	VulkanContext::VulkanContext(VulkanGlfwWindow* window)
		: window(window),
		  device(*window),
		  renderer(*window, device)
	{
		
	}

	VulkanContext::~VulkanContext()
	{

	}

	void VulkanContext::Init()
	{
		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device).Build());
	}

	void VulkanContext::SwapBuffers()
	{

	}
}