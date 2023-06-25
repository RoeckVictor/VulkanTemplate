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
		uniformBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uniformBuffers.size(); i++)
		{
			uniformBuffers[i] = std::make_unique<Buffer>
				(
					device,
					sizeof(UniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uniformBuffers[i]->map();
		}

		uint32_t maxObjectsPerFrame = 4;
		uint32_t maxSets = SwapChain::MAX_FRAMES_IN_FLIGHT * maxObjectsPerFrame;

		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(maxSets + SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets + SwapChain::MAX_FRAMES_IN_FLIGHT)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build());

		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build());

		// Create the descriptor sets
		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uniformBuffers[i]->descriptorInfo();

			DescriptorWriter(*globalSetLayouts[0], *globalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}
		std::vector<VkDescriptorSetLayout> setLayouts;
		for (int i = 0; i < globalSetLayouts.size(); i++)
			setLayouts.push_back(globalSetLayouts[i]->GetDescriptorSetLayout());

	}

	void VulkanContext::BeginFrame()
	{
		VkCommandBuffer commandBuffer = GetRenderer().BeginFrame();
		GetRenderer().BeginSwapChainRenderPass(commandBuffer);
	}

	void VulkanContext::EndFrame()
	{
		GetRenderer().EndSwapChainRenderPass(GetRenderer().GetCurrentCommandBuffer());
		GetRenderer().EndFrame();
	}
}